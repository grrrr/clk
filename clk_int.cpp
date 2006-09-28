/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk.h"

namespace clk {

class Sync
    : public flext_base
    , public Master
{
    FLEXT_HEADER(Sync,flext_base)

public:
    Sync(int argc,const t_atom *argv)
        : Master(argc,argv)
    {
    }

	void m_set(double y) { settime(Time(),y); }

    void m_double(float a,float b)  { m_set((double)a+(double)b); }
};

FLEXT_LIB_V("clk.sync",Sync)

} // namespace
