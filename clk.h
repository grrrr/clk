/* 
clk - syncable clocking objects

Copyright (c)2006-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

$LastChangedRevision$
$LastChangedDate$
$LastChangedBy$
*/

#ifndef __CLK_H
#define __CLK_H

#define FLEXT_ATTRIBUTES 1

#include <flext.h>
#include <set>
#include <map>
#include <vector>
#include <stdexcept>
#include <iostream>

#define SLIDING 10
#define MAXSLIDING 1000

#ifdef BIGNUM
#include <gmp.h>
#include <mpfr.h>
#endif

namespace clk {


inline double Time() { return flext::GetTime(); }

class Master;
class Client;

#ifdef BIGNM
class BigNum
{
public:
    BigNum(double d = 0)
    {
        mpfr_init_set_d(&x,d,GMP_RNDN);
    }

/*
    BigNum &operator =(const BigNum &b)
    {
        mpfr_set(&x,&b.x,GMP_RNDN);
        return *this;
    }
*/
    BigNum &operator +=(const BigNum &b)
    {
        mpfr_add(&x,&x,&b.x,GMP_RNDN);
        return *this;
    }

    BigNum operator +(const BigNum &b)
    {
        return BigNum(*this);
    }

    operator double() const 
    {
        return mpfr_get_d(&x,GMP_RNDN);
    }

private:
    mpfr_t x;
};
#else
typedef double BigNum;
#endif


template <class T>
class SlidingAvg
{
public:
    SlidingAvg(int sz)
        : sum(0),pos(0),maxsz(sz)
    {
    }

    operator T() const 
    {
        return sum/val.size();
    }

    SlidingAvg &clear()
    {
        sum = 0;
        val.resize(0);
        pos = 0;
        return *this;
    }

    size_t size() const { return maxsz; }

    SlidingAvg &resize(size_t sz)
    {
        if(sz < val.size()) {
            // preserve values
            rotate((int)pos);
            pos = 0;
            val.resize(sz);
        }
        maxsz = sz;
        return recalc();
    }

    SlidingAvg &rotate(int k)
    {
        const int n = (int)size();
        if(k < 0 || k >= n) {
            k %= n;
            if (k < 0) k += n;
        }
        if (k == 0) return *this;

        int c = 0;
        for(int v = 0; c < n; v++) {
            int t = v, tp = v + k;
            T tmp = val[v];
            c++;
            while (tp != v) {
                val[t] = val[tp];
                t = tp;
                tp += k;
                if (tp >= n) tp -= n;
                c++;
            }
            val[t] = tmp;
        }

        return *this;
    }

    SlidingAvg &recalc()
    {
        sum = 0;
        for(std::vector<T>::const_iterator it = val.begin(); it != val.end(); ++it)
            sum += *it;
        return *this;
    }

    SlidingAvg operator +=(T v)
    {
        if(val.size() == maxsz) {
            sum += v-val[pos];
            val[pos] = v;
            if(++pos >= val.size()) pos = 0;
        }
        else {
            sum += v;
            val.push_back(v);
        }
        return *this;
    }

    T sum;
    std::vector<T> val;
    size_t pos,maxsz;
};

template <class T>
static std::ostream &operator <<(std::ostream &s,SlidingAvg<T> &sa)
{
    s << &sa << ":" << sa.val.size() << "(" << sa.maxsz << ") - " << sa.pos << "[";
    for(std::vector<T>::const_iterator it = sa.val.begin(); it != sa.val.end(); ++it)
        s << *it << ' ';
    return s << "]";
}



class Clock
{
public:
	void Set(double x,double y,bool pre = false);

    double Get(double x) const { return a+b*x; } 

	double Current() const { return Get(Time()); }

	double Offset() const { return a; }
	double Factor() const { return b; }

	void Factor(double f)
    {
        reset();
        n = -1;
        b = f;
    }

    float Precision() const { return precision; }
    void Precision(float f) { precision = f; }

    float Weight() const 
    { 
#ifndef SLIDING
        return weight; 
#else
        return 1-pow(10.f,-1.f/(float)s.size());
#endif
    }

