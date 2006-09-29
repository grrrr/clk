/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
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
    {
		clock->set(0,0,0.5);
		clock->set(1,1000,0.5);
    }

protected:

    static void Setup(t_classid c)
    {
    }
};

FLEXT_LIB_V("clk.int",Internal)

} // namespace
