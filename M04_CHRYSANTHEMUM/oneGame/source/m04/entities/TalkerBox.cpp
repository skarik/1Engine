
#include "m04/entities/TalkerBox.h"
#include "render2d/object/CTextMesh.h"

using namespace M04;

TalkerBox::TalkerBox ( void )
	: TalkerBase()
{
	m_textmesh = new CTextMesh();
	m_textmesh->SetFont("ComicNeue-Angular-Bold.ttf", 12, FW_BOLD);
	m_textmesh->m_text = "";
	m_textmesh->UpdateText();

	m_displayTickerTimer = 0;
	m_displayLength = 0;
	m_displayString = "";
}

TalkerBox::~TalkerBox ( void )
{
	delete_safe(m_textmesh);
}

void TalkerBox::Update ( void )
{
	int display_length_previous = m_displayString.length();

	m_displayTickerTimer += 30.0F * Time::deltaTime;
	while ( m_displayTickerTimer > 1.0F )
	{
		m_displayTickerTimer -= 1.0F;
		m_displayLength += 1;
	}

	m_displayString = text.substr( 0, std::min<int>(m_displayLength, (int)text.length()) );

	if ( display_length_previous != m_displayString.length() )
	{
		// Update text
		m_textmesh->m_text = m_displayString;
		m_textmesh->UpdateText();
	}

	// Update text position
	m_textmesh->transform.position = position;
}