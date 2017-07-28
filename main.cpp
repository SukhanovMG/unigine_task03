struct Particle : public IParticle
{
	Particle() : prev(nullptr), next(nullptr) {}
	struct Particle *prev, *next;
};

template <typename T>
class List
{
public:
	List() : m_first(nullptr), m_last(nullptr) {}
	
	T* begin() { return m_first; }
	T* end() { return m_last; }
	void push_back(T*);
	T* emplace_back();
	void remove(T*);
	T* erase(T*);
private:
	T *m_first, *m_last;
};

template <typename T>
void List<T>::push_back(T *elem)
{
	if (elem == nullptr)
		return;
	if (nullptr == m_first)
	{
		elem->next = nullptr;
		elem->prev = nullptr;
		m_first = elem;
		m_last = elem;
	}
	else
	{
		m_last->next = elem;
		elem->next = nullptr;
		elem->prev = m_last;
		m_last = elem;
	}
}

template <typename T>
T* List<T>::emplace_back()
{
	T *new_elem = new T();
	push_back(new_elem);
	return new_elem;	
}

template <typename T>
void List<T>::remove(T* elem)
{
	if (nullptr == elem)
		return;

	if (nullptr == elem->prev)
		m_first = elem->next;
	else
		elem->prev->next = elem->next;

	if (nullptr == elem->next)
		m_last = elem->prev;
	else
		elem->next->prev = elem->prev;	
}

template <typename T>
T* List<T>::erase(T* elem)
{
	if (nullptr == elem)
		return nullptr;

	T* next = elem->next;

	remove(elem);

	delete elem;
	return next;
}

class System : public ISystem<Particle*>
{
public:
	explicit System(vec3 system_pos, const Settings & s);
private:
	List<Particle> pool_lst;
	List<Particle> lst;

	virtual Particle* Create() override;
	virtual Particle* Kill(Particle* p) override;
	virtual Particle* GetFirst() override;
	virtual Particle* GetEnd() override;
	virtual Particle* GetNext(Particle* p) override;
};

System::System(vec3 system_pos, const Settings & s)
	: ISystem<Particle*>(system_pos, s)
{
	for (int i = 0; i < 1000; i++)
	{
		pool_lst.emplace_back();
	}
}

Particle* System::Create()
{
	count++;
	if (pool_lst.begin() != nullptr)
	{
		Particle* new_elem = pool_lst.end();
		pool_lst.remove(new_elem);
		lst.push_back(new_elem);
		return new_elem;
	}
	else
		return lst.emplace_back();
}

Particle* System::Kill(Particle* p)
{
	if (p == nullptr)
		return nullptr;
	count--;
	Particle* next = p->next;
	lst.remove(p);
	pool_lst.push_back(p);
	return next;
}

Particle* System::GetFirst()
{
	return lst.begin();
}

Particle* System::GetEnd()
{
	Particle* last = lst.end();
	if (last == nullptr)
		return nullptr;
	else
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
