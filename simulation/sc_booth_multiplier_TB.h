/*******************************************************************************
Vendor: GoodKook, goodkook@gmail.com
Associated Filename: sc_booth_multiplier_TB.h
Purpose: Testbench for 8bit booth_multiplier
Revision History: Jul. 1, 2024
*******************************************************************************/
#ifndef _SC_BOOTH_MULTIPLIER_TB_H_
#define _SC_BOOTH_MULTIPLIER_TB_H_

#include <systemc.h>
#include "Vbooth_multiplier.h"
#ifdef CO_EMULATION
#include "booth_multiplier.h"
#endif

SC_MODULE(sc_booth_multiplier_TB)
{
    // Verilator treats all Verilog's vector as <uint32_t>
    sc_clock                clk;
    sc_signal<uint32_t>     multiplier;     // input
    sc_signal<uint32_t>     multiplicand;   // input
    sc_signal<uint32_t>     product;        // output

    // Verilated DUT or Foreign Verilog
    Vbooth_multiplier*      u_Vbooth_multiplier;
    
#ifdef CO_EMULATION
    booth_multiplier*       u_booth_multiplier;
    sc_signal<sc_uint<8> >  multiplier_emu;
    sc_signal<sc_uint<8> >  multiplicand_emu;
    sc_signal<sc_uint<16> > product_emu;
#endif

    // Convert to exact DUT ports' vector width
    sc_signal<sc_uint<8> >  multiplier_dut;
    sc_signal<sc_uint<8> >  multiplicand_dut;
    sc_signal<sc_uint<16> > product_dut;
    // Convert Verilator's ports to DUT's ports
    void conv_method()
    {
        multiplier_dut.write((sc_uint<8>)multiplier.read());
        multiplicand_dut.write((sc_uint<8>)multiplicand.read());
        product_dut.write((sc_uint<16>)product.read());
#ifdef CO_EMULATION
        multiplier_emu.write((sc_uint<8>)multiplier.read());
        multiplicand_emu.write((sc_uint<8>)multiplicand.read());
#endif
    }

    void test_generator();
    void monitor();
    
    sc_trace_file* fp;  // VCD file

    SC_CTOR(sc_booth_multiplier_TB) :   // Constructor
        clk("clk", 100, SC_NS, 0.5, 0.0, SC_NS, false),
        multiplier("multiplier"), multiplicand("multiplicand"), product("product")
    {
        SC_THREAD(test_generator);
        sensitive << clk;

        SC_THREAD(monitor);
        sensitive << clk;

        SC_METHOD(conv_method);
        sensitive << multiplier << multiplicand << product;

        // DUT Instantiation
        u_Vbooth_multiplier = new Vbooth_multiplier("u_Vbooth_multiplier");
        // Binding
        u_Vbooth_multiplier->clk(clk);
        u_Vbooth_multiplier->multiplier(multiplier);
        u_Vbooth_multiplier->multiplicand(multiplicand);
        u_Vbooth_multiplier->product(product);
#ifdef CO_EMULATION
        u_booth_multiplier = new booth_multiplier("u_booth_multiplier");
        // Binding
        u_booth_multiplier->clk(clk);
        u_booth_multiplier->multiplier(multiplier_emu);
        u_booth_multiplier->multiplicand(multiplicand_emu);
        u_booth_multiplier->product(product_emu);
#endif

        // VCD Trace
        fp = sc_create_vcd_trace_file("sc_booth_multiplier_TB");
        fp->set_time_unit(100, SC_PS);
        sc_trace(fp, clk,              "clk");
        sc_trace(fp, multiplier_dut,   "multiplier_dut");
        sc_trace(fp, multiplicand_dut, "multiplicand_dut");
        sc_trace(fp, product_dut,      "product_dut");
#ifdef CO_EMULATION
        sc_trace(fp, multiplier_emu,   "multiplier_emu");
        sc_trace(fp, multiplicand_emu, "multiplicand_emu");
        sc_trace(fp, product_emu,      "product_emu");
#endif
    }

    // Destructor
    ~sc_booth_multiplier_TB()
    {}

};

#endif
