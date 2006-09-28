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
#include <exception>

namespace clk {


inline double Time() { return flext::GetTime(); }

class Master;
class Client;

typedef std::set<Client *> Clients;

class Clock
{
public:
    Clock(const t_symbol *n): name(n),master(NULL) { reset(); }

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

	double current() const { return get(Time()); }

	double Offset() const { return a; }
	double Factor() const { return b; }

    const t_symbol *const name;

    void Unregister(Client *c) 
    {
        clients.erase(c);
    }

protected:

    Master *master;
    Clients clients;

    double a,b,s,sx,sy,sxx,sxy;
};


typedef std::map<const t_symbol *,Clock *> Clocks;



class ExcSyntax: public exception { public: ExcSyntax(): exception("Syntax error") {} };
class ExcExisting: public exception { public: ExcExisting(): exception("Name already existing") {} };


class Parent
    : public flext
{
public:

protected:

    static Clocks clocks;
};


class Master
	: public Parent
{
public:

protected:
    Master(int argc,const t_atom *argv)
    {
        if(argc != 1 || !IsSymbol(*argv))
            throw ExcSyntax();

        const t_symbol *name = GetSymbol(*argv);

        //////////////////////////////////////////////
    }

    ~Master()
    {
        /////////////////////////////////////////////
    }

	void settime(double x,double y) { clock->set(x,y,weight); }

	Clock *clock;

	float weight;
};


class Client
	: public Parent
{
protected:
	Client(int argc,const t_atom *argv)
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

    ~Client()
    {
		ms_name(NULL);
    }

	void ms_name(const t_symbol *n)
	{
        if(clock) {
            if(clock->name == n) return;

            clock->Unregister(this);
            clock = NULL;
        }
        else if(!n || n == sym__)
            return;

		if(n) {
            //////////////////////////////////////
		}
	}

	void mg_name(const t_symbol *&n) { n = clock?clock->name:sym__; }


	double current() const { return (clock->current()+offset)*factor; }

	static t_atom *mkdbl(t_atom *dbl,double d)
	{
		float f = (float)d;
		SetFloat(dbl[0],f);
		SetFloat(dbl[1],(float)(d-f));
		return dbl;
	}

    Clock *clock;
	Timer timer;

	double offset,factor;
};


} // namespace

#endif
