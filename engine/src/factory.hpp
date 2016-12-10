/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef FACTORY_H
#define FACTORY_H
#include <map>
#include <string>

template <class T>
void* constructor() { return (void*)new T(); }

typedef void*(*constructor_t)();

struct factory
{
	typedef std::map<std::string, constructor_t> map_type;
	map_type m_classes;

	template <class T>
	void register_class(std::string const& n)
	{
		m_classes.insert(std::make_pair(n, &constructor<T>));
	}

	void* construct(std::string const& n)
	{
		auto i = m_classes.find(n);
		if (i == m_classes.end()) return 0;
		return i->second();
	}
};

extern factory g_factory, g_programs;

#define REGISTER_CLASS(n) g_factory.register_class<n>(#n)
#define REGISTER_PROGS(n) g_programs.register_class<n>(#n)

#endif
