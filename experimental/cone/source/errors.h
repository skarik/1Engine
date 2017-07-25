#ifndef C_ONE_ERRORS_H_
#define C_ONE_ERRORS_H_

namespace cone
{
	enum coneErrorCodes
	{
		ERROR_NO_ERROR = 0,

		ERROR_MISSING_FILE,
		ERROR_OUT_OF_MEMORY,
		ERROR_LEXXER_ERROR,
	};

	int errorReport(int code, const char *format, ...);
}

#endif//C_ONE_ERRORS_H_