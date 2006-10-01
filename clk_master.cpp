/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_master.h"

namespace clk {

Master::Master(int argc,const t_atom *argv)
    : weight(0.5)
    , pre(true)
{
    if(argc != 1 || !IsSymbol(*argv))
        throw ExcSyntax();

    const t_symbol *name = GetSymbol(*argv);
    clock = Clock::Register(name,this);

    if(!clock) 
        throw ExcExisting();
}

Master::~Master()
{
    Clock::Unregister(clock,this);
}


MasterExt::MasterExt(int argc,const t_atom *argv)
    : Master(argc,argv)
{
    AddInAnything();
}

void MasterExt::m_reset() 
{ 
    reset(); 
}

void MasterExt::Setup(t_classid c)
{
	FLEXT_CADDATTR_GET(c,"timebase",mg_timebase);
}

} // namespace

