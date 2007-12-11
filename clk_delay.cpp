/* 
clk - syncable clocking objects

Copyright (c)2006-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

$LastChangedRevision$
$LastChangedDate$
$LastChangedBy$
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
        , scheduled(-1)
    {
		FLEXT_ADDTIMER(timer,CbTimer);
    }

	void m_delay(double intv,double offs = 0) 
	{ 
        if(LIKELY(clock)) {
            double dur = intv/(clock->Factor()*Factor());

		    if(t3mode) {
			    double dticks = (dur+offs)/ticks2s;
			    int iticks = (int)dticks;
			    tickoffs = (dticks-iticks)*ticks2s;
			    dur = iticks*ticks2s;
		    }
		    else
                tickoffs = 0;
    		
		    // schedule
		    timer.Delay(dur); 

            double cur = Current();
            scheduled = cur+intv;
//            post("%lf: Scheduled for +%lf = %lf",cur,dur,scheduled);
        }
	}

	void m_delay2(float intv1,float intv2) { m_delay((double)intv1+(double)intv2); }
	
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

        if(UNLIKELY(still < 0)) {
            m_stop();

            if(LIKELY(missedmsg)) {
                ToOutAnything(GetOutAttr(),sym_missed,0,NULL);
                // we missed the time already... output immediately!
                m_get();
            }
        }
        else {
//            post("Reschedule in %lf!",still);

            // schedule new delay
            m_delay(still);
        }
    }

	void CbTimer(void *) 
	{ 
		m_get(tickoffs);
        scheduled = -1;
	}

	FLEXT_CALLBACK_F(m_delay)
	FLEXT_CALLBACK_FF(m_delay2)
	FLEXT_CALLBACK(m_stop)

	FLEXT_CALLBACK_T(CbTimer)

    static void Setup(t_classid c)
    {
        sym_missed = MakeSymbol("missed");

		FLEXT_CADDMETHOD(c,0,m_delay);
		FLEXT_CADDMETHOD_FF(c,0,sym_list,m_delay2);
		FLEXT_CADDMETHOD_(c,0,"stop",m_stop);
    }

	Timer timer;
	double scheduled,tickoffs;

    static const t_symbol *sym_missed;
};

const t_symbol *Delay::sym_missed;

FLEXT_LIB_V("clk.delay, clk",Delay)

} // namespace
