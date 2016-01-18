#include "mapLoader.h"

MapLoader::MapLoader (std::string map)
	: m_map(map)
{
	m_doc = new tinyxml2::XMLDocument (true, tinyxml2::Whitespace::COLLAPSE_WHITESPACE);
	m_doc->LoadFile (m_map.c_str ());

	m_node = m_doc->FirstChildElement ("map");

	if (m_node == nullptr)
	{
		std::cout << "Invalid map! (map tag is missing)" << map;
		assert (0 == 1);
	}

	m_meta = m_node->FirstChildElement ("meta");

	if (m_meta == nullptr)
	{
		std::cout << "Invalid map! (meta tag is missing)" << map;
		assert (0 == 1);
	}

	m_name = m_meta->FirstChildElement ("name")->GetText ();
	m_author = m_meta->FirstChildElement ("author")->GetText ();
	m_version = m_meta->FirstChildElement ("version")->GetText ();


	m_entities = m_node->FirstChildElement ("entities");

	if (m_entities == nullptr)
	{
		std::cout << "Invalid map! (entities tag is missing)" << map;
		assert (0 == 1);
	}

	m_data = m_node->FirstChildElement ("data");

	if (m_data == nullptr)
	{
		std::cout << "Invalid map! (data tag is missing)" << map;
		assert (0 == 1);
	}
}

void MapLoader::LoadEntities (Entity * root, tinyxml2::XMLElement * sibling)
{
	if (sibling == nullptr)
		sibling = m_entities->FirstChildElement ("entity");
	while (sibling != nullptr)
	{
		Entity* entity = new Entity ();

		if (sibling->Attribute ("name"))
		{
			std::string name = sibling->Attribute ("name");
			entity->SetDisplayName (name);
		}
		else
		{
			entity->SetDisplayName ("undefined");
		}

		Transform trans;

		if (sibling->Attribute ("position"))
		{
			std::string position = sibling->Attribute ("position");
			std::vector<std::string> pos = Util::Split (position, ';');
			Vector3f pos_ (atof (pos[0].c_str ()), atof (pos[1].c_str ()), atof (pos[2].c_str ()));
			trans.SetPos (pos_);
		}

		if (sibling->Attribute ("rotation"))
		{
			std::string rotation = sibling->Attribute ("rotation");
			std::vector<std::string> rot = Util::Split (rotation, ';');
			Quaternion rot_ (Vector3f (atof (rot[0].c_str ()), atof (rot[1].c_str ()), atof (rot[2].c_str ())), ToRadians (atof (rot[3].c_str ())));
			trans.SetRot (rot_);
		}

		if (sibling->Attribute ("scale"))
		{
			std::string scale = sibling->Attribute ("scale");
			float scl_ = atof (scale.c_str ());
			trans.SetScale (scl_);
		}

		entity->SetTransform (trans);

		tinyxml2::XMLElement* component = sibling->FirstChildElement ("component");

		while (component != nullptr)
		{
			std::string type = component->Attribute ("type");
			EntityComponent* comp = (EntityComponent*)g_factory.construct (type);

			if (comp == nullptr)
			{
				std::cerr << "Component isn't registered in engine! Are you sure you registered it? " << type << std::endl;
				assert (1 == 0);
			}

			comp->DataDeploy (component);
			entity->AddComponent (comp);

			component = component->NextSiblingElement ("component");
		}

		tinyxml2::XMLElement* program = sibling->FirstChildElement ("program");

		while (program != nullptr)
		{
			std::string name = program->Attribute ("name");
			bool verbose = false;

			if (program->Attribute ("verbose"))
				verbose = atoi (program->Attribute ("verbose"));

			auto programObj = (Program*)g_factory.construct (name);

			if (programObj == nullptr)
			{
				std::cerr << "Program isn't registered in engine! Are you sure you registered it? " << name << std::endl;
				assert (1 == 0);
			}

			programObj->DataDeploy (program);

			ProgramHoster* hoster = new ProgramHoster (programObj, verbose);

			entity->AddComponent (hoster);

			program = program->NextSiblingElement ("program");
		}

		tinyxml2::XMLElement* entities = sibling->FirstChildElement ("entity");

		while (entities != nullptr)
		{
			LoadEntities (entity, entities);

			entities = entities->NextSiblingElement ("entity");
		}

		root->AddChild (entity);

		sibling = sibling->NextSiblingElement ("entity");
	}
}
