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
		void			Hide ( void );

		bool			IsHidden ( void );

	protected:
		//		InitializeTalkerValues()
		// Sets up default talker values without touching or creating any visual elements
		void			InitializeTalkerValues ( void );

	public:
		std::string	text;
		Vector3f	position;
		Real		width;

		bool		scramble_lerp;

	protected:
		CTextMesh*	m_textmesh;
		TalkerBoxBackground*		m_boxbackground;
		TalkerBoxBackgroundColor*	m_boxbackgroundblend;

		float		m_displayTickerTimer;
		int			m_displayLength;
		std::string	m_displayString;

		float		m_scrambleFixTimer;
		float		m_scrambleTimer;
		int			m_scrambleLength;
		bool		m_scrambleUpdate;

		float		m_fadeLerp;
		bool		m_fadeIn;
	};
}

#endif//_M04_TALKER_BOX_H_