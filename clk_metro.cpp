/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_client.h"

namespace clk {

class Metro
    : public ClientExt
{
    FLEXT_HEADER_S(Metro,ClientExt,Setup)

public:
    Metro(int argc,const t_atom *argv)
        : ClientExt(argc,argv)
    {
		FLEXT_ADDTIMER(timer,CbTimer);
    }

	void m_metro(double intv,double offs = 0) 
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
			
			reentered = false;

			perintv = dur;
			m_get(); // immediate bang ... re-entering might happen here!
				
			// schedule
			if(!reentered) timer.Delay(realdur); 
			
			reentered = true;
		} 
	}

	void m_metro2(float intv1,float intv2) { m_metro((double)intv1+(double)intv2); }
	
	void m_stop() { timer.Reset(); }

protected:

	void CbTimer(void *) 
	{ 
		reentered = false;
		m_get(tickoffs);  // ... re-entering might happen here!
		if(!reentered) {
			// reschedule
			if(t3mode) {
				double dticks = (perintv+tickoffs)/ticks2s;
				int iticks = (int)dticks;
				tickoffs = (dticks-iticks)*ticks2s;
				timer.Delay(iticks*ticks2s);
			}
			else {
				tickoffs = 0;
				timer.Delay(perintv);
			}
		}
	}
	
	FLEXT_CALLBACK_F(m_metro)
	FLEXT_CALLBACK_FF(m_metro2)
	FLEXT_CALLBACK(m_stop)

	FLEXT_CALLBACK_T(CbTimer)

    static void Setup(t_classid c)
    {
		FLEXT_CADDMETHOD(c,0,m_metro);
		FLEXT_CADDMETHOD_FF(c,0,sym_list,m_metro2);
		FLEXT_CADDMETHOD_(c,0,"stop",m_stop);
    }

	Timer timer;
	double perintv,tickoffs;
    bool reentered;
};

FLEXT_LIB_V("clk.metro",Metro)

} // namespace
