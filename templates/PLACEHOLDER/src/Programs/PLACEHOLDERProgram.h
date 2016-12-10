#pragma once
#include "components/programComponent.h"
#include "components/devMode.h"

class PLACEHOLDERProgram : public Program
{
public:
	PLACEHOLDERProgram ();
	virtual ~PLACEHOLDERProgram ();

	virtual int Init ();

	virtual int ProcessInput (const Input& input, float delta);

	virtual int Update (float delta);

	virtual int Render (Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);

	virtual std::string GetProgramName ()
	{
		return "PLACEHOLDERProgram";
	}

	virtual std::string GetLatestError ()
	{
		if (m_error.size () == 0)
		{
			return "";
		}

		std::vector<std::string> error (m_error.begin (), m_error.end ());
		std::string lastError = "Unhandled exception: {\n";
		for (unsigned int i = 0; i < error.size (); i++)
		{
			lastError += " -- " + error[i] + "\n";
		}
		lastError += "};\n";

		m_error.clear ();
		return lastError;
	}
};

