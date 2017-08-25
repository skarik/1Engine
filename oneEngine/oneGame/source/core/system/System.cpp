
#include "System.h"
#include "core/exceptions/exceptions.h"

// System specific includes
#include "core/os.h"


using namespace std;

void System::GetWorkingDirectory ( char* nOutDirectory, int nStrlen )
{
	GetCurrentDirectory( nStrlen, nOutDirectory );
}


bool System::GetOpenFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir, const char* nDialogueTitle  )
{
#ifdef _WIN32
/*	BOOL result;
	CHAR	workingDir [1024];
	GetCurrentDirectory( 1024, workingDir );
	while ( ShowCursor( true ) < 0 );
	SetCursor( LoadCursor( GetModuleHandle( NULL ), IDC_ARROW ) );

	CHAR	pstrFile [1024];
	CHAR	pFiletypes[1024];
	memset( pFiletypes, 0, 256 );
	sprintf( pFiletypes, "%s\0%s\0\0", nFiletypes[0].filetype, nFiletypes[0].extension );
	OPENFILENAME	ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = RrWindow::pActive->getWindowHandle();
	ofn.lpstrFilter	= pFiletypes;//"Particle system files\0*.PCF\0\0";
	ofn.lpstrFile	= pstrFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile	= 1024;
	ofn.lpstrInitialDir	= nInitialDir;
	ofn.lpstrTitle = nDialogueTitle;
	ofn.Flags = OFN_PATHMUSTEXIST;

	result = GetOpenFileName( &ofn );
	if ( !result ) {
		cout << "CommDlgExtendedError():" << CommDlgExtendedError() << endl;
	}

	SetCurrentDirectory( workingDir );
	//ShowCursor( false );

	strcpy( nOutFilename, pstrFile );

	return result!=0;
	*/
#endif
	throw core::NotYetImplementedException();
	return false;
}
bool System::GetSaveFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir, const char* nDialogueTitle )
{
#ifdef _WIN32
	/*BOOL result;
	CHAR	workingDir [1024];
	GetCurrentDirectory( 1024, workingDir );
	while ( ShowCursor( true ) < 0 );
	SetCursor( LoadCursor( GetModuleHandle( NULL ), IDC_ARROW ) );

	CHAR	pstrFile [1024];
	CHAR	pFiletypes[1024];
	memset( pFiletypes, 0, 256 );
	sprintf( pFiletypes, "%s\0%s\0\0", nFiletypes[0].filetype, nFiletypes[0].extension );
	OPENFILENAME	ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = RrWindow::pActive->getWindowHandle();
	ofn.lpstrFilter	= pFiletypes;//"Particle system files\0*.PCF\0\0";
	ofn.lpstrFile	= pstrFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile	= 1024;
	ofn.lpstrInitialDir	= nInitialDir;
	ofn.lpstrTitle = nDialogueTitle;
	ofn.Flags = OFN_PATHMUSTEXIST;

	result = GetSaveFileName( &ofn );
	if ( !result ) {
		cout << "CommDlgExtendedError():" << CommDlgExtendedError() << endl;
	}

	SetCurrentDirectory( workingDir );
	//ShowCursor( false );

	strcpy( nOutFilename, pstrFile );

	return result!=0;
	*/
#endif
	throw core::NotYetImplementedException();
	return false;
}