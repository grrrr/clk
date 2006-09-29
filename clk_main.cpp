/* 
clk - syncable clocking objects

Copyright (c)2006 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  
*/

#include "clk.h"

#define CLK_VERSION "0.1"

namespace clk {

static void clk_main()
{
	flext::post("-------------------------------");
	flext::post("clk - syncable clocking objects");
    flext::post("version " CLK_VERSION " (c)2006 Thomas Grill");
#ifdef FLEXT_DEBUG
    flext::post("");
    flext::post("DEBUG BUILD - " __DATE__ " " __TIME__);
#endif
	flext::post("-------------------------------");

	// call the objects' setup routines
	FLEXT_SETUP(Internal);
	FLEXT_SETUP(Sync);
	FLEXT_SETUP(Tap);
	FLEXT_SETUP(Metro);
	FLEXT_SETUP(Delay);
}

// setup the library
FLEXT_LIB_SETUP(clk,clk_main)


#if 0

/*! 
*/
class Clock:
	public flext_dsp
{
	FLEXT_HEADER_S(Clock,flext_dsp,setup)

public:
	Clock(int argc,const t_atom *argv)
		: name(NULL)
		, offset(0)
		, factor(1)
		, data(NULL)
		, weight(0.5)
		, taps(0)
		, internal(false)
		, t3mode(false)
		, dblprec(false)
	{
		const t_symbol *n;
		if(!argc)
			n = NULL;
		else if(argc == 1 && IsSymbol(argv[0]))
			n = GetSymbol(argv[0]);
		else {
			post("Syntax: %s [name]",thisName()); 
			InitProblem();
			return;
		}
	
		FLEXT_ADDTIMER(timer,CbTimer);
	
		AddInSignal();
		AddOutAnything(); // for sync'd timebase
		AddOutAnything(); // for logical time and correction
		
		ms_name(n);
		
		setcnv();
	}

    ~Clock()
    {
		ms_name(NULL);
    }

	void ms_name(const t_symbol *n)
	{
		if(n != name) {
			if(data && !data->pop()) {
				clocks.erase(name);
				delete data; data = NULL;
			}
			
			if(n) {
				Clocks::iterator it = clocks.find(n);
				if(it != clocks.end())
					(data = it->second)->push();
				else
					clocks[n] = data = new Data;
			}
			
			name = n;
		}
	}
	
	void mg_name(const t_symbol *&n) { n = name?name:sym__; }
	

	void m_clear() { if(!internal && data) data->reset(); taps = 0; }
	
	void m_set(double y) { if(!internal) settime(reftime(),y); }

	void m_double(float a,float b)  { m_set((double)a+(double)b); }
	
	void m_tap() { m_set(taps++); }


	void m_get(double offs = 0) 
	{ 
		ToSysFloat(1,offs*1000.);
		if(dblprec) {
			t_atom dbl[2];
			ToSysList(0,2,mkdbl(dbl,current()));
		}
		else
			ToSysFloat(0,current()); 
	} 

	
	void m_metro(double intv,double offs = 0,bool metro = true) 
	{ 
		if(data) {
			double dur = intv/(data->Factor()*factor);
			double realdur;
			if(t3mode) {
				double dticks = (dur+offs)/ticks2s;
				int iticks = (int)dticks;
				tickoffs = (dticks-iticks)*ticks2s;
				realdur = iticks*ticks2s;
			}
			else {
				tickoffs = 0;
				realdur = dur;
			}
			
			reentered = false;

			if(metro) {
				perintv = dur;
				m_get(); // immediate bang ... re-entering might happen here!
			}
			else
				perintv = 0;
				
			// schedule
			if(!reentered) timer.Delay(realdur); 
			
			reentered = true;
		} 
	}

	void m_metro2(float intv1,float intv2) { m_metro((double)intv1+(double)intv2); }
	
	void m_delay(double intv,double offs = 0) { m_metro(intv,offs,false); }
	
	void m_delay2(float intv1,float intv2) { m_delay((double)intv1+(double)intv2); }
	
	void m_stop() { timer.Reset(); }


	void m_reset() { offset = data?-data->get(reftime()):0; } 
	
	
	void ms_internal(bool i) 
	{
		if(internal != i) {
			m_clear();
			internal = i;
			if(i && data) {
				data->set(0,0,0.5);
				data->set(1,1000,0.5);
			}
		}
	}

	void ms_factor(float f) 
	{ 
		if(f > 0) 
			factor = f; 
		else
			post("%s - factor must be > 0",thisName()); 
	} 


	void mg_timebase(float &t) const { if(data) { float f = data->Factor(); t = f?1/f:0; } else t = 0; }
	
private:

	inline double reftime() const { return GetTime(); }

	void CbTimer(void *) 
	{ 
		reentered = false;
		m_get(tickoffs);  // ... re-entering might happen here!
		if(perintv && !reentered) {
			// we are a metro... reschedule
			if(t3mode) {
				double dticks = (perintv+tickoffs)/ticks2s;
				int iticks = (int)dticks;
				tickoffs = (dticks-iticks)*ticks2s;
				timer.Delay(iticks*ticks2s);
			}
			else {
				tickoffs = 0;
				timer.Delay(perintv);
			}
		}
	}
	
	virtual bool CbDsp() { setcnv(); return false; }

	void setcnv() { ticks2s = (double)Blocksize()/(double)Samplerate(); }

	void settime(double x,double y) { data->set(x,y/factor-offset,weight); }

	double gettime(double x) const { return (data->get(x)+offset)*factor; }

	double current() const { return gettime(reftime()); }
	
	static t_atom *mkdbl(t_atom *dbl,double d)
	{
		float f = (float)d;
		SetFloat(dbl[0],f);
		SetFloat(dbl[1],(float)(d-f));
		return dbl;
	}

	FLEXT_CALLBACK(m_clear)
	FLEXT_CALLBACK(m_get)
	FLEXT_CALLBACK_F(m_set)
	FLEXT_CALLBACK_FF(m_double)
	FLEXT_CALLBACK(m_tap)

	FLEXT_CALLBACK_F(m_metro)
	FLEXT_CALLBACK_FF(m_metro2)
	FLEXT_CALLBACK_F(m_delay)
	FLEXT_CALLBACK_FF(m_delay2)
	FLEXT_CALLBACK(m_stop)

	FLEXT_CALLBACK_T(CbTimer)

	FLEXT_CALLSET_S(ms_name)
	FLEXT_CALLGET_S(mg_name)

	FLEXT_ATTRVAR_F(weight)

	FLEXT_CALLGET_F(mg_timebase)
	FLEXT_ATTRVAR_B(dblprec)

	FLEXT_CALLSET_B(ms_internal)
	FLEXT_ATTRGET_B(internal)

	FLEXT_ATTRVAR_B(t3mode)

	FLEXT_CALLBACK(m_reset)

	FLEXT_ATTRVAR_F(offset)
	FLEXT_ATTRGET_F(factor)
	FLEXT_CALLSET_F(ms_factor)

	static void setup(t_classid c)
	{
		FLEXT_CADDMETHOD_(c,0,"clear",m_clear);
		FLEXT_CADDMETHOD(c,0,m_get);
		FLEXT_CADDMETHOD(c,0,m_set);
		FLEXT_CADDMETHOD_FF(c,0,sym_list,m_double);
		FLEXT_CADDMETHOD_FF(c,0,"double",m_double);
		FLEXT_CADDMETHOD_(c,0,"tap",m_tap);

		FLEXT_CADDMETHOD_F(c,0,"metro",m_metro);
		FLEXT_CADDMETHOD_FF(c,0,"metro",m_metro2);
		FLEXT_CADDMETHOD_F(c,0,"delay",m_delay);
		FLEXT_CADDMETHOD_FF(c,0,"delay",m_delay2);
		FLEXT_CADDMETHOD_(c,0,"stop",m_stop);

		FLEXT_CADDMETHOD_(c,0,"reset",m_reset);

		FLEXT_CADDATTR_VAR(c,"name",mg_name,ms_name);

		FLEXT_CADDATTR_GET(c,"timebase",mg_timebase);
		FLEXT_CADDATTR_VAR1(c,"dblprec",dblprec);
		FLEXT_CADDATTR_VAR(c,"internal",internal,ms_internal);
		FLEXT_CADDATTR_VAR1(c,"t3mode",t3mode);

		FLEXT_CADDATTR_VAR1(c,"weight",weight);
		FLEXT_CADDATTR_VAR1(c,"offset",offset);
		FLEXT_CADDATTR_VAR(c,"factor",factor,ms_factor);
    }
	
	const t_symbol *name;
	Data *data;
	double offset,factor;
	float weight;
	int taps;
	bool internal,t3mode;
	bool dblprec;
	Timer timer;
	double ticks2s,perintv,tickoffs;
	bool reentered;

	static Clocks clocks;
};

Clocks Clock::clocks;

#endif

} // namespace
