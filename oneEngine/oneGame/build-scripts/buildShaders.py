#!python3

import os, subprocess, types, sys

#=========================================================#
# Options
#=========================================================#

g_glslScript = "buildGLSL.py"
g_hlslScript = "buildHLSL.py"

#=========================================================#
# Runner
#=========================================================#

def main():
	RunScript(g_glslScript, sys.argv[1:]);
	RunScript(g_hlslScript, sys.argv[1:]);

#=========================================================#
# Compiler forwarding
#=========================================================#

def RunScript ( script, arguments ):
	l_callArray = 	[script,
					*arguments]
	
	subprocess.call(l_callArray, shell=True)

#=========================================================#
# Run program
#=========================================================#


main()