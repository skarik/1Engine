#include "Spacer.h"

void dusk::elements::Spacer::PostCreate ( void )
{
	if (m_parent != NULL)
	{
		m_parentReferenceSize = m_parent->m_localRect.size;
	}
}

void dusk::elements::Spacer::Update ( const UIStepInfo* stepinfo )
{
}