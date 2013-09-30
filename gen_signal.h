#include <vector>
#include <stdint.h>

class Packet
{
    public:
        Packet() ;
        Packet(bool bc0) ; 
        ~Packet();
    private:
        uint8_t m_data[12];

        //Sets bits value[hi_bit:low_bit] to byte_num starting at bit "start"
        void set_bits(int byte_num, int start, int low_bit, int hi_bit, int value);
        //prints value as binary
        void output_binary(uint8_t value);
        //makes a 8bit mask from low_bit to hi_bit
        uint8_t make_mask(int low_bit, int hi_bit);

    public:
        //sets adc value for channel "qie"
        void set_adc(int qie,uint8_t value);
        //sets tdc value for channel "qie"
        void set_tdc(int qie, uint8_t value);
        //sets CapId for the packet
        void set_capid(int i);

        //Prints data as binary
        void print_binary();
        //prints data as hex
        void print_hex();

        //Write hex to file
        //void write_hex(char name[]);

    private:
        //Constants
        static const int CHANNELS_PER_FIBER=6;

};


class Generator
{
    public:
        Generator();
        Generator(int fibers);
        ~Generator();

    public:
        void print_hex();
        void print_binary();
        //Write hex to file
        void write_hex(char name[]);

    protected:
        virtual void get_data(int fiber, int qie, int BX, uint8_t &adc,  uint8_t &tdc);
        void create_data();
        void init_matrix();

    private:
        //Constants
        static const int CHANNELS_PER_FIBER=6;
        static const int N_BX=2048;

        //Member variables
        std::vector< std::vector<Packet> > packets;
        int m_nfibers;
        bool m_data_created;

};


