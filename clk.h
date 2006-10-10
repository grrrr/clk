/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#ifndef __CLK_H
#define __CLK_H

#define FLEXT_ATTRIBUTES 1

#include <flext.h>
#include <set>
#include <map>
#include <exception>

namespace clk {


inline double Time() { return flext::GetTime(); }

class Master;
class Client;


class Clock
{
public:
	void Set(double x,double y,float weight,bool pre = false);

    double Get(double x) const { return a+b*x; } 

	double Current() const { return Get(Time()); }

	double Offset() const { return a; }
	double Factor() const { return b; }

    const t_symbol *const name;


    static Clock *Register(const t_symbol *n,Client *c);
    static void Unregister(Clock *clk,Client *c);

    static Clock *Register(const t_symbol *n,Master *m);
    static void Unregister(Clock *clk,Master *m);

private:

    int n;
    double a,b,s,sx,sy,sxx,sxy;
    double prex,prey;
    float prew;

    Clock(const t_symbol *n,Master *m = NULL)
        : name(n),master(m) 
    { 
        reset(); 
    }

    ~Clock() 
    { 
        FLEXT_ASSERT(!master);
        FLEXT_ASSERT(clients.empty()); 
    }

    void Update(double told,double tnew);

	void reset()
	{
        n = 0;
		a = b = s = sx = sy = sxx = sxy = 0;
	}

    void add(double x,double y,float w)
    { 
	    float iw = 1.f-w;
	    s = s*iw+w;
	    sx = sx*iw+x*w;
	    sy = sy*iw+y*w;
	    sxx = sxx*iw+x*x*w;
	    sxy = sxy*iw+x*y*w;
        ++n;

	    double d = s*sxx-sx*sx;
	    a = (sxx*sy-sx*sxy)/d;
	    b = (s*sxy-sx*sy)/d;
    }

    Master *master;

    typedef std::set<Client *> Clients;
    Clients clients;

    typedef std::map<const t_symbol *,Clock *> Clocks;
    static Clocks clocks;

    static void TryFree(Clock *clk);
};




class ExcSyntax: public std::exception { };
class ExcExisting: public std::exception { };


class Parent
    : public flext
{
protected:

    Parent(): clock(NULL) {}

	static t_atom *mkdbl(t_atom *dbl,double d)
	{
		float f = (float)d;
		SetFloat(dbl[0],f);
		SetFloat(dbl[1],(float)(d-f));
		return dbl;
	}

	void mg_timebase(float &t) const 
    { 
        if(clock) { 
            float f = (float)clock->Factor(); 
            t = f?1.f/f:0; 
        } 
        else 
            t = 0; 
    }

	Clock *clock;
};

} // namespace

#endif
