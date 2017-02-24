#ifndef _M04_TALKER_BOX_H_
#define _M04_TALKER_BOX_H_

#include "m04/entities/TalkerBase.h"

class CTextMesh;

namespace M04
{
	class TalkerBox : public TalkerBase
	{
	public:
		explicit		TalkerBox ( void );
		~TalkerBox ( void );

		void			Update ( void ) override;

	public:
		std::string	text;
		Vector3d	position;

	protected:
		CTextMesh*	m_textmesh;

		float		m_displayTickerTimer;
		int			m_displayLength;
		std::string	m_displayString;
	};
}

#endif//_M04_TALKER_BOX_H_