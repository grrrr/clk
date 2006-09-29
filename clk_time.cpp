/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_client.h"

namespace clk {

class Time
    : public ClientExt
{
    FLEXT_HEADER_S(Time,ClientExt,Setup)

public:
    Time(int argc,const t_atom *argv)
        : ClientExt(argc,argv)
    {
    }

    void m_float(float f) { m_get(f*0.001); }

protected:

	FLEXT_CALLBACK(m_get)
	FLEXT_CALLBACK_F(m_float)

    static void Setup(t_classid c)
    {
		FLEXT_CADDMETHOD(c,0,m_get);
		FLEXT_CADDMETHOD(c,0,m_float);
    }
};

FLEXT_LIB_V("clk.time",Time)

} // namespace
