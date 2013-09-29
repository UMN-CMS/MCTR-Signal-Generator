#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

using namespace std;

class Packet
{
    public:
        Packet() 
        {
            for(int i=0; i<12;i++)
                data[i]=0;

            data[0] = (uint8_t)0xbc;
        }
        Packet(bool bc0) 
        {
            for(int i=0; i<12;i++)
                data[i]=0;

            data[0] = (uint8_t)0xbc;
            data[1] = (uint8_t)bc0;
        }
        ~Packet() {} 
    private:
        uint8_t data[12];

        void set_bits(int byte_num, int start, int low_bit, int hi_bit, int value)
        {
            uint8_t in_mask = make_mask(low_bit,hi_bit);
            uint8_t out_mask = ~make_mask(start,start+(hi_bit-low_bit +1));
            data[byte_num] = (((in_mask & value) << start) >> low_bit) + (out_mask & data[byte_num]);
        }

        void output_char(uint8_t value)
        {
            for(int j=7; j>=0;j--)
                cout << (bool)(value & (1 << j));
            cout << endl;
        }

        uint8_t make_mask(int low_bit, int hi_bit)
        {
            uint8_t mask=0;
            for(int i = low_bit; i <= hi_bit;i++)
                mask+=(1 << i);
            return mask;
        }

    public:
        void set_adc(int qie,uint8_t value)
        {
            data[qie+2]=value;
        }
        void set_tdc(int qie, uint8_t value)
        {
            if(value < 64)
            {
                switch(qie)
                {
                    case 0:
                        set_bits(1, 4, 0, 3, value);
                        set_bits(8, 0, 4, 5, value);
                        break;
                    case 1:
                        set_bits(8, 2, 0, 5, value);
                        break;
                    case 2:
                        set_bits(9, 0, 0, 5, value);
                        break;
                    case 3:
                        set_bits(9, 6, 0, 1, value);
                        set_bits(10, 0, 2, 5, value);
                        break;
                    case 4:
                        set_bits(10, 4, 0, 3, value);
                        set_bits(11, 0, 4, 5, value);
                        break;
                    case 5:
                        set_bits(11, 2, 0, 5, value);
                        break;
                }
            }
            else cout << "tdc value, " << (int)value << ", to high\n";
        }

        void set_capid(int i)
        {
            set_bits(1,2,0,1,i);
        }

        void print_data()
        {
            for(int i=0; i<12;i++)
            {
                cout << i << ": ";
                output_char(data[i]);
            }
        }
        void write_data()
        {
            for(int i=0; i<6;i++)
            {
                printf("%02x%02x\n",data[2*i+1],data[2*i]);
            }
        }

};


class Generator
{
    public:
        Generator()
        {
            orbits = 10;
            channels = 3;
            qies = 6;
            cout << "Constructor\n";
            init_matrix();
            create_data();
        }

        Generator(int n_orbits, int n_channels) 
        {
            orbits = n_orbits;
            channels = n_channels;
            qies = 6;
            init_matrix();
            create_data();
        }
        ~Generator() {}

    public:
        void write_data()
        {
            for ( int j = 0; j< channels; j++)
                for (int i = 0; i < orbits; i++)
                    for( int k = 0; k< qies; k++)
                        packets.at(j).at(i).write_data();
        }

        void print_data()
        {
            for ( int j = 0; j< channels; j++)
                for (int i = 0; i < orbits; i++)
                    for( int k = 0; k< qies; k++)
                        packets.at(j).at(i).print_data();
        }

    private:
        virtual int get_data(int orbit, int channel, int qie, uint8_t &adc,  uint8_t &tdc) = 0;
        void create_data()
        {
            cout << "create_data\n";
            uint8_t adc=0;
            uint8_t tdc=0;

            cout << packets.size();

            for ( int j = 0; j< channels; j++) {
                for (int i = 0; i < orbits; i++) {
                    cout << "debug: " << i<< " " << j<< endl;
                    packets.at(j).at(i).set_capid(i % 3);
                    for( int k = 0; k< qies; k++) {
                        this->get_data(i,j,k,adc,tdc);
                        packets.at(j).at(i).set_adc(k,adc);
                        packets.at(j).at(i).set_tdc(k,tdc);
                    }
                }
            }
        }
        void init_matrix()
        {
            cout << "init\n";
            for ( int j = 0; j< channels; j++)
            {
                vector<Packet> temp_vec;
                temp_vec.push_back(Packet(true));
                for (int i = 1; i < orbits; i++)
                    temp_vec.push_back(Packet());
                packets.push_back(temp_vec);
            }
        }

        vector< vector<Packet> > packets;
        int channels;
        int orbits;
        int qies;

};

class special_Generator : public Generator
{
    public:
        special_Generator() : Generator() {}
        special_Generator(int a,int b) : Generator(a,b) {}
    private:
        virtual int get_data(int orbit, int channel, int qie, uint8_t &adc,  uint8_t &tdc)
        {
            adc = 0;
            tdc = 0;
            adc = orbit + channel + qie;
            tdc = orbit + channel + qie;
        }
};



int main()
{
    special_Generator mygen;
    mygen.write_data();
    //mygen.print_data();
}

