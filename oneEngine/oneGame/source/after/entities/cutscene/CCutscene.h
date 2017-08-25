#ifndef _CCutScene_H_
#define _CCutScene_H_

#include "after/animation/Sequence.h"
#include "engine/behavior/CGameBehavior.h"

class Cutscene : public CGameBehavior
{

	public:
		Cutscene();
		~Cutscene();

		void	startScene(Animation::Sequence* name);
		void	pauseScene();
		void	resumeScene();
		void	endScene();
		void	Update() override;

		static	Cutscene* active;

	private:

		Animation::Sequence* activeSeq;
		bool paused;
};


#endif