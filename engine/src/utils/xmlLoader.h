// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

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
