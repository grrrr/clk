/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#ifndef __CLK_MASTER_H
#define __CLK_MASTER_H

#include "clk.h"

namespace clk {

class Master
	: public Parent
{
    friend class Clock;

protected:
    Master(int argc,const t_atom *argv);
    ~Master();

    void reset() 
    { 
        pre = true;
    }

	void settime(double x,double y) 
    { 
        if(clock)
            clock->Set(x,y,weight,pre); 
        pre = false;
    }

    void setcurrent(double y) { settime(Time(),y); }

    bool pre;
	float weight;
};


class MasterExt
    : public flext_base
    , public Master
{
    FLEXT_HEADER_S(MasterExt,flext_base,Setup)

public:
    MasterExt(int argc,const t_atom *argv);

	virtual void m_reset();
	
protected:

	double current() const { return clock->Current(); }

	FLEXT_CALLGET_F(mg_timebase)

	FLEXT_CALLVAR_F(mg_precision,ms_precision)

    static void Setup(t_classid c);
};

} // namespace

#endif
