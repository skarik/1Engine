#include "Clipboard.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

bool core::shell::clipboard::ContainsString ( void )
{
	OpenClipboard(NULL);

	UINT Format = 0;
	do
	{
		Format = EnumClipboardFormats(Format);
		if (Format == CF_TEXT)
		{
			// Found a compatible format, break out
			break;
		}
	}
	while (Format != 0);

	CloseClipboard();

	// Format isn't zero - therefore there is text that exists in the clipboard.
	return Format != 0;
}

std::string core::shell::clipboard::GetString ( void )
{
	std::string result;
	
	OpenClipboard(NULL);

	// Pull the handle to the global allocation
	HANDLE textGlobalAllocation = GetClipboardData(CF_TEXT);

	// Lock it for reading
	char* textGlobalBuffer = (char*)GlobalLock(textGlobalAllocation);
	if (textGlobalBuffer != NULL)
	{
		result = textGlobalBuffer;
		GlobalUnlock(textGlobalAllocation);
	}

	CloseClipboard();

	return result;
}

void core::shell::clipboard::SetString ( const char* str )
{
	OpenClipboard(NULL);
	EmptyClipboard();

	const size_t str_len = strlen(str);

	// Create global allocation
	HANDLE textGlobalAllocation = GlobalAlloc(GMEM_MOVEABLE, str_len);

	// Lock it for writing
	char* textGlobalBuffer = (char*)GlobalLock(textGlobalAllocation);
	if (textGlobalBuffer != NULL)
	{
		memcpy(textGlobalBuffer, str, str_len);
		GlobalUnlock(textGlobalAllocation);
	}

	// Save it to the clipboard now
	SetClipboardData(CF_TEXT, textGlobalAllocation);

	CloseClipboard();
}

#endif