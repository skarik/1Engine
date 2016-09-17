#ifndef _C_PERSONALITY_H_
#define _C_PERSONALITY_H_

class CPersonality
{
public:
	CPersonality();

	~CPersonality();

	//Likely going to need some arguments
	virtual void Execute() = 0;

private:
	//Knowledge
	bool mPlayerLocation;
	//Timers
	int mCurrent;
};

#endif