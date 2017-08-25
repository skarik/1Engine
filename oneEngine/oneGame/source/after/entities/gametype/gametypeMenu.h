
#ifndef _GAMETYPE_MENU_H_
#define _GAMETYPE_MENU_H_

#include "CGameType.h"

class gametypeMenu : public CGameType
{
public:
	gametypeMenu ( void );
	gametypeMenu ( CGameType* );
	~gametypeMenu ( void );

	void Update ( void );

	void InitWorld ( void );
	void StopWorld ( void );

};

#endif//_GAMETYPE_MENU_H_