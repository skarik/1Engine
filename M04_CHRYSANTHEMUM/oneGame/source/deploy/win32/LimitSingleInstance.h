
#ifndef LimitSingleInstance_H
#define LimitSingleInstance_H
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h> 

//This code is from Q243953 in case you lose the article and wonder
//where this code came from.
class CLimitSingleInstance
{
protected:
	DWORD  m_dwLastError;
	HANDLE m_hMutex;

public:
	CLimitSingleInstance(TCHAR *strMutexName)
	{
		//Make sure that you use a name that is unique for this application otherwise
		//two apps may think they are the same if they are using same name for
		//3rd parm to CreateMutex
		m_hMutex = CreateMutex(NULL, FALSE, strMutexName); //do early
		m_dwLastError = GetLastError(); //save for use later...
	}
   
	~CLimitSingleInstance() 
	{
		if (m_hMutex)  //Do not forget to close handles.
		{
			CloseHandle(m_hMutex); //Do as late as possible.
			m_hMutex = NULL; //Good habit to be in.
		}
	}

	BOOL IsAnotherInstanceRunning() 
	{
		return (ERROR_ALREADY_EXISTS == m_dwLastError);
	}
};

#endif//_WIN32
#endif//LimitSingleInstance_H