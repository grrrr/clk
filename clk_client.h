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
    ~Client();

    virtual void Update(double told,double tnew) = 0;

    void ms_name(const t_symbol *n);

	void mg_name(const t_symbol *&n) { n = clock?clock->name:sym__; }

	void m_reset() { offset = clock?-clock->Current():0; } 

	double current(double offs = 0) const { return (clock->Get(Time()+offs)+offset)*factor; }

	Timer timer;

	double offset,factor;
};

class ClientExt
    : public flext_dsp
    , public Client
{
    FLEXT_HEADER_S(ClientExt,flext_dsp,Setup)

public:
    ClientExt(int argc,const t_atom *argv);

	void m_get(double offs = 0);

	void ms_factor(float f) 
	{ 
		if(f > 0) 
			factor = f; 
		else
			post("%s - factor must be > 0",thisName()); 
	} 


protected:

	virtual bool CbDsp();

	void setcnv() { ticks2s = (double)Blocksize()/(double)Samplerate(); }


	FLEXT_CALLSET_S(ms_name)
	FLEXT_CALLGET_S(mg_name)

	FLEXT_CALLBACK(m_reset)

    FLEXT_ATTRVAR_F(offset)
	FLEXT_ATTRGET_F(factor)
	FLEXT_CALLSET_F(ms_factor)

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
