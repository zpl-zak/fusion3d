/*
* Copyright (C) 2015 Dominik Madarasz
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
#include "../3DEngine.h"
#include "../staticLibs/tinyxml2.h"
#define TIXML_USE_STL

class MapLoader
{
public:
	MapLoader(std::string map)
		:
		m_map(map)
	{
		m_doc = new tinyxml2::XMLDocument(true, tinyxml2::Whitespace::COLLAPSE_WHITESPACE);
		m_doc->LoadFile(m_map.c_str());

		m_node = m_doc->FirstChildElement("map");

		if (m_node == nullptr)
		{
			std::cout << "Invalid map! (map tag is missing)" << map;
			assert(0 == 1);
		}

		m_meta = m_node->FirstChildElement("meta");

		if (m_meta == nullptr)
		{
			std::cout << "Invalid map! (meta tag is missing)" << map;
			assert(0 == 1);
		}

		m_name = m_meta->FirstChildElement("name")->GetText();
		m_author = m_meta->FirstChildElement("author")->GetText();
		m_version = m_meta->FirstChildElement("version")->GetText();
		

		m_entities = m_node->FirstChildElement("entities");

		if (m_entities == nullptr)
		{
			std::cout << "Invalid map! (entities tag is missing)" << map;
			assert(0 == 1);
		}

		m_data = m_node->FirstChildElement("data");

		if (m_data == nullptr)
		{
			std::cout << "Invalid map! (data tag is missing)" << map;
			assert(0 == 1);
		}
	}

	~MapLoader()
	{
		delete m_doc;
	}

	void LoadEntities(Entity* root, tinyxml2::XMLElement* sibling = nullptr)
	{
		if (sibling == nullptr)
			sibling = m_entities->FirstChildElement("entity");
		while (sibling != nullptr)
		{
			Entity* entity = new Entity();
			
			if (sibling->Attribute("name"))
			{
				std::string name = sibling->Attribute("name");
				entity->SetDisplayName(name);
				std::cout << name;
			}
			else
			{
				entity->SetDisplayName("undefined");
			}

			Transform trans;

			if (sibling->Attribute("position"))
			{
				std::string position = sibling->Attribute("position");
				std::vector<std::string> pos = Util::Split(position, ';');
				Vector3f pos_(atof(pos[0].c_str()), atof(pos[1].c_str()), atof(pos[2].c_str()));
				trans.SetPos(pos_);	
			}

			if (sibling->Attribute("rotation"))
			{
				std::string rotation = sibling->Attribute("rotation");
				std::vector<std::string> rot = Util::Split(rotation, ';');
				Quaternion rot_(Vector3f(atof(rot[0].c_str()), atof(rot[1].c_str()), atof(rot[2].c_str())), ToRadians(atof(rot[3].c_str())));
				trans.SetRot(rot_);
			}

			if (sibling->Attribute("scale"))
			{
				std::string scale = sibling->Attribute("scale");
				float scl_ = atof(scale.c_str());
				trans.SetScale(scl_);
			}

			entity->SetTransform(trans);

			tinyxml2::XMLElement* component = sibling->FirstChildElement("component");

			while (component != nullptr)
			{
				std::string type = component->Attribute("type");
				EntityComponent* comp = (EntityComponent*)g_factory.construct(type);
				
				if (comp == nullptr)
				{
					std::cerr << "Component isn't registered in engine! Are you sure you registered it? " << type << std::endl;
					assert(1 == 0);
				}

				comp->DataDeploy(component);
				entity->AddComponent(comp);

				component = component->NextSiblingElement("component");
			}
			
			tinyxml2::XMLElement* entities = sibling->FirstChildElement("entity");

			while (entities != nullptr)
			{
				LoadEntities(entity, entities);

				entities = entities->NextSiblingElement("entity");
			}

			root->AddChild(entity);

			sibling = sibling->NextSiblingElement("entity");
		}
	}

private:
	tinyxml2::XMLDocument*		m_doc;
	tinyxml2::XMLElement*		m_node;
	tinyxml2::XMLElement*		m_meta;
	tinyxml2::XMLElement*		m_data;
	tinyxml2::XMLElement*		m_entities;
	std::string				m_map;

	std::string				m_name, m_version, m_author;
};