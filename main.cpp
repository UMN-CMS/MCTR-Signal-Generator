#include "gen_signal.h"

class special_Generator : public Generator
{
    public:
        special_Generator() : Generator(1) {}
    protected:
        void get_data(int orbit, int channel, int qie, uint8_t &adc,  uint8_t &tdc)
        {
            adc = qie;
            tdc = qie;
        }
};


int main()
{
    special_Generator mygen;
    mygen.print_hex();
}
