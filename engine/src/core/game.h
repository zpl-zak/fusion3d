// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef MYGAME_H
#define MYGAME_H

#include "entity.h"
#include "coreEngine.h"
#include "profiling.h"

/*
 * This is our base game class, which also contains our scene graph.
 */
class Game
{
public:
	Game() {}
	virtual ~Game() {}

	virtual void Init(const Window& window) {}
	void ProcessInput(const Input& input, float delta);
	void Update(float delta);
	void Render(RenderingEngine* renderingEngine);
	
	inline double DisplayInputTime(double dividend) { return m_inputTimer.DisplayAndReset("Input Time: ", dividend); }
	inline double DisplayUpdateTime(double dividend) { return m_updateTimer.DisplayAndReset("Update Time: ", dividend); }
	
	inline void SetEngine(CoreEngine* engine) { m_root.SetEngine(engine); }
	inline Entity* AddToScene (Entity* child) { m_root.AddChild (child); child->InitAll (); return child; }
protected:
	Entity       m_root;
private:
	Game(Game& game) {}
	void operator=(Game& game) {}
	
	ProfileTimer m_updateTimer;
	ProfileTimer m_inputTimer;
};

#endif
