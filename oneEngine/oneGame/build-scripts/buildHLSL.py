#!python3

import os, subprocess, sys

#=========================================================#
# Options
#=========================================================#

g_hlslCompiler = "fxc-auto"
g_hlslBaseProfile = "5_0"

g_spirvTranspiler = "spirv-cross.exe" # Should come with the Vulkan SDK
g_spirvParameters = "--hlsl --shader-model 50"

#=========================================================#
# File walker
#=========================================================#

#colors ripped from blender
class bcolors:
	HEADER = '\x1b[95m'
	OKBLUE = '\x1b[94m'
	OKGREEN = '\x1b[92m'
	WARNING = '\x1b[93m'
	FAIL = '\x1b[91m'
	ENDC = '\x1b[0m'
	BOLD = '\x1b[1m'
	UNDERLINE = '\x1b[4m'

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
	
	# Check args for if we're going to compile a single file instead
	m_matchingPattern = None
	if (len(sys.argv) > 1):
		m_matchingPattern = sys.argv[1]

	# Grab directory
	l_directoryPath = os.path.dirname(os.path.realpath(__file__))
	m_projectRootPath = os.path.split(l_directoryPath)[0] + "/"

	# Set up display
	m_buildSucceeded = 0
	m_buildFailed = 0
	m_buildUpToDate = 0
	m_buildSkipped = 0

	# Loop through each .res folder to compile shaders.
	for i_filename in os.listdir(m_projectRootPath):
	
		# Only work on res folders:
		if not (".res" in i_filename):
			continue
		if ("backup" in i_filename):
			continue

		# Check if the folder exists
		l_resourceFolder = m_projectRootPath + i_filename + "/shaders"
		print(m_outp + "	Scanning in " + bcolors.UNDERLINE + i_filename + bcolors.ENDC + "...")
		if not os.path.isdir(l_resourceFolder):
			continue

		# Loop through each file
		for subdir, dirs, files in os.walk(l_resourceFolder):
			for file in files:
				if (file.endswith(".glsl") and ((m_matchingPattern is None) or (m_matchingPattern in file))):
					# Get the file path & display it
					l_shaderFilePath = os.path.join(subdir, file)
					l_displayName = l_shaderFilePath[len(m_projectRootPath):]
					print(m_outp + l_displayName)
					
					# Get the name without suffix
					l_transpileVariants = False
					l_nakedFile = os.path.splitext(l_shaderFilePath)[0]
					if (os.path.exists(l_nakedFile + ".variants.h")):
						l_transpileVariants = True
					
					if (not l_transpileVariants):
						# Transpile the shader
						l_status = TranspileShader(l_shaderFilePath, l_displayName)
						if (l_status == 0):
							pass
						else:
							m_buildFailed += 1
							continue
						
						# Compile the shader
						l_status = CompileShader(l_shaderFilePath, l_nakedFile, l_displayName)
						if (l_status == 0):
							m_buildSucceeded += 1
						else:
							m_buildFailed += 1
					else:
						print("Searching for variants...")
						for local_file in os.listdir(subdir):
							
							l_localFilePath = os.path.join(subdir, local_file)
							if not(l_localFilePath.startswith(l_nakedFile) and l_localFilePath.endswith(".spv")):
								continue
							
							l_localNakedFile = os.path.splitext(l_localFilePath)[0]
							l_shaderFilePath = l_localNakedFile + ".dummy"
							print("Found variant " + l_localNakedFile)
							
							# Transpile the variant
							l_status = TranspileShader(l_shaderFilePath, l_displayName)
							if (l_status == 0):
								pass
							else:
								m_buildFailed += 1
								continue
							
							# Compile the shader
							l_status = CompileShader(l_shaderFilePath, l_nakedFile, l_displayName)
							if (l_status == 0):
								m_buildSucceeded += 1
							else:
								m_buildFailed += 1

	print((bcolors.WARNING if m_buildFailed > 0 else bcolors.OKGREEN)
		  + "Shader Build: {:d} succeeded, {:d} failed, {:d} up-to-date, {:d} skipped."
		  .format(m_buildSucceeded, m_buildFailed, m_buildUpToDate, m_buildSkipped)
		  + bcolors.ENDC)
	
#=========================================================#
# Compiler shell command
#=========================================================#

def TranspileShader(shaderFilePath, displayName):
	
	# Get the name without suffix
	l_nakedFile = os.path.splitext(shaderFilePath)[0]
	l_inputFileSpirV	= l_nakedFile + ".spv"
	l_outputFileHLSL	= l_nakedFile + ".hlsl"
	
	# Ensure SPIR-V file exists
	if (not os.path.exists(l_inputFileSpirV)):
		print(bcolors.FAIL + bcolors.ENDC)
		return -1

	# Start up the compiler
	stream = subprocess.Popen(
		' '.join([g_spirvTranspiler,
				  *g_spirvParameters.split(),
				  f"--output \"{l_outputFileHLSL}\"",
				  l_inputFileSpirV]),
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
				if "no code produced" in line:
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
				print(m_outp[0:2] + bcolors.OKBLUE + line + bcolors.ENDC)

		if (output_bytestream[1] != None):
			output_string = output_bytestream[1].decode("utf-8")
			print(output_string)

	return code
	
def CompileShader(shaderFilePath, nakedFilePath, displayName):
	
	# Get the name without suffix
	l_nakedFile = os.path.splitext(shaderFilePath)[0]
	l_inputFileHLSL		= l_nakedFile + ".hlsl"
	l_outputFileObj		= l_nakedFile + ".dxc"
	l_outputFileAsm		= l_nakedFile + ".aasm"

	# Grab the correct shader type by the name
	l_hlslCompilerProfile = ""
	if nakedFilePath.endswith("_vv"):
		l_hlslCompilerProfile = "vs"
	elif nakedFilePath.endswith("_p"):
		l_hlslCompilerProfile = "ps"
	elif nakedFilePath.endswith("_g"):
		l_hlslCompilerProfile = "gs"
	elif nakedFilePath.endswith("_h"):
		l_hlslCompilerProfile = "hs"
	elif nakedFilePath.endswith("_d"):
		l_hlslCompilerProfile = "ds"
	elif nakedFilePath.endswith("_c"):
		l_hlslCompilerProfile = "cs"
	l_hlslCompilerProfile = f"{l_hlslCompilerProfile}_{g_hlslBaseProfile}"

	# Start up the compiler
	stream = subprocess.Popen(
		' '.join([g_hlslCompiler,
				  f"/T {l_hlslCompilerProfile}",
				  f"/Fc \"{l_outputFileAsm}\"",
				  f"/Fo \"{l_outputFileObj}\"",
				  l_inputFileHLSL]),
		stdout=subprocess.PIPE,
		shell=True)

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
				if "no code produced" in line:
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
				print(m_outp[0:2] + bcolors.OKGREEN + line + bcolors.ENDC)

		if (output_bytestream[1] != None):
			output_string = output_bytestream[1].decode("utf-8")
			print(output_string)

	return code

#=========================================================#
# Run program
#=========================================================#

main()