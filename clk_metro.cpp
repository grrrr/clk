/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#define FLEXT_ATTRIBUTES 1

#include "clk.h"

namespace clk {

class Metro
    : public flext_dsp
    , public Client
{
    FLEXT_HEADER_S(Metro,flext_dsp,Setup)

public:
    Metro(int argc,const t_atom *argv)
        : Client(argc,argv)
    {
    }

protected:

	FLEXT_CALLSET_S(ms_name)
	FLEXT_CALLGET_S(mg_name)

    static void Setup(t_classid c)
    {
		FLEXT_CADDATTR_VAR(c,"name",mg_name,ms_name);
    }
};

FLEXT_LIB_V("clk.metro",Metro)

} // namespace
