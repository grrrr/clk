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
{
    FLEXT_HEADER(Delay,flext_dsp)
};

FLEXT_LIB("clk.delay",Delay)

} // namespace
