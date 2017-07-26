template <typename T>
struct ListElem
{
	ListElem() : prev(nullptr), next(nullptr) {}
	T *prev, *next;
};

struct Particle : public IParticle, public ListElem<Particle>
{

};

class System : public ISystem<Particle*>
{
public:
	explicit System(vec3 system_pos, const Settings & s);
private:
	Particle *first, *last;

	virtual Particle* Create() override;
	virtual Particle* Kill(Particle* p) override;
	virtual Particle* GetFirst() override;
	virtual Particle* GetEnd() override;
	virtual Particle* GetNext(Particle* p) override;
};

System::System(vec3 system_pos, const Settings & s)
	: ISystem<Particle*>(system_pos, s),
	first(nullptr),
	last(nullptr)
{}

Particle* System::Create()
{
	Particle* new_particle = new Particle();
	if (nullptr == first)
	{
		first = new_particle;
		last = new_particle;
	}
	else
	{
		last->next = new_particle;
		new_particle->prev = last;
		last = new_particle;
	}
	count++;
	return new_particle;
}

Particle* System::Kill(Particle* p)
{
	if (nullptr == p)
		return nullptr;

	Particle* next = p->next;

	if (nullptr == p->prev)
		first = p->next;
	else
		p->prev->next = p->next;

	if (nullptr == p->next)
		last = p->prev;
	else
		p->next->prev = p->prev;

	count--;
	delete p;
	return next;
}

Particle* System::GetFirst()
{
	return first;
}

Particle* System::GetEnd()
{
	return last->next;
}

Particle* System::GetNext(Particle* p)
{
	return p->next;
}


// Как ни крутился не смог достать через ITER_TYPE
typedef std::list<IParticle>::iterator lst_ip_it;

class StdSystem : public ISystem<lst_ip_it>
{
public:
	explicit StdSystem(vec3 system_pos, const Settings & s);
private:
	std::list<IParticle> lst;

	virtual lst_ip_it Create() override;
	virtual lst_ip_it Kill(lst_ip_it p) override;
	virtual lst_ip_it GetFirst() override;
	virtual lst_ip_it GetEnd() override;
	virtual lst_ip_it GetNext(lst_ip_it p) override;
};

StdSystem::StdSystem(vec3 system_pos, const Settings & s)
	: ISystem<lst_ip_it>(system_pos, s)
{
}


lst_ip_it StdSystem::Create()
{
	lst.emplace_back();
	count++;
	return --lst.end();
}

lst_ip_it StdSystem::Kill(lst_ip_it p)
{
	if (p == lst.end())
		return p;
	lst_ip_it next = p;
	next++;
	lst.erase(p);
	count--;
	return next;
}

lst_ip_it StdSystem::GetFirst()
{
	return lst.begin();
}

lst_ip_it StdSystem::GetEnd()
{
	return lst.end();
}

lst_ip_it StdSystem::GetNext(lst_ip_it p)
{
	lst_ip_it next = p;
	return ++next;
}



// #include <math.h>
// #include <limits.h>
// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <chrono>

// using namespace std::chrono;

// typedef unsigned int uint;
// typedef unsigned char uchar;
// typedef long long llong;

// struct vec3
// {
// 	float x, y, z;
// 	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
// 	vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
// };

// struct Sprite
// {
// 	vec3 pos;
// 	uint color; // bgra
// };

// template <typename T>
// struct ListElement
// {
// 	T *next, *prev;
// 	ListElement() : next(nullptr), prev(nullptr) {}
// };

// struct Particle : public Sprite, public ListElement<Particle>
// {
// 	vec3 velocity;
// 	int start_tick; // it was created on this tick
// 	int end_tick; // it must die on that tick

// 	/* ... */
// };

// struct Settings
// {
// 	int max_emission_tick; // when to stop emitting (one-off vs constantly-running systems)
// 	float emission_delay_min; // counted in ticks (ie. active frames), for simplicity
// 	float emission_delay_max;
// 	vec3 velocity_min;
// 	vec3 velocity_max;
// 	int ttl_min;
// 	int ttl_max;
// 	uint color_min;
// 	uint color_max;
// 	int fadeout_ticks;

// 	Settings()
// 		: max_emission_tick(INT_MAX)
// 		, emission_delay_min(0.33f)
// 		, emission_delay_max(0.5f)
// 		, velocity_min(-1.0f, -1.0f, -1.0f)
// 		, velocity_max(1.0f, 1.0f, 1.0f)
// 		, ttl_min(800)
// 		, ttl_max(1200)
// 		, color_min(0xff808080)
// 		, color_max(0xffffffff)
// 		, fadeout_ticks(10)
// 	{}
// };

