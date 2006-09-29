/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#define FLEXT_ATTRIBUTES 1

#include "clk.h"

namespace clk {

class Sync
    : public flext_base
    , public Master
{
    FLEXT_HEADER_S(Sync,flext_base,Setup)

public:
    Sync(int argc,const t_atom *argv)
        : Master(argc,argv)
    {
    }

	void m_set(double y) { settime(Time(),y); }

    void m_double(float a,float b)  { m_set((double)a+(double)b); }

protected:
	FLEXT_CALLBACK_F(m_set)
	FLEXT_CALLBACK_FF(m_double)

    FLEXT_ATTRVAR_F(weight)

    static void Setup(t_classid c)
    {
		FLEXT_CADDMETHOD(c,0,m_set);

        FLEXT_CADDMETHOD_FF(c,0,sym_list,m_double);
		FLEXT_CADDMETHOD_FF(c,0,"double",m_double);

        FLEXT_CADDATTR_VAR1(c,"weight",weight);
    }
};

FLEXT_LIB_V("clk.sync",Sync)

} // namespace
