/* 
clk - syncable clocking objects

Copyright (c)2006-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

$LastChangedRevision$
$LastChangedDate$
$LastChangedBy$
*/

#include "clk_master.h"

namespace clk {

class Internal
    : public MasterExt
{
    FLEXT_HEADER_S(Internal,MasterExt,Setup)

public:
    Internal(int argc,const t_atom *argv)
        : MasterExt(argc,argv)
        , intv(-1),cnt(0)
    {
		FLEXT_ADDTIMER(check,CbCheck);
    }

    virtual bool Init()
    {
        ms_interval(0);
        return MasterExt::Init();
    }

    void ms_interval(float i)
    {
        FLEXT_ASSERT(clock);

        if(intv == i) return;

        if(i) {
            if(!intv) {
                cnt = 0;
                reset();   
            }
            check.Delay(0);
        }
        else {
            cnt = 0;
            check.Reset();
        }
        intv = i;

        const double t = clock->Time();
    	settime(cnt++,t-1);
	    settime(cnt++,t);
    }

	void CbCheck(void * = NULL) 
	{ 
        FLEXT_ASSERT(clock);
		settime(cnt++,clock->Time());
        check.Delay(intv*1000);
	}

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_VAR(c,"interval",intv,ms_interval);
    }

    Timer check;
    double intv;
    int cnt;

	FLEXT_CALLSET_F(ms_interval)
	FLEXT_ATTRGET_F(intv)
    FLEXT_CALLBACK_T(CbCheck)
};

FLEXT_LIB_V("clk.int, clk",Internal)

} // namespace
