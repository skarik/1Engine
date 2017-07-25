#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "errors.h"

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

int cone::lex_debug(const char* n_filename)
{
	// Load in the entire file
	FILE* file = fopen(n_filename, "rb");
	if (file == NULL)
		return cone::errorReport(ERROR_MISSING_FILE, "File \"%s\" could not be found.\n", n_filename);

	// get file size
	fseek(file, 0, SEEK_END);
	uint64_t l_charcount = (uint64_t)ftell(file);

	// Allocate memory for the file
	char* l_codeBuffer = new char[l_charcount];
	if (l_codeBuffer == NULL)
		return cone::errorReport(ERROR_OUT_OF_MEMORY, "Could not allocate %d bytes for the input file.\n", l_charcount);
	// Read in the entire file
	fseek(file, 0, SEEK_SET);
	fread(l_codeBuffer, 1, l_charcount, file);

	// Done with the file now
	fclose(file);

	const int kTempStorageSize = 1024;
	char* l_tempStorage = new char[kTempStorageSize];

	// Begin w/ lexer
	stb_lexer lexer;
	stb_c_lexer_init(&lexer, l_codeBuffer, l_codeBuffer + l_charcount, l_tempStorage, kTempStorageSize);
	printf("Starting to parse file...\n");
	// Loop and start parsing
	int token_return = 0;
	do
	{
		token_return = stb_c_lexer_get_token(&lexer);

		if (token_return == 0)
		{
			break;
		}
		switch (lexer.token)
		{
			// Check for error token
			case CLEX_parse_error:
				{
					stb_lex_location location;
					stb_c_lexer_get_location(&lexer, lexer.parse_point, &location);
					delete[] l_codeBuffer;
					delete[] l_tempStorage;
					return cone::errorReport(ERROR_LEXXER_ERROR, "Lexxer error at line %d, position %d.\n", location.line_number, location.line_offset);
				}
				break;

			case CLEX_id:
				{
					printf("Read indentifier: \"%s\"\n", lexer.string);
				}
				break;
			case CLEX_intlit:
			case CLEX_charlit:
				{
					printf("Read integer literal: %d\n", lexer.int_number);
				}
				break;
			case CLEX_floatlit:
				{
					printf("Read float literal: %lf\n", lexer.real_number);
				}
				break;
			case CLEX_dqstring:
				{
					printf("Read string literal: \"%s\"\n", lexer.string);
				}
				break;


			case CLEX_newline:
				{
					printf(" + Newline.\n");
				}
				break;
			case CLEX_semicolon:
				{
					printf(" + Semicolon.\n");
				}
				break;
			case CLEX_period:
				{
					printf(" + Period.\n");
				}
				break;
			case CLEX_comma:
				{
					printf(" + Comma.\n");
				}
				break;
		}
	}
	while (token_return != 0);

	// Done with the code now
	delete[] l_codeBuffer;
	delete[] l_tempStorage;


	return 0;
}
