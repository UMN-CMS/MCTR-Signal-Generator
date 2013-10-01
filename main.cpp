#include "gen_signal.h"

class special_Generator : public Generator
{
    public:
        special_Generator() : Generator(12) {}
    protected:
        void get_data(int fiber, int qie, int BX, uint8_t &adc,  uint8_t &tdc) 
        {
            adc = fiber   + qie + BX;
            tdc = (fiber*2 + qie + BX) %  TDC_MAX;
        }
};


int main()
{
    special_Generator mygen;
    char name_pattern[] = "file%02u.txt";
    mygen.write_hex(name_pattern);
}
