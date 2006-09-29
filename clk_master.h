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
public:

protected:
    Master(int argc,const t_atom *argv)
        : weight(0.5)
    {
        if(argc != 1 || !IsSymbol(*argv))
            throw ExcSyntax();

        const t_symbol *name = GetSymbol(*argv);
        clock = Clock::Register(name,this);

        if(!clock) 
            throw ExcExisting();
    }

    ~Master()
    {
        Clock::Unregister(clock,this);
    }

    void reset() { if(clock) clock->reset(); }

	void settime(double x,double y) { if(clock) clock->set(x,y,weight); }

	float weight;
};


class MasterExt
    : public flext_base
    , public Master
{
    FLEXT_HEADER_S(MasterExt,flext_base,Setup)

public:
    MasterExt(int argc,const t_atom *argv)
        : Master(argc,argv)
    {
        AddInAnything();
    }

	virtual void m_reset() { reset(); }
	
protected:

	double current() const { return clock->current(); }

	FLEXT_CALLGET_F(mg_timebase)

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_GET(c,"timebase",mg_timebase);
    }
};

} // namespace

#endif
