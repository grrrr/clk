/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#ifndef __CLK_H
#define __CLK_H

#include <flext.h>
#include <set>
#include <map>

namespace clk {

class Data
{
public:
	Data(): refs(1) { reset(); }
	~Data() { FLEXT_ASSERT(!refs); }

	void push() { ++refs; }
	bool pop() { return --refs != 0; }

	void reset()
	{
		a = b = s = sx = sy = sxx = sxy = 0;
	}

	void set(double x,double y,float w) 
    { 
		float iw = 1.f-w;
		s = s*iw+w;
		sx = sx*iw+x*w;
		sy = sy*iw+y*w;
		sxx = sxx*iw+x*x*w;
		sxy = sxy*iw+x*y*w;

		double d = s*sxx-sx*sx;
		a = (sxx*sy-sx*sxy)/d;
		b = (s*sxy-sx*sy)/d;
    }

	double get(double x) const { return a+b*x; } 

	double Offset() const { return a; }
	double Factor() const { return b; }

protected:
	int refs;
	double a,b,s,sx,sy,sxx,sxy;
};

typedef std::map<const t_symbol *,Data *> Clocks;


class Client;

typedef std::set<Client *> Clients;

class Master
	: public flext
{
public:

protected:
	Clients clients;
};

class Client
	: public flext
{
public:

protected:
	Master *master;
};


} // namespace

#endif