    void Weight(float w) 
    { 
#ifndef SLIDING
        weight = w; 
#else
        int sz;
        if(w < 0)
            sz = MAXSLIDING;
        else if(w >= 1)
            sz = 1;
        else
            sz = std::max(std::min((int)(-1/log10(1-w)+0.5),MAXSLIDING),1);
//        std::cerr << sx << std::endl;
        s.resize(sz);
        sx.resize(sz); sy.resize(sz);
        sxx.resize(sz); sxy.resize(sz);
//        std::cerr << sx << std::endl;
#endif
    }

    const t_symbol *const name;


    static Clock *Register(const t_symbol *n,Client *c);
    static void Unregister(Clock *clk,Client *c);

    static Clock *Register(const t_symbol *n,Master *m);
    static void Unregister(Clock *clk,Master *m);

    typedef std::set<Client *> Clients;

    const Master *GetMaster() const { return master; }
    const Clients &GetClients() const { return clients; }

private:

    int n;
    double a,b;
    double prex,prey;

#ifndef SLIDING
    BigNum s,sx,sy,sxx,sxy;
    float weight;
#else
    SlidingAvg<BigNum> s,sx,sy,sxx,sxy;
#endif

    Clock(const t_symbol *n,Master *m = NULL)
        : name(n),master(m)
        , precision(1.e-10f)
#ifndef SLIDING
        , weight(0.5)
#else
        , s(SLIDING),sx(SLIDING),sy(SLIDING),sxx(SLIDING),sxy(SLIDING)
#endif
    { 
        reset(); 
    }

    ~Clock() 
    { 
        FLEXT_ASSERT(!master);
        FLEXT_ASSERT(clients.empty()); 
    }

	void reset()
	{
        n = 0;
		a = b = 0;
#ifndef SLIDING
        s = sx = sy = sxx = sxy = 0;
#else
        s.clear();
        sx.clear(); sy.clear();
        sxx.clear(); sxy.clear();
#endif
	}

    void add(double x,double y)
    { 
        ++n;
#ifndef SLIDING
	    float w = weight,iw = 1.f-w;
	    s = s*iw+w;
	    sx = sx*iw+x*w;
	    sy = sy*iw+y*w;
	    sxx = sxx*iw+x*x*w;
	    sxy = sxy*iw+x*y*w;
#else
        s += 1;
        sx += x;
        sy += y;
        sxx += x*x;
        sxy += x*y;
#endif
	    BigNum d = s*sxx-sx*sx;
        if(LIKELY(d)) {
    	    a = (sxx*sy-sx*sxy)/d;
	        b = (s*sxy-sx*sy)/d;
        }
        else
            a = b = 0;

//        fprintf(stderr,"%i: %lf %lf %lf %lf %lf, %lf -> %lf %lf\n",n,(double)s,(double)sx,(double)sy,(double)sxx,(double)sxy,d,a,b);
    }

    Master *master;
    float precision;

    Clients clients;

    typedef std::map<const t_symbol *,Clock *> Clocks;
    static Clocks clocks;

    static void TryFree(Clock *clk);
};



class ExcSyntax: public std::runtime_error { public: ExcSyntax(): runtime_error("Syntax error") {} };
class ExcExisting: public std::runtime_error { public: ExcExisting(): runtime_error("Name already existing") {} };


class Parent
    : public flext
{
protected:

    Parent(): clock(NULL) {}

	void mg_timebase(float &t) const 
    { 
        if(LIKELY(clock)) { 
            float f = static_cast<float>(clock->Factor()); 
            t = LIKELY(f)?1.f/f:0; 
        } 
        else 
            t = 0; 
    }

	void ms_timebase(float t)
    { 
        if(LIKELY(clock) && LIKELY(t))
            clock->Factor(1.f/t);
    }

	void mg_precision(float &p) const 
    { 
        p = LIKELY(clock)?clock->Precision():0;
    }

	void ms_precision(float p)
    {
        if(LIKELY(clock)) clock->Precision(p);
    }

	Clock *clock;
};

} // namespace

#endif
