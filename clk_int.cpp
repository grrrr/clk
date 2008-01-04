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
    
        if(intv < 0) {
            // first time hint
            settime(flext::GetTime()-1,clock->Time()-1);
        }
        intv = i;

        check.Reset();

        if(intv)
            CbCheck();
        else
    	    settime(flext::GetTime(),clock->Time());
    }

	void CbCheck(void * = NULL) 
	{ 
        FLEXT_ASSERT(clock);
		settime(flext::GetTime(),clock->Time());
        check.Delay(intv*1000);
	}

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_VAR(c,"interval",intv,ms_interval);
    }

    Timer check;
    double cnt,intv;

	FLEXT_CALLSET_F(ms_interval)
	FLEXT_ATTRGET_F(intv)
    FLEXT_CALLBACK_T(CbCheck)
};

FLEXT_LIB_V("clk.int, clk",Internal)

} // namespace
