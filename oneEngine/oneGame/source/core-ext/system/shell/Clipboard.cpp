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
		if (Format == CF_UNICODETEXT)
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
	HANDLE textGlobalAllocation = GetClipboardData(CF_UNICODETEXT);

	// Lock it for reading
	wchar_t* textGlobalBuffer = (wchar_t*)GlobalLock(textGlobalAllocation);
	if (textGlobalBuffer != NULL)
	{
		// Pull the UTF16 string
		int textGlobalBufferLen = lstrlenW(textGlobalBuffer);

		// Convert UTF16 to UTF8
		int utf8BufferSize = WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, textGlobalBuffer, textGlobalBufferLen, NULL, 0, NULL, NULL);
		char* utf8Buffer = new char [utf8BufferSize];
		WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, textGlobalBuffer, textGlobalBufferLen, utf8Buffer, utf8BufferSize, NULL, NULL);

		// We have the result!
		result = utf8Buffer;

		// Clear the used buffer
		delete[] utf8Buffer;

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
	int utf16BufferSize = MultiByteToWideChar(CP_UTF8, WC_COMPOSITECHECK, str, str_len, NULL, 0);

	// Create global allocation
	HANDLE textGlobalAllocation = GlobalAlloc(GMEM_MOVEABLE, utf16BufferSize);

	// Lock it for writing
	wchar_t* textGlobalBuffer = (wchar_t*)GlobalLock(textGlobalAllocation);
	if (textGlobalBuffer != NULL)
	{
		MultiByteToWideChar(CP_UTF8, WC_COMPOSITECHECK, str, str_len, textGlobalBuffer, utf16BufferSize);
		GlobalUnlock(textGlobalAllocation);
	}

	// Save it to the clipboard now
	SetClipboardData(CF_TEXT, textGlobalAllocation);

	CloseClipboard();
}

#endif