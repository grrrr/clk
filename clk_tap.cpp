/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#define FLEXT_ATTRIBUTES 1

#include "clk.h"

namespace clk {

class Tap
    : public flext_base
    , public Master
{
    FLEXT_HEADER_S(Tap,flext_base,Setup)

public:
    Tap(int argc,const t_atom *argv)
        : Master(argc,argv)
    {
    }

protected:
	FLEXT_ATTRVAR_F(weight)

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_VAR1(c,"weight",weight);
    }
};

FLEXT_LIB_V("clk.tap",Tap)

} // namespace
