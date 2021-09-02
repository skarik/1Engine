#!python3

# This script searches the PC and then sets the WIN_SDK_VER system environment variable to the correct value.
# If none can be found, it falls back to 8.1

import os, sys;

print( "WIN_SDK_VER is currently set to \"%s\"" % (os.environ.get("WIN_SDK_VER")) );

def SetEnvironmentVariable ( variableName, variableValue ):
	import winreg, win32gui, win32con;

	try:
		path = r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'
		reg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE);
		key = winreg.OpenKey(reg, path, 0, winreg.KEY_ALL_ACCESS);
		
		winreg.SetValueEx( key, variableName, 0, winreg.REG_EXPAND_SZ, variableValue );
		
		winreg.CloseKey(key)
		winreg.CloseKey(reg)
		
		win32gui.SendMessageTimeout(win32con.HWND_BROADCAST, win32con.WM_SETTINGCHANGE, 0, u'Environment', win32con.SMTO_ABORTIFHUNG, 500);
		
	except Exception as e:
		print( e )
		
		
def FindWindowsSDKVersion ():

	# Start in program files and go to the kits directory
	l_kitsBaseDirectory = os.environ["ProgramFiles"] + "\\Windows Kits";
	if (not os.path.isdir(l_kitsBaseDirectory)):
		l_kitsBaseDirectory = os.environ["ProgramFiles(x86)"] + "\\Windows Kits";
	
	# We support two kits: 8.1 and 10. We want to go with the newest one we can find, so we check 10 first.
	if (os.path.isdir(l_kitsBaseDirectory + "\\10")):
		# Check all the kit versions in the bin subdirectory
		for subdir in reversed(sorted(os.listdir(l_kitsBaseDirectory + "\\10\\Include"))):
			# Only check versioned folders (the x86 folder is not linked to anything here).
			if not "10." in subdir:
				continue
				
			# Look for stdlib
			l_stdlibPath = l_kitsBaseDirectory + "\\10\\Include\\" + subdir + "\\ucrt\\stdlib.h"
			if (os.path.exists(l_stdlibPath)):
				return subdir
					
	# We still here, then we check the 8.1 path (since there's only one path)
	l_windowsPath = l_kitsBaseDirectory + "\\8.1\\Include\\um\\Windows.h"
	if (os.path.exists(l_windowsPath)):
		return "8.1"

	return None

def main ():
	l_foundSDKVersion = FindWindowsSDKVersion();
	if (l_foundSDKVersion == None):
		print( "Could not find a Windows SDK!" );
		sys.exit(-1)
	else:
		print( "Setting WIN_SDK_VER to \"%s\"" % (l_foundSDKVersion) );
		SetEnvironmentVariable("WIN_SDK_VER", l_foundSDKVersion);
		print( "Done." );
		
main()