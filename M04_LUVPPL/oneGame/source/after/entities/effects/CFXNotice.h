
#ifndef _C_FX_NOTICE_H_
#define _C_FX_NOTICE_H_

#include "engine/behavior/CGameBehavior.h"
//#include "CBillboard.h"
class CBillboard;

class CFXNotice : public CGameBehavior
{
public:
	explicit		CFXNotice ( const int ntype, const Vector3d& nposition );
					~CFXNotice ( void );
	void			Update ( void );

private:
	CBillboard*	m_effect;

	Vector3d	position;
	ftype		timer;
	ftype		size;
};

#endif//_C_FX_NOTICE_H_