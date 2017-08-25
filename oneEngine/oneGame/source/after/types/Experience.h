
#ifndef _EXPERIENCE_H_
#define _EXPERIENCE_H_

#include "Disciplines.h"

struct Experience
{
	Discipline		discipline;
	SubDisciplines	subdiscipline;
	int				amount;

	Experience ( void )
		: amount(0), discipline(DscNone), subdiscipline(SubDscNone)
	{
		;
	}
	Experience ( int n_amount )
		: amount(n_amount), discipline(DscNone), subdiscipline(SubDscNone)
	{
		;
	}
	Experience ( int n_amount, Discipline n_discipline, SubDisciplines n_subdiscipline=SubDscNone )
		: amount(n_amount), discipline(n_discipline), subdiscipline(n_subdiscipline)
	{
		;
	}
};


#endif//_EXPERIENCE_H_