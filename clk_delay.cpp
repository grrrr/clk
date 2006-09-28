/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk.h"

namespace clk {

class Delay
    : public flext_dsp
    , public Client
{
    FLEXT_HEADER(Delay,flext_dsp)

public:
    Delay(int argc,const t_atom *argv)
        : Client(argc,argv)
    {
    }
};

FLEXT_LIB_V("clk.delay",Delay)

} // namespace
