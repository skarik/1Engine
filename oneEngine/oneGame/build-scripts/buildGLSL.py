#!python3

import os, subprocess
import types

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

# Output prefix.
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
	
	# Grab include foldrs
	m_includeFolders = CollectIncludeFolders(m_projectRootPath);

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
					
					# Need extensionless filename
					l_nakedFile = os.path.splitext(l_shaderFilePath)[0]
					
					# Create default ouput
					l_outputFilePath = l_nakedFile + ".spv"
					
					# Check for variants
					l_variantsFile = l_nakedFile + ".variants.h";
					l_variants = []
					if (os.path.exists(l_variantsFile)):
						l_variants = ReadVariants(l_variantsFile);

					# Compile the shader
					if (not l_variants):
						l_status = CompileShader(l_shaderFilePath, l_outputFilePath, m_includeFolders, [])
						if (l_status == 1):
							m_buildSkipped += 1
						elif (l_status == 0):
							m_buildSucceeded += 1
						elif (l_status == 2):
							m_buildFailed += 1
					else:
						# Cache all permuations of macros
						l_macroVariants = []
						for variant in l_variants:
							l_newMacroVariants = []
							# For each variant value...
							for variant_value in variant.values:
								# Create macro that needs to be added to every macro list
								macro = types.SimpleNamespace(name=variant.name, value=str(variant_value));
								if (l_macroVariants):
									# Add macro to every existing macro list
									for old_macro_list in l_macroVariants:
										new_macro_list = old_macro_list.copy()
										new_macro_list.append(macro)
										l_newMacroVariants.append(new_macro_list)
								else:
									#If macro list is empty, then we just start with size-1 lists
									l_newMacroVariants.append([macro])
							# Save the current list
							l_macroVariants = l_newMacroVariants
						
						l_macroVariantIndex = 0;
						for l_macroVariant in l_macroVariants:
							
							l_outputFilePath = l_nakedFile + "_" + str(l_macroVariantIndex) + ".spv";
							print(m_outp + "\tOutput: " + l_outputFilePath)
							
							l_status = CompileShader(l_shaderFilePath, l_outputFilePath, m_includeFolders, l_macroVariant)
							if (l_status == 1):
								m_buildSkipped += 1
							elif (l_status == 0):
								m_buildSucceeded += 1
							elif (l_status == 2):
								m_buildFailed += 1
								
							l_macroVariantIndex += 1

	print((bcolors.WARNING if m_buildFailed > 0 else bcolors.OKGREEN)
		  + "Shader Build: {:d} succeeded, {:d} failed, {:d} up-to-date, {:d} skipped."
		  .format(m_buildSucceeded, m_buildFailed, m_buildUpToDate, m_buildSkipped)
		  + bcolors.ENDC)
	return (0 if m_buildSucceeded else 1) # Done.

def CollectIncludeFolders(m_projectRootPath):
	# Loop through each .res folder to generate include directories.
	includeFolders = []
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
		includeFolders.append(l_resourceFolder);
	# Return the folders list we now have
	return includeFolders;
	
def ReadVariants(variantsFilePath):
	l_variantsFileHandle = open(variantsFilePath, "r");
	l_variantsLines = l_variantsFileHandle.readlines();
	
	l_valid = False;
	l_variants = [];
	
	# Read in the file line-by-line
	for l_variantLine in l_variantsLines:
		if (not l_valid):
			if ("SHADER_VARIANT_BEGIN" in l_variantLine):
				l_valid = True;
		else:
			# Parse in the variants
			if ("//" in l_variantLine):
				# Skip lines with comments. TODO: This needs to be better.
				continue;
			elif ("SHADER_BOOL" in l_variantLine):
				l_booleanName = l_variantLine[l_variantLine.find('(') + 1 : l_variantLine.find(')')].replace(';', '')
				
				# Create a new boolean variant
				l_variant = types.SimpleNamespace(name=l_booleanName, values=[0, 1]);
				l_variants.append(l_variant);
				
				# Print this new variant's info
				print("Boolean variant definition: \"" + l_variant.name + "\" = " + ','.join(map(str, l_variant.values)));
				
	return l_variants;

#=========================================================#
# Compiler shell command
#=========================================================#

def CompileShader(shaderFilePath, outputFilePath, includeFolders, macros):
	
	# Get the name without suffix
	l_nakedFile = os.path.splitext(shaderFilePath)[0]

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
				  "-o \"" + outputFilePath + "\"",
				  *["-I\"" + include + "\"" for include in includeFolders],
				  *["-D" + macro.name + "=" + macro.value for macro in macros],
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