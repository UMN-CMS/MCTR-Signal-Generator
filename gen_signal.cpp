#include <iostream>
#include "gen_signal.h"

Packet::Packet()
{
    for(int i=0; i<12;i++)
        m_data[i]=0;

    m_data[0] = (uint8_t)0xbc;
}
Packet::Packet(bool bc0) 
{
    for(int i=0; i<12;i++)
        m_data[i]=0;

    m_data[0] = (uint8_t)0xbc;
    m_data[1] = (uint8_t)bc0;
}
Packet::~Packet() {} 


void Packet::set_bits(int byte_num, int start, int low_bit, int hi_bit, int value)
{
    uint8_t in_mask = make_mask(low_bit,hi_bit);
    uint8_t out_mask = ~make_mask(start,start+(hi_bit-low_bit +1));
    m_data[byte_num] = (((in_mask & value) << start) >> low_bit) + (out_mask & m_data[byte_num]);
}

void Packet::output_binary(uint8_t value)
{
    for(int j=7; j>=0;j--)
        std::cout << (bool)(value & (1 << j));
    std::cout << std::endl;
}

uint8_t Packet::make_mask(int low_bit, int hi_bit)
{
    uint8_t mask=0;
    for(int i = low_bit; i <= hi_bit;i++)
        mask+=(1 << i);
    return mask;
}

void Packet::set_adc(int qie,uint8_t value)
{
    if(0<= qie && qie < CHANNELS_PER_FIBER)
        m_data[qie+2]=value;
    else
        std::cerr << "Invalid QIE number: " << qie << std::endl;
}
void Packet::set_tdc(int qie, uint8_t value)
{
    if(value < TDC_MAX)
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

void Packet::set_capid(int i)
{
    set_bits(1,2,0,1,i);
}

void Packet::print_binary()
{
    for(int i=0; i<12;i++)
    {
        std::cout << i << ": ";
        output_binary(m_data[i]);
    }
}
void Packet::print_hex()
{
    printf("1%02x%02x\n",m_data[1],m_data[0]);
    for(int i=1; i<6;i++)
    {
        printf("0%02x%02x\n",m_data[2*i+1],m_data[2*i]);
    }
}

void Packet::write_hex(FILE * fileout)
{
    fprintf(fileout,"1%02x%02x\n",m_data[1],m_data[0]);
    for(int i=1; i<6;i++)
    {
        fprintf(fileout,"0%02x%02x\n",m_data[2*i+1],m_data[2*i]);
    }
}

Generator::Generator()
{
    m_nfibers = 1;
    init_matrix();
    m_data_created = false;
}

Generator::Generator(int fibers) : m_nfibers(fibers)
{
    init_matrix();
    m_data_created = false;
}

Generator::~Generator() {}

void Generator::print_hex()
{
    if(!m_data_created) create_data();
    for ( int j = 0; j< m_nfibers; j++)
        for (int i = 0; i < N_BX; i++)
            packets.at(j).at(i).print_hex();
}

void Generator::print_binary()
{
    if(!m_data_created) create_data();
    for ( int j = 0; j< m_nfibers; j++)
        for (int i = 0; i < N_BX; i++)
            packets.at(j).at(i).print_binary();
}

void Generator::write_hex(char name_pattern[])
{
    if(!m_data_created) create_data();
    for ( int j = 0; j< m_nfibers; j++)
    {
        char filename[128];
        sprintf(filename,name_pattern,j);
        FILE * fileout = fopen(filename,"w");
        for (int i = 0; i < N_BX; i++)
            packets.at(j).at(i).write_hex(fileout);

        fclose(fileout);
    }
}

void Generator::get_data(int fiber, int qie, int BX, uint8_t &adc,  uint8_t &tdc) 
{
    adc = 0;
    tdc = 0;
}

void Generator::create_data()
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
    m_data_created = true;
}
void Generator::init_matrix()
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
