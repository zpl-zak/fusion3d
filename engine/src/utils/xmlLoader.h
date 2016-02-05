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

#include "../staticLibs/tinyxml2.h"
#include <string>
#include <fstream>
#include <cassert>

using namespace tinyxml2;

class XMLLoader
{
public:
	XMLLoader(std::string document)
		:
			m_document(document)
	{}
	
	~XMLLoader()
	{
		delete m_xmlDocument;
	}

	XMLLoader* LoadXML()
	{
		if (m_xmlDocument->LoadFile(m_document.c_str()))
		{
			printf("XML parser failed!");
			assert(0 == 1);
		}

		return this;
	};

	XMLLoader* SaveXML()
	{
		
		return this;
	}

	XMLDocument*  GetXMLDocument() { return m_xmlDocument; }
	void		 SetXMLDocument(XMLDocument* xmlDocument) { m_xmlDocument = xmlDocument; }

private:
	XMLDocument*		m_xmlDocument;
	std::string		m_document;
};
