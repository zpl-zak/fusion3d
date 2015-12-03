#ifndef FACTORY_H
#define FACTORY_H
#include <map>

template <class T> void* constructor() { return (void*)new T(); }

struct factory
{
	typedef void*(*constructor_t)();
	typedef std::map<std::string, constructor_t> map_type;
	map_type m_classes;

	template <class T>
	void register_class(std::string const& n)
	{
		m_classes.insert(std::make_pair(n, &constructor<T>));
	}

	void* construct(std::string const& n)
	{
		map_type::iterator i = m_classes.find(n);
		if (i == m_classes.end()) return 0;
		return i->second();
	}
};

extern factory g_factory;

#define REGISTER_CLASS(n) g_factory.register_class<n>(#n)

#endif