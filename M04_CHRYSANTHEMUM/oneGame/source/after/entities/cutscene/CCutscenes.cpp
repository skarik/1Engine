#include "CCutscene.h"

Cutscene* Cutscene::active = nullptr;

Cutscene::Cutscene()
{
	active = this;
	paused = true;
	activeSeq = nullptr;
}

Cutscene::~Cutscene()
{
	if(active == this)
	{
		active = nullptr;
	}
}

void	Cutscene::startScene(Animation::Sequence* name)
{
	activeSeq = name;
}
void	Cutscene::pauseScene()
{
	paused = true;
}
void	Cutscene::resumeScene()
{
	paused = false;
}
void	Cutscene::endScene()
{
	paused = true;
	delete activeSeq;
}
void	Cutscene::Update()
{
	if(paused == false && activeSeq != nullptr)
	{
		activeSeq->Update();
	}
	
}