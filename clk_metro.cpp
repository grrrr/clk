/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk.h"

namespace clk {

class Metro
    : public flext_dsp
{
    FLEXT_HEADER(Metro,flext_dsp)
};

FLEXT_LIB("clk.metro",Metro)

} // namespace
