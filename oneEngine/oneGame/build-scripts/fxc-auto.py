#!python3

import sys, os, subprocess

#=========================================================#
# Options
#=========================================================#

#g_hlslCompiler = "dxc.exe"
g_hlslCompiler = "fxc.exe"

#=========================================================#
# Invoke compiler
#=========================================================#

#colors ripped from blender
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
	
def main():

	# Look for the compiler
	print(bcolors.OKBLUE + "Looking for \"fxc.exe\"."  + bcolors.ENDC)
	l_hlslCompiler = GetFxc();
	if l_hlslCompiler is None:
		print(bcolors.FAIL + "Could not find \"fxc.exe\". Please install a Windows SDK with DirectX support." + bcolors.ENDC)
		return -1;
	print(bcolors.OKGREEN + f"Found \"{l_hlslCompiler}\"" + bcolors.ENDC)
	
	# Pass all script arguments to the compiler
	return subprocess.call([l_hlslCompiler, *sys.argv[1:]])
	
#=========================================================#
# Find fxc.exe
#=========================================================#

def GetFxc():

	# Start in program files and go to the kits directory
	l_kitsBaseDirectory = os.environ["ProgramFiles(x86)"] + "\\Windows Kits";
	
	# We support two kits: 8.1 and 10. We want to go with the newest one we can find, so we check 10 first.
	if (os.path.isdir(l_kitsBaseDirectory + "\\10")):
		# Check all the kit versions in the bin subdirectory
		for subdir in reversed(sorted(os.listdir(l_kitsBaseDirectory + "\\10\\bin"))):
			# Only check versioned folders (the x86 folder is not linked to anything here).
			if not "10." in subdir:
				continue
				
			# Look for a compiler
			l_fxcPath = l_kitsBaseDirectory + "\\10\\bin\\" + subdir + "\\x86\\" + g_hlslCompiler
			if (os.path.exists(l_fxcPath)):
				return l_fxcPath
				
	# We still here, then we check the 8.1 path (since there's only one path)
	l_fxcPath = l_kitsBaseDirectory + "\\8.1\\bin\\x86\\" + g_hlslCompiler
	if (os.path.exists(l_fxcPath)):
		return l_fxcPath
	
	# Didn't find anything :(
	return None;

#=========================================================#
# Run program
#=========================================================#

main()