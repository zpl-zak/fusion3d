/*
* Copyright (C) 2015 Subvision Studio
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
#if !defined(MAP_LOADER_H)

#include "../staticLibs/tinyxml2.h"
#include <string>

class Entity;

// Tell the TIXML to use Standard Library for XML handling
#define TIXML_USE_STL 

/**
 * Handles serialized scene graph structure.
 */
class MapLoader
{
public:
	MapLoader (std::string map);
	

	inline ~MapLoader()
	{
		delete m_doc;
	}

	void LoadEntities (Entity* root, tinyxml2::XMLElement* sibling = nullptr);

private:
	tinyxml2::XMLDocument*		m_doc;
	tinyxml2::XMLElement*		m_node;
	tinyxml2::XMLElement*		m_meta;
	tinyxml2::XMLElement*		m_data;
	tinyxml2::XMLElement*		m_entities;
	std::string				m_map;

	std::string				m_name, m_version, m_author;
};

#define MAP_LOADER_H
#endif