// /// System as in "particle system"
// class System
// {
// public:
// 	explicit System(vec3 system_pos, const Settings & s);
// 	void SetPos(vec3 new_pos); // move the entire system in the world
// 	~System();

// 	void Tick();
// 	int GetCount();
// 	void GetData(Sprite *buf);

// private:
// 	Settings st;
// 	int tick; // different in different systems, because they might get stopped!
// 	float emit_tick;
// 	Particle *first;
// 	Particle *last;
// 	int count;
// 	vec3 pos;

// private:
// 	Particle *Create();
// 	Particle *Kill(Particle *p);
// 	Particle *GetFirst();
// 	Particle *GetNext(Particle *p);
// };

// System::System(vec3 system_pos, const Settings & s)
// 	: st(s)
// 	, tick(0)
// 	, emit_tick(0.0f)
// 	, first(nullptr)
// 	, last(nullptr)
// 	, count(0)
// 	, pos(system_pos)
// {}

// void System::SetPos(vec3 new_pos)
// {
// 	pos = new_pos;
// }

// System::~System()
// {
// 	while (GetFirst())
// 		Kill(GetFirst());
// }

// float frand()
// {
// 	return float(rand()) / RAND_MAX;
// }

// float flerp(float a, float b, float k)
// {
// 	return a + (b - a) * k;
// }

// uchar blerp(uchar a, uchar b, float k)
// {
// 	return a + uchar((b - a) * k);
// }

// int ilerp(int a, int b, float k)
// {
// 	return a + int((b - a) * k);
// }

// void System::Tick()
// {
// 	// do emission
// 	tick++;
// 	while (emit_tick < tick)
// 	{
// 		Particle * p = Create();
// 		// pos is zero, which is okay

// 		p->color = 0;
// 		float k = frand();
// 		for (int i = 0; i < 32; i += 8)
// 			p->color |= blerp((st.color_min >> i) & 0xff, (st.color_max >> i) & 0xff, k);

// 		k = frand();
// 		p->velocity.x = flerp(st.velocity_min.x, st.velocity_max.x, k);
// 		p->velocity.y = flerp(st.velocity_min.y, st.velocity_max.y, k);
// 		p->velocity.z = flerp(st.velocity_min.z, st.velocity_max.z, k);

// 		p->start_tick = tick;
// 		p->end_tick = tick + ilerp(st.ttl_min, st.ttl_max, frand());

// 		emit_tick += flerp(st.emission_delay_min, st.emission_delay_max, frand());
// 	}

// 	// do updates (including kills)
// 	Particle *p = GetFirst();
// 	while (p)
// 	{
// 		// do kills
// 		if (p->end_tick <= tick)
// 		{
// 			p = Kill(p);
// 			continue;
// 		}

// 		// do updates
// 		p->pos.x += p->velocity.x;
// 		p->pos.y += p->velocity.y;
// 		p->pos.z += p->velocity.z;

// 		// next customer
// 		p = GetNext(p);
// 	}
// }

// int System::GetCount()
// {
// 	return count;
// }

// Particle* System::Create()
// {
// 	Particle* new_particle = new Particle();
// 	if (nullptr == first)
// 	{
// 		first = new_particle;
// 		last = new_particle;
// 	}
// 	else
// 	{
// 		last->next = new_particle;
// 		new_particle->prev = last;
// 		last = new_particle;
// 	}
// 	count++;
// 	return new_particle;
// }

// Particle* System::Kill(Particle* p)
// {
// 	if (nullptr == p)
// 		return nullptr;

// 	Particle* next = p->next;

// 	if (nullptr == p->prev)
// 		first = p->next;
// 	else
// 		p->prev->next = p->next;

// 	if (nullptr == p->next)
// 		last = p->prev;
// 	else
// 		p->next->prev = p->prev;

// 	count--;
// 	delete p;
// 	return next;
// }

// Particle* System::GetFirst()
// {
// 	return first;
// }

// Particle* System::GetNext(Particle* p)
// {
// 	return p->next;
// }

// ///////////////////////////////////////////////////////////////////////////

// /* ... */

// ///////////////////////////////////////////////////////////////////////////

// int main(int argc, char **argv)
// {
// 	vec3 p(1.0f, 2.0f, 3.0f);
// 	Settings st;

// 	auto tm = steady_clock::now();
// 	System s(p, st);
// 	llong t = 0;
// 	for (int f = 0; f < 100000; f++)
// 	{
// 		s.Tick();
// 		t += s.GetCount();
// 	}
// 	auto dt = duration_cast<duration<double>>(steady_clock::now() - tm);

// 	printf("total %lld in %.1f ms\n", t, dt*1000);
// }
