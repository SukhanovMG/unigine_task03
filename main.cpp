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
