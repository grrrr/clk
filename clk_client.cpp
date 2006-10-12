/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk_client.h"

namespace clk {

Client::Client(int argc,const t_atom *argv)
    : offset(0),factor(1)
{
	const t_symbol *n;
	if(!argc)
		n = NULL;
	else if(argc == 1 && IsSymbol(argv[0]))
		n = GetSymbol(argv[0]);
	else 
        throw ExcSyntax();

	ms_name(n);
}

Client::~Client()
{
	ms_name(NULL);
}

void Client::ms_name(const t_symbol *n)
{
    if(clock) {
        if(clock->name == n) return;

        Clock::Unregister(clock,this);
        clock = NULL;
    }
    else if(!n || n == sym__)
        return;

	if(n) {
        clock = Clock::Register(n,this);
        m_reset();
	}
}


ClientExt::ClientExt(int argc,const t_atom *argv)
    : Client(argc,argv)
    , dblprec(false)
    , t3mode(false)
{
    AddInSignal();
	AddOutAnything(); // for sync'd timebase
	AddOutAnything(); // for logical time and correction

	setcnv();
}

void ClientExt::m_get(double offs) 
{ 
    if(!clock) return;

	ToSysFloat(1,(float)(offs*1000.));
	if(dblprec) {
		t_atom dbl[2];
		ToSysList(0,2,mkdbl(dbl,current(offs)));
	}
	else
		ToSysFloat(0,(float)current(offs)); 
} 

bool ClientExt::CbDsp() { setcnv(); return false; }

void ClientExt::Setup(t_classid c)
{
	FLEXT_CADDATTR_VAR(c,"name",mg_name,ms_name);

	FLEXT_CADDMETHOD_(c,0,"reset",m_reset);

	FLEXT_CADDATTR_VAR1(c,"offset",offset);
	FLEXT_CADDATTR_VAR(c,"factor",factor,ms_factor);

	FLEXT_CADDATTR_GET(c,"timebase",mg_timebase);

	FLEXT_CADDATTR_VAR1(c,"dblprec",dblprec);
	FLEXT_CADDATTR_VAR1(c,"t3mode",t3mode);
}


} // namespace

