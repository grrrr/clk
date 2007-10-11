/* 
clk - syncable clocking objects

Copyright (c)2006-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_client.h"
#include <math.h>

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

    virtual void Update(double told,double tnew,bool missedmsg)
    {
        FLEXT_ASSERT(clock);

        if(scheduled < 0) return; // clock not set

        double time = Convert(tnew);
        double still = scheduled-time;

//        post("%lf: time=%lf",Current(),time);

        if(LIKELY(missedmsg)) {
            while(UNLIKELY(still < 0)) {
                ToOutAnything(GetOutAttr(),sym_missed,0,NULL);

                // we missed the time already... output immediately!
                m_get();
                still += perintv;
            }
        }
        else if(UNLIKELY(still < 0)) {
            // forget all missed ticks, advance to the next future one
            still = perintv+fmod(still,perintv);
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
        // obviously the metro can have been unregistered before being re-scheduled
        if(!clock) return; 

		reentered = false;
#if 1 // tentative fix
        double outoffs = tickoffs;
#else
		if(bang) m_get(tickoffs);  // bang out
        // Through the outlet in m_get we might re-enter this function.
        // "reentered" is false then and the following block is executed and a new delay scheduled.
        // After this block has been executed "reentered" is true, so we won't execute it again upon return from recursion

        // although we might not have been reentered, the metro might have been stopped in the meantime

		if(LIKELY(!reentered) && LIKELY(scheduled >= 0)) 
#endif
        {
            double factor = clock->Factor()*Factor();
            // factor might be 0 if initialization is still in progress... we have to bail out in that case...
            if(factor <= 0) 
                post("%s - Can't schedule tick, invalid data for period",thisName());
            else
            {
			    double dur = intv/factor;
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

#if 1 // tentative fix
		if(bang) m_get(outoffs);  // bang out
#endif
    }

	FLEXT_CALLBACK_F(m_metro)
	FLEXT_CALLBACK_FF(m_metro2)
	FLEXT_CALLBACK(m_stop)

	FLEXT_CALLBACK_T(CbTimer)

    static void Setup(t_classid c)
    {
        sym_missed = MakeSymbol("missed");

		FLEXT_CADDMETHOD(c,0,m_metro);
		FLEXT_CADDMETHOD_FF(c,0,sym_list,m_metro2);
		FLEXT_CADDMETHOD_(c,0,"stop",m_stop);
    }

	Timer timer;
	double scheduled,perintv,tickoffs;
    bool reentered;

    static const t_symbol *sym_missed;
};

const t_symbol *Metro::sym_missed;

FLEXT_LIB_V("clk.metro, clk",Metro)

} // namespace
