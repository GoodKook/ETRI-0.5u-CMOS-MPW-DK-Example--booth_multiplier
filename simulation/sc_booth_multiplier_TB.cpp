/*******************************************************************************
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: sc_booth_multiplier_TB.cpp
Purpose: Testbench for 8-bit booth_multiplier
Revision History: Jun. 1, 2024
*******************************************************************************/
#include "sc_booth_multiplier_TB.h"

#include <iostream>
#include <iomanip>

#include <gsl/gsl_rng.h>

void sc_booth_multiplier_TB::test_generator() // SC_THREAD
{
    gsl_rng *r = gsl_rng_alloc (gsl_rng_taus);

    for (int n=0; n<10000; n++)
    {
        wait(clk.posedge_event());
        
        multiplier.write((int8_t)(gsl_rng_uniform (r)*256.0));
        multiplicand.write((int8_t)(gsl_rng_uniform (r)*256.0));

        if (n>10000) sc_stop();
    }

    gsl_rng_free (r);
    sc_stop();
}

void sc_booth_multiplier_TB::monitor()    // SC_THREAD
{
    static int n = 0;
    int8_t  _multiplier   = (int8_t)multiplier.read();
    int8_t  _multiplicand = (int8_t)multiplicand.read();
    int16_t _product_dut  = (int16_t)product_dut.read();
    int16_t _product_mult = _multiplicand * _multiplier;
#ifdef CO_EMULATION
    int16_t _product_emu= product_emu.read();
#endif

    while(true)
    {
        _multiplier   = (int8_t)multiplier.read();
        _multiplicand = (int8_t)multiplicand.read();
        _product_mult = _multiplicand * _multiplier;    // Reference, signed multiplier!

        wait(clk.posedge_event());

        _product_dut  = (int16_t)product_dut.read();

#ifdef CO_EMULATION
        int16_t _product_emu= product_emu.read();
        printf("Iteration=%5d, multiplier=%5d, multiplicand=%5d, product=%7d[%7d]<%7d> ", n,
                    _multiplier, _multiplicand, _product_dut, _product_mult, _product_emu);
        if (_product_dut!=_product_emu)    cout << "Error ";
#else
        printf("Iteration=%5d, multiplier=%5d, multiplicand=%5d, product=%7d[%7d] ", n,
                    _multiplier, _multiplicand, _product_dut, _product_mult);
        if (_product_mult!=_product_dut)    cout << "Error ";
#endif
        cout << "@" << sc_time_stamp() << endl;

        n++;
    }
}
