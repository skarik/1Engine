#!python3

import os, subprocess

#=========================================================#
# Options
#=========================================================#

g_glslCompiler = "glslangValidator"
g_glslParameters = "-V100"

#=========================================================#
# File walker
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

def enableConsoleColors():
	import ctypes
	# Pulling from kernel32 to enable VT100 processing in the console we're working in.
	l_kernel32 = ctypes.WinDLL('kernel32')
	l_hStdOut = l_kernel32.GetStdHandle(-11)
	
	mode = ctypes.c_ulong()
	l_kernel32.GetConsoleMode(l_hStdOut, ctypes.byref(mode))
	mode.value |= 0x004 # ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x004
	l_kernel32.SetConsoleMode(l_hStdOut, mode)
enableConsoleColors()

m_outp = "0>  "

def main():

	# Grab directory
	l_directoryPath = os.path.dirname(os.path.realpath(__file__))
	m_projectRootPath = os.path.split(l_directoryPath)[0] + "/"

	# Set up display
	m_buildSucceeded = 0
	m_buildFailed = 0
	m_buildUpToDate = 0
	m_buildSkipped = 0
	
	# Loop through each .res folder to generate include directors.
	l_includeFolders = []
	for i_filename in os.listdir(m_projectRootPath):
	
		# Only work on res folders:
		if not (".res" in i_filename):
			continue
		if ("backup" in i_filename):
			continue
	
		# Check if the shaders folder exists
		l_resourceFolder = m_projectRootPath + i_filename + "/shaders"
		if not os.path.isdir(l_resourceFolder):
			continue
		   
		# Save the current shader folder
		l_includeFolders.append(l_resourceFolder);

	# Loop through each .res folder to compile shaders.
	for i_filename in os.listdir(m_projectRootPath):
	
		# Only work on res folders:
		if not (".res" in i_filename):
			continue
		if ("backup" in i_filename):
			continue

		# Check if the folder exists
		l_resourceFolder = m_projectRootPath + i_filename + "/shaders"
		print(m_outp + "	Scanning in " + bcolors.UNDERLINE + i_filename + bcolors.ENDC + "...");
		if not os.path.isdir(l_resourceFolder):
			continue

		# Loop through each file
		for subdir, dirs, files in os.walk(l_resourceFolder):
			for file in files:
				if (file.endswith(".glsl")):
					# Get the file path & display it
					l_shaderFilePath = os.path.join(subdir, file)
					l_displayName = l_shaderFilePath[len(m_projectRootPath):]
					print(m_outp + l_displayName)

					# Compile the shader
					l_status = CompileShader(l_shaderFilePath, l_displayName, l_includeFolders)
					if (l_status == 1):
						m_buildSkipped += 1
					elif (l_status == 0):
						m_buildSucceeded += 1
					elif (l_status == 2):
						m_buildFailed += 1

	print((bcolors.WARNING if m_buildFailed > 0 else bcolors.OKGREEN)
		  + "Shader Build: {:d} succeeded, {:d} failed, {:d} up-to-date, {:d} skipped."
		  .format(m_buildSucceeded, m_buildFailed, m_buildUpToDate, m_buildSkipped)
		  + bcolors.ENDC)

#=========================================================#
# Compiler shell command
#=========================================================#

def CompileShader(shaderFilePath, displayName, includeFolders):
	
	# Get the name without suffix
	l_nakedFile = os.path.splitext(shaderFilePath)[0]
	l_outputFile = l_nakedFile + ".spv"

	# Grab the correct shader type by the name
	l_glslCompilerMode = "comp"
	if l_nakedFile.endswith("_vv"):
		l_glslCompilerMode = "vert"
	elif l_nakedFile.endswith("_p"):
		l_glslCompilerMode = "frag"
	elif l_nakedFile.endswith("_g"):
		l_glslCompilerMode = "geom"
	elif l_nakedFile.endswith("_h"):
		l_glslCompilerMode = "tesc"
	elif l_nakedFile.endswith("_d"):
		l_glslCompilerMode = "tese"

	# Start up the compiler
	stream = subprocess.Popen(
		' '.join([g_glslCompiler,
				  g_glslParameters,
				  "-S " + l_glslCompilerMode,
				  "-o \"" + l_outputFile + "\"",
				  *["-I\"" + include + "\"" for include in includeFolders],
				  shaderFilePath]),
		stdout=subprocess.PIPE)
	# Grab the output and return code
	output_bytestream = stream.communicate()
	code = stream.returncode

	# If there was an error on the compile...
	if True:
		# Parse the stdout bytestream
		if (output_bytestream[0] != None):
			output_string = output_bytestream[0].decode("utf-8")

			# Split across newlines
			output_lines = output_string.splitlines()

			# Stop displaying lines after the line with "No code generated"
			l_issueLine = len(output_lines) - 1
			for index, line in enumerate(output_lines):
				if "No code generated." in line:
					l_issueLine = index
					break
			del output_lines[l_issueLine:]

			# Remove useless first line if it matches the input file
			if len(output_lines) > 0:
				if shaderFilePath in output_lines[0]:
					del output_lines[0]

			# Replace the long filename with the nice display name
			#for index, line in enumerate(output_lines):
			#	 output_lines[index] = line.replace(shaderFilePath, displayName)

			# Display the output
			for line in output_lines:
				print(m_outp[0:2] + bcolors.FAIL + line + bcolors.ENDC)

		if (output_bytestream[1] != None):
			output_string = output_bytestream[1].decode("utf-8")
			print(output_string)

	return code

#=========================================================#
# Run program
#=========================================================#

main()