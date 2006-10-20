/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#ifndef __CLK_CLIENT_H
#define __CLK_CLIENT_H

#include "clk.h"

namespace clk {

class Client
	: public Parent
{
    friend class Clock;

protected:
	Client(int argc,const t_atom *argv);
    virtual ~Client();

    virtual void Update(double told,double tnew) = 0;

    void ms_name(const t_symbol *n);

	void mg_name(const t_symbol *&n) { n = LIKELY(clock)?clock->name:sym__; }

	void m_reset() { offset = LIKELY(clock)?-clock->Current():0; } 

	double Convert(double time) const { return (time+offset)*factor; }
	double Current(double offs = 0) const { return Convert(clock->Get(Time()+offs)); }

    void Factor(double f) 
    {
        if(LIKELY(clock)) {
            double current = clock->Current();
            offset = (current+offset)*factor/f-current;
        }
		factor = f; 
    }

    double Factor() const { return factor; }

    void Offset(double o) { offset = o; }

    double Offset() const { return offset; }

	Timer timer;

private:
	double offset,factor;
};

class ClientExt
    : public flext_dsp
    , public Client
{
    FLEXT_HEADER_S(ClientExt,flext_dsp,Setup)

    friend class MasterExt;

public:
    ClientExt(int argc,const t_atom *argv);

	void m_get(double offs = 0);

    void ms_factor(float f) 
    { 
        if(UNLIKELY(f <= 0))
		    post("%s - factor must be > 0",thisName()); 
        else if(LIKELY(Factor() != f)) {
            Factor(f);
            if(clock) {
                double c = clock->Current();
                Update(c,c);
            }
        }
    } 

    void mg_factor(float &f) { f = (float)Factor(); }

    void ms_offset(float o) 
    { 
        if(LIKELY(Offset() != o)) {
            Offset(o); 
            if(clock) {
                double c = clock->Current();
                Update(c,c);
            }
        }
    }

    void mg_offset(float &o) { o = (float)Offset(); }

    void m_message(int argc,const t_atom *argv) { Forward(sym_message,argc,argv); }

protected:

    static const t_symbol *sym_message;

    void Forward(const t_symbol *sym,int argc,const t_atom *argv);
    void Message(const t_symbol *sym,int argc,const t_atom *argv) { ToOutAnything(GetOutAttr(),sym,argc,argv); }

	virtual bool CbDsp();

	void setcnv() { ticks2s = (double)Blocksize()/(double)Samplerate(); }


	FLEXT_CALLSET_S(ms_name)
	FLEXT_CALLGET_S(mg_name)

	FLEXT_CALLBACK(m_reset)
	FLEXT_CALLBACK_V(m_message)

    FLEXT_CALLVAR_F(mg_offset,ms_offset)
	FLEXT_CALLVAR_F(mg_factor,ms_factor)

	FLEXT_CALLGET_F(mg_timebase)

	FLEXT_ATTRVAR_B(dblprec)
	FLEXT_ATTRVAR_B(t3mode)

    static void Setup(t_classid c);

	bool dblprec;
    bool t3mode;
    double ticks2s;
};

} // namespace

#endif
