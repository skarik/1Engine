#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "wgl_core_3_3.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>

static void* AppleGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}
	
#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
	#else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

int wgl_ext_EXT_swap_control = wgl_LOAD_FAILED;
int wgl_ext_ARB_create_context = wgl_LOAD_FAILED;
int wgl_ext_ARB_create_context_profile = wgl_LOAD_FAILED;

int (CODEGEN_FUNCPTR *_ptrc_wglGetSwapIntervalEXT)() = NULL;
BOOL (CODEGEN_FUNCPTR *_ptrc_wglSwapIntervalEXT)(int) = NULL;

static int Load_EXT_swap_control()
{
	int numFailed = 0;
	_ptrc_wglGetSwapIntervalEXT = (int (CODEGEN_FUNCPTR *)())IntGetProcAddress("wglGetSwapIntervalEXT");
	if(!_ptrc_wglGetSwapIntervalEXT) numFailed++;
	_ptrc_wglSwapIntervalEXT = (BOOL (CODEGEN_FUNCPTR *)(int))IntGetProcAddress("wglSwapIntervalEXT");
	if(!_ptrc_wglSwapIntervalEXT) numFailed++;
	return numFailed;
}

HGLRC (CODEGEN_FUNCPTR *_ptrc_wglCreateContextAttribsARB)(HDC, HGLRC, const int *) = NULL;

static int Load_ARB_create_context()
{
	int numFailed = 0;
	_ptrc_wglCreateContextAttribsARB = (HGLRC (CODEGEN_FUNCPTR *)(HDC, HGLRC, const int *))IntGetProcAddress("wglCreateContextAttribsARB");
	if(!_ptrc_wglCreateContextAttribsARB) numFailed++;
	return numFailed;
}


static const char * (CODEGEN_FUNCPTR *_ptrc_wglGetExtensionsStringARB)(HDC) = NULL;

typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct wgl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} wgl_StrToExtMap;

static wgl_StrToExtMap ExtensionMap[3] = {
	{"WGL_EXT_swap_control", &wgl_ext_EXT_swap_control, Load_EXT_swap_control},
	{"WGL_ARB_create_context", &wgl_ext_ARB_create_context, Load_ARB_create_context},
	{"WGL_ARB_create_context_profile", &wgl_ext_ARB_create_context_profile, NULL},
};

static int g_extensionMapSize = 3;

static wgl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	wgl_StrToExtMap *currLoc = ExtensionMap;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
	wgl_ext_EXT_swap_control = wgl_LOAD_FAILED;
	wgl_ext_ARB_create_context = wgl_LOAD_FAILED;
	wgl_ext_ARB_create_context_profile = wgl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
	wgl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtString(const char *strExtList)
{
	size_t iExtListLen = strlen(strExtList);
	const char *strExtListEnd = strExtList + iExtListLen;
	const char *strCurrPos = strExtList;
	char strWorkBuff[256];

	while(*strCurrPos)
	{
		/*Get the extension at our position.*/
		int iStrLen = 0;
		const char *strEndStr = strchr(strCurrPos, ' ');
		int iStop = 0;
		if(strEndStr == NULL)
		{
			strEndStr = strExtListEnd;
			iStop = 1;
		}

		iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

		if(iStrLen > 255)
			return;

		strncpy(strWorkBuff, strCurrPos, iStrLen);
		strWorkBuff[iStrLen] = '\0';

		LoadExtByName(strWorkBuff);

		strCurrPos = strEndStr + 1;
		if(iStop) break;
	}
}

int wgl_LoadFunctions(HDC hdc)
{
	ClearExtensionVars();
	
	_ptrc_wglGetExtensionsStringARB = (const char * (CODEGEN_FUNCPTR *)(HDC))IntGetProcAddress("wglGetExtensionsStringARB");
	if(!_ptrc_wglGetExtensionsStringARB) return wgl_LOAD_FAILED;
	
	ProcExtsFromExtString((const char *)_ptrc_wglGetExtensionsStringARB(hdc));
	return wgl_LOAD_SUCCEEDED;
}

