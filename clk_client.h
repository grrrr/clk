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
protected:
	Client(int argc,const t_atom *argv)
        : offset(0),factor(1)
    {
		const t_symbol *n;
		if(!argc)
			n = NULL;
		else if(argc == 1 && IsSymbol(argv[0]))
			n = GetSymbol(argv[0]);
		else 
            throw ExcSyntax();

		ms_name(n);
    }

    ~Client()
    {
		ms_name(NULL);
    }

	void ms_name(const t_symbol *n)
	{
        if(clock) {
            if(clock->name == n) return;

            Clock::Unregister(clock,this);
            clock = NULL;
        }
        else if(!n || n == sym__)
            return;

		if(n) {
            clock = Clock::Register(n,this);
            m_reset();
		}
	}

	void mg_name(const t_symbol *&n) { n = clock?clock->name:sym__; }

	void m_reset() { offset = clock?-clock->current():0; } 

	double current(double offs = 0) const { return (clock->get(Time()+offs)+offset)*factor; }

	Timer timer;

	double offset,factor;
};

class ClientExt
    : public flext_dsp
    , public Client
{
    FLEXT_HEADER_S(ClientExt,flext_dsp,Setup)

public:
    ClientExt(int argc,const t_atom *argv)
        : Client(argc,argv)
        , dblprec(false)
        , t3mode(false)
    {
        AddInSignal();
		AddOutAnything(); // for sync'd timebase
		AddOutAnything(); // for logical time and correction

		setcnv();
    }

	void m_get(double offs = 0) 
	{ 
        if(!clock) return;

		ToSysFloat(1,(float)(offs*1000.));
		if(dblprec) {
			t_atom dbl[2];
			ToSysList(0,2,mkdbl(dbl,current(offs)));
		}
		else
			ToSysFloat(0,(float)current(offs)); 
	} 

	void ms_factor(float f) 
	{ 
		if(f > 0) 
			factor = f; 
		else
			post("%s - factor must be > 0",thisName()); 
	} 


protected:

	virtual bool CbDsp() { setcnv(); return false; }

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

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_VAR(c,"name",mg_name,ms_name);

		FLEXT_CADDMETHOD_(c,0,"reset",m_reset);

		FLEXT_CADDATTR_VAR1(c,"offset",offset);
		FLEXT_CADDATTR_VAR(c,"factor",factor,ms_factor);

		FLEXT_CADDATTR_GET(c,"timebase",mg_timebase);

		FLEXT_CADDATTR_VAR1(c,"dblprec",dblprec);
		FLEXT_CADDATTR_VAR1(c,"t3mode",t3mode);
    }

	bool dblprec;
    bool t3mode;
    double ticks2s;
};

} // namespace

#endif
