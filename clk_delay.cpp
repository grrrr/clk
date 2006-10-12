/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_client.h"

namespace clk {

class Delay
    : public ClientExt
{
    FLEXT_HEADER_S(Delay,ClientExt,Setup)

public:
    Delay(int argc,const t_atom *argv)
        : ClientExt(argc,argv)
    {
		FLEXT_ADDTIMER(timer,CbTimer);
    }

	void m_delay(double intv,double offs = 0) 
	{ 
		if(clock) {
			double dur = intv/(clock->Factor()*factor);
			double realdur;
			if(t3mode) {
				double dticks = (dur+offs)/ticks2s;
				int iticks = (int)dticks;
				tickoffs = (dticks-iticks)*ticks2s;
				realdur = iticks*ticks2s;
			}
			else {
				tickoffs = 0;
				realdur = dur;
			}
			
			// schedule
			timer.Delay(realdur); 
		} 
	}

	void m_delay2(float intv1,float intv2) { m_delay((double)intv1+(double)intv2); }
	
	void m_stop() { timer.Reset(); }

protected:

    virtual void Update(double told,double tnew)
    {
        thisParent::Update(told,tnew);
    }

	void CbTimer(void *) 
	{ 
		m_get(tickoffs);
	}

	FLEXT_CALLBACK_F(m_delay)
	FLEXT_CALLBACK_FF(m_delay2)
	FLEXT_CALLBACK(m_stop)

	FLEXT_CALLBACK_T(CbTimer)

    static void Setup(t_classid c)
    {
		FLEXT_CADDMETHOD(c,0,m_delay);
		FLEXT_CADDMETHOD_FF(c,0,sym_list,m_delay2);
		FLEXT_CADDMETHOD_(c,0,"stop",m_stop);
    }

	Timer timer;
	double tickoffs;
};

FLEXT_LIB_V("clk.delay",Delay)

} // namespace
