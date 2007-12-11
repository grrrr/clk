/* 
clk - syncable clocking objects

Copyright (c)2006-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

$LastChangedRevision$
$LastChangedDate$
$LastChangedBy$
*/

#include "clk_master.h"

namespace clk {

class Internal
    : public MasterExt
{
    FLEXT_HEADER(Internal,MasterExt)

public:
    Internal(int argc,const t_atom *argv)
        : MasterExt(argc,argv)
    {
		settime(0,0);
		settime(1,1000);
    }
};

FLEXT_LIB_V("clk.int, clk",Internal)

} // namespace
