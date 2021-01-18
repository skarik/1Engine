#include "CInput.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

unsigned char core::VkToAsciiTable [core::kVkCount] = {};

void core::InitializeVkToAsciiTable ( void )
{
	// Clear entire table to invalids
	for ( int i = 0; i < core::kVkCount; ++i ) {
		VkToAsciiTable[i] = 0;
	}
	for ( int a = '0'; a <= '9'; ++a ) {
		VkToAsciiTable[a] = a;
	}
	for ( int a = 'A'; a <= 'Z'; ++a ) {
		VkToAsciiTable[a] = a;
	}

	// The following are the actual ASCII mappings
	VkToAsciiTable[VK_BACK]			= 0x08;
	VkToAsciiTable[VK_TAB]			= '\t';
	VkToAsciiTable[VK_RETURN]		= '\r';
	VkToAsciiTable[VK_ESCAPE]		= 0x1B;
	VkToAsciiTable[VK_SPACE]		= ' ';

	VkToAsciiTable[VK_DELETE]		= 0x7F;
	VkToAsciiTable[VK_NUMPAD0]		= '0';
	VkToAsciiTable[VK_NUMPAD1]		= '1';
	VkToAsciiTable[VK_NUMPAD2]		= '2';
	VkToAsciiTable[VK_NUMPAD3]		= '3';
	VkToAsciiTable[VK_NUMPAD4]		= '4';
	VkToAsciiTable[VK_NUMPAD5]		= '5';
	VkToAsciiTable[VK_NUMPAD6]		= '6';
	VkToAsciiTable[VK_NUMPAD7]		= '7';
	VkToAsciiTable[VK_NUMPAD8]		= '8';
	VkToAsciiTable[VK_NUMPAD9]		= '9';
	VkToAsciiTable[VK_MULTIPLY]		= '*';
	VkToAsciiTable[VK_ADD]			= '+';
	VkToAsciiTable[VK_SEPARATOR]	= '*';
	VkToAsciiTable[VK_SUBTRACT]		= '-';
	VkToAsciiTable[VK_DECIMAL]		= '.';
	VkToAsciiTable[VK_DIVIDE]		= '/';

	VkToAsciiTable[VK_OEM_NEC_EQUAL]= '=';
	VkToAsciiTable[VK_OEM_1]		= ';';
	VkToAsciiTable[VK_OEM_2]		= '/';
	VkToAsciiTable[VK_OEM_3]		= '`';
	VkToAsciiTable[VK_OEM_PLUS]		= '+';
	VkToAsciiTable[VK_OEM_COMMA]	= ',';
	VkToAsciiTable[VK_OEM_MINUS]	= '-';
	VkToAsciiTable[VK_OEM_PERIOD]	= '.';
	VkToAsciiTable[VK_OEM_4]		= '[';
	VkToAsciiTable[VK_OEM_5]		= '\\';
	VkToAsciiTable[VK_OEM_6]		= ']';
	VkToAsciiTable[VK_OEM_7]		= '\'';
}

#endif