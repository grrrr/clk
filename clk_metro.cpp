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
        , scheduled(-1)
    {
		FLEXT_ADDTIMER(timer,CbTimer);
    }

	void m_metro(double intv,double offs = 0) 
	{ 
		if(LIKELY(clock)) {
            tickoffs = 0;
            SchedDelay(perintv = intv);
        }
	}

	void m_metro2(float intv1,float intv2) { m_metro((double)intv1+(double)intv2); }
	
	void m_stop() 
    { 
        timer.Reset(); 
        scheduled = -1;
    }

protected:

    virtual void Update(double told,double tnew)
    {
        FLEXT_ASSERT(clock);

        if(scheduled < 0) return; // clock not set

        double time = (tnew+offset)*factor;
        double still = scheduled-time;

//        post("%lf: time=%lf",Current(),time);

        while(UNLIKELY(still < 0)) {
//            post("Missed!");
            // we missed the time already... output immediately!
            m_get();
            still += perintv;
        }

        // schedule new delay
        SchedDelay(still,false);
    }

	void CbTimer(void *) 
	{ 
        SchedDelay(perintv);
	}

    void SchedDelay(double intv,bool bang = true)
    {
        FLEXT_ASSERT(clock);

		reentered = false;

		if(bang) m_get(tickoffs);  // bang out
        // Through the outlet in m_get we might re-enter this function.
        // "reentered" is false then and the following block is executed and a new delay scheduled.
        // After this block has been executed "reentered" is true, so we won't execute it again upon return from recursion

		if(LIKELY(!reentered)) {
			double dur = intv/(clock->Factor()*factor);
			// reschedule
			if(t3mode) {
				double dticks = (dur+tickoffs)/ticks2s;
				int iticks = (int)dticks;
				tickoffs = (dticks-iticks)*ticks2s;
                // recalculate dur
				dur = iticks*ticks2s;
			}
			else
				tickoffs = 0;

			timer.Delay(dur);

            double cur = Current();
            scheduled = cur+intv;
//            post("%lf: Scheduled for +%lf = %lf",cur,dur,scheduled);
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
	double scheduled,perintv,tickoffs;
    bool reentered;
};

FLEXT_LIB_V("clk.metro",Metro)

} // namespace
