#ifndef _M04_TALKER_BOX_H_
#define _M04_TALKER_BOX_H_

#include "m04/entities/TalkerBase.h"

class CTextMesh;

namespace M04
{

	class TalkerBox : public TalkerBase
	{
		class TalkerBoxBackground;
		class TalkerBoxBackgroundColor;

	public:
		explicit		TalkerBox ( void );
		~TalkerBox ( void );

		void			Update ( void ) override;

		void			Show ( void );

	public:
		std::string	text;
		Vector3d	position;
		Real		width;

	protected:
		CTextMesh*	m_textmesh;
		TalkerBoxBackground*	m_boxbackground;

		float		m_displayTickerTimer;
		int			m_displayLength;
		std::string	m_displayString;

		float		m_fadeLerp;

	};
}

#endif//_M04_TALKER_BOX_H_