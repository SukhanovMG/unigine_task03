// cl13 /O2 /DWHO=my.cpp /EHsc task03rip.cpp

#include <math.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <list>

using namespace std::chrono;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef long long llong;

struct vec3
{
	float x, y, z;
	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
};

struct Sprite
{
	vec3 pos;
	uint color; // bgra
};

struct IParticle : public Sprite
{
	vec3 velocity;
	int start_tick; // it was created on this tick
	int end_tick; // it must die on that tick
};

struct Settings
{
	int max_emission_tick; // when to stop emitting (one-off vs constantly-running systems)
	float emission_delay_min; // counted in ticks (ie. active frames), for simplicity
	float emission_delay_max;
	vec3 velocity_min;
	vec3 velocity_max;
	int ttl_min;
	int ttl_max;
	uint color_min;
	uint color_max;
	int fadeout_ticks;

	Settings()
		: max_emission_tick(INT_MAX)
		, emission_delay_min(0.33f)
		, emission_delay_max(0.5f)
		, velocity_min(-1.0f, -1.0f, -1.0f)
		, velocity_max(1.0f, 1.0f, 1.0f)
		, ttl_min(800)
		, ttl_max(1200)
		, color_min(0xff808080)
		, color_max(0xffffffff)
		, fadeout_ticks(10)
	{}
};

/// System as in "particle system"
template<typename ITER>
class ISystem
{
public:
	typedef ITER ITER_TYPE;

public:
	explicit ISystem(vec3 system_pos, const Settings & s);
	void SetPos(vec3 new_pos); // move the entire system in the world
	virtual ~ISystem() {}

	void Tick();
	int GetCount();
	void GetData(Sprite *buf);

protected:
	Settings st;
	int tick; // different in different systems, because they might get stopped!
	float emit_tick;
	int count;
	vec3 pos;

protected:
	virtual ITER Create() = 0;
	virtual ITER Kill(ITER p) = 0;
	virtual ITER GetFirst() = 0;
	virtual ITER GetEnd() = 0;
	virtual ITER GetNext(ITER p) = 0;
};

template<typename ITER>
ISystem<ITER>::ISystem(vec3 system_pos, const Settings & s)
	: st(s)
	, tick(0)
	, emit_tick(0.0f)
	, count(0)
	, pos(system_pos)
{}

template<typename ITER>
void ISystem<ITER>::SetPos(vec3 new_pos)
{
	pos = new_pos;
}

float frand()
{
	return float(rand()) / RAND_MAX;
}

float flerp(float a, float b, float k)
{
	return a + (b - a) * k;
}

uchar blerp(uchar a, uchar b, float k)
{
	return a + uchar((b - a) * k);
}

int ilerp(int a, int b, float k)
{
	return a + int((b - a) * k);
}

template<typename ITER>
void ISystem<ITER>::Tick()
{
	// do emission
	tick++;
	while (emit_tick < tick)
	{
		ITER_TYPE p = Create();
		// pos is zero, which is okay

		p->color = 0;
		float k = frand();
		for (int i = 0; i < 32; i += 8)
			p->color |= blerp((st.color_min >> i) & 0xff, (st.color_max >> i) & 0xff, k);

		k = frand();
		p->velocity.x = flerp(st.velocity_min.x, st.velocity_max.x, k);
		p->velocity.y = flerp(st.velocity_min.y, st.velocity_max.y, k);
		p->velocity.z = flerp(st.velocity_min.z, st.velocity_max.z, k);

		p->start_tick = tick;
		p->end_tick = tick + ilerp(st.ttl_min, st.ttl_max, frand());

		emit_tick += flerp(st.emission_delay_min, st.emission_delay_max, frand());
	}

	// do updates (including kills)
	ITER_TYPE p = GetFirst();
	ITER_TYPE end = GetEnd();
	while (p != end)
	{
		// do kills
		if (p->end_tick <= tick)
		{
			p = Kill(p);
			continue;
		}

		// do updates
		p->pos.x += p->velocity.x;
		p->pos.y += p->velocity.y;
		p->pos.z += p->velocity.z;

		// next customer
		p = GetNext(p);
	}
}

template<typename ITER>
int ISystem<ITER>::GetCount()
{
	return count;
}

///////////////////////////////////////////////////////////////////////////

struct Particle; // : public IParticle;
class System; // : public ISystem<IParticle*>;
class StdSystem; // : public ISystem< std::list<IParticle>::iterator >;

///////////////////////////////////////////////////////////////////////////

#define S1(arg) #arg
#define S2(arg) S1(arg)

#include S2(WHO)
/* ... your code here, by means of /DWHO=xxxx.cpp ...*/

///////////////////////////////////////////////////////////////////////////

template<typename SYSTEM_TYPE>
void BenchSystem()
{
	vec3 p(1.0f, 2.0f, 3.0f);
	Settings st;

	auto tm = steady_clock::now();
	llong t = 0;
	{
		SYSTEM_TYPE s(p, st);
		srand(0);
		for (int f = 0; f < 100000; f++)
		{
			s.Tick();
			t += s.GetCount();
		}
	}
	auto dt = duration_cast<duration<double>>(steady_clock::now() - tm).count();
	printf("total %lld in %.1f ms\n", t, dt*1000);
}

int main(int, char **)
{
	BenchSystem<System>();
	BenchSystem<StdSystem>();
}
