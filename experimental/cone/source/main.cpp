#include <stdio.h>
#include "lexer.h"

int main ( void )
{
	cone::lex_debug("tests/00_assignment.c");

	// Pause output:
	getc(stdin);

	return 0;
}