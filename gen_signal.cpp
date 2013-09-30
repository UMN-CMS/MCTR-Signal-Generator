#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

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
                std::cout << (bool)(value & (1 << j));
            std::cout << std::endl;
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
            if(0<= qie && qie < CHANNELS_PER_FIBER)
                data[qie+2]=value;
            else
                std::cerr << "Invalid QIE number: " << qie << std::endl;
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
            else std::cerr << "tdc value, " << (int)value << ", to high\n";
        }

        void set_capid(int i)
        {
            set_bits(1,2,0,1,i);
        }

        void print_data()
        {
            for(int i=0; i<12;i++)
            {
                std::cout << i << ": ";
                output_char(data[i]);
            }
        }
        void write_data()
        {
            printf("1%02x%02x\n",data[1],data[0]);
            for(int i=1; i<6;i++)
            {
                printf("0%02x%02x\n",data[2*i+1],data[2*i]);
            }
        }

    private:
        //Constants
        static const int CHANNELS_PER_FIBER=6;

};


class Generator
{
    public:
        Generator()
        {
            m_nfibers = 1;
            init_matrix();
        }

        Generator(int fibers) : m_nfibers(fibers)
        {
            init_matrix();
        }
        ~Generator() {}

    public:
        void write_data()
        {
            create_data();
            for ( int j = 0; j< m_nfibers; j++)
                for (int i = 0; i < N_BX; i++)
                        packets.at(j).at(i).write_data();
        }

        void print_data()
        {
            for ( int j = 0; j< m_nfibers; j++)
                for (int i = 0; i < N_BX; i++)
                        packets.at(j).at(i).print_data();
        }

    protected:
        virtual int get_data(int fiber, int qie, int BX, uint8_t &adc,  uint8_t &tdc) 
        {
            adc = 0;
            tdc = 0;
        }

        void create_data()
        {
            uint8_t adc=0;
            uint8_t tdc=0;

            for ( int j = 0; j< m_nfibers; j++) {
                for (int i = 0; i < N_BX; i++) {
                    packets.at(j).at(i).set_capid(i % 3);
                    for( int k = 0; k< CHANNELS_PER_FIBER; k++) {
                        get_data(i,j,k,adc,tdc);
                        packets.at(j).at(i).set_adc(k,adc);
                        packets.at(j).at(i).set_tdc(k,tdc);
                    }
                }
            }
        }
        void init_matrix()
       {
            for ( int j = 0; j< m_nfibers; j++)
            {
                std::vector<Packet> temp_vec;
                temp_vec.push_back(Packet(true));
                for (int i = 1; i < N_BX; i++)
                    temp_vec.push_back(Packet(false));
                packets.push_back(temp_vec);
            }
        }

    private:
        //Constants
        static const int CHANNELS_PER_FIBER=6;
        static const int N_BX=2040;

        //Member variables
        std::vector< std::vector<Packet> > packets;
        int m_nfibers;

};

class special_Generator : public Generator
{
    public:
        special_Generator() : Generator(1) {}
    protected:
        virtual int get_data(int orbit, int channel, int qie, uint8_t &adc,  uint8_t &tdc)
        {
            adc = qie;
            tdc = 0;
        }
};



int main()
{
    special_Generator mygen;
    mygen.write_data();
    //mygen.print_data();
}

