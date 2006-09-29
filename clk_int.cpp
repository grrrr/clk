/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#define FLEXT_ATTRIBUTES 1

#include "clk.h"

namespace clk {

class Internal
    : public flext_base
    , public Master
{
    FLEXT_HEADER_S(Internal,flext_base,Setup)

public:
    Internal(int argc,const t_atom *argv)
        : Master(argc,argv)
    {
    }

protected:

    static void Setup(t_classid c)
    {
    }
};

FLEXT_LIB_V("clk.int",Internal)

} // namespace
