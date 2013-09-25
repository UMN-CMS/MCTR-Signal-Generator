#include <iostream>

using namespace std;

class Packet
{
    public:
        Packet() 
        {
            for(int i=0; i<12;i++)
                data[i]=0;

            //set bc0
            data[1] = 1;
        }
        ~Packet() {} 
    private:
        char data[12];

        void set_bits(int byte_num, int start, int low_bit, int hi_bit, int value)
        {
            cout << "value:";
            output_char(value);
            char in_mask = make_mask(low_bit,hi_bit);

            cout << "Mask:";
            output_char(in_mask);
            cout << "masked value: ";
            output_char(in_mask & value);
            cout << "masked value shifted: ";
            output_char(((in_mask & value) << start) >> low_bit);
            char out_mask = ~make_mask(start,start+(hi_bit-low_bit +1));
            data[byte_num] = (((in_mask & value) << start) >> low_bit) + (out_mask & data[byte_num]);
        }

        void output_char(char value)
        {
            for(int j=7; j>=0;j--)
                cout << ((value & (1 << j)) != 0);
            cout << endl;
        }

        char make_mask(int low_bit, int hi_bit)
        {
            char mask=0;
            for(int i = low_bit; i <= hi_bit;i++)
                mask+=(1 << i);
            return mask;
        }

    public:
        void set_adc(int qie,char value)
        {
            data[qie+2]=value;
        }
        void set_tdc(int qie, char value)
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

        void print_data()
        {
            for(int i=0; i<12;i++)
            {
                cout << i << ": ";
                output_char(data[i]);
            }
        }

};

int main()
{
    Packet mypacket;
    
    mypacket.set_adc(0,255);
    mypacket.set_adc(1,255);
    mypacket.set_adc(2,255);
    mypacket.set_adc(3,255);
    mypacket.set_adc(4,255);
    mypacket.set_adc(5,255);
    mypacket.set_tdc(0,63);
    mypacket.set_tdc(1,63);
    mypacket.set_tdc(2,63);
    mypacket.set_tdc(3,63);
    mypacket.set_tdc(4,63);
    mypacket.set_tdc(5,63);


    mypacket.print_data();
}

