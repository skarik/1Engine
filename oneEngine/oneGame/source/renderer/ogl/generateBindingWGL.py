#!python3
import textwrap

m_filenames = {
    "core": "wgl.h",
    "cimpl": "wgl_impl.c",
    "himpl": "wgl_impl.h"
    };

def glgParseDefinitions(n_headerFile, n_listing):
    l_functionCount = 0;
    l_skip = False
    
    ## Loop through the entire header:
    for l_line in n_headerFile:
        # Create a string from the line
        l_lineStr = l_line.decode("utf-8");

        # Skip everything from WGL_VERSION_1_0 since those are in WinGDI.
        if (("WGL_VERSION_1_0" in l_lineStr) and ("#define" not in l_lineStr)):
            l_skip = not l_skip;

        # Are we skipping?
        if (l_skip):
            continue;
        
        # Decode the string
        if (("typedef" not in l_lineStr) and ("WINAPI" in l_lineStr)):
            l_functionCount += 1;

            # Parse out the name of the function:
            l_fnName = None;
            l_breakOnNext = False;
            l_splitString = l_lineStr.split();
            for l_token in l_splitString:
                if (l_token.startswith("WINAPI")):
                    l_breakOnNext = True;
                elif (l_breakOnNext):
                    l_fnName = l_token;
                    break;

            # Save the function
            if (l_fnName != None):
                n_listing.append(l_fnName);
            
        else:
            continue;

    print("There were " + str(l_functionCount) + " functions found.");
    return True;

def glgGenerateHeader(n_filenames, n_header, n_listing):
    # Write the topper
    n_header.write("//===============================================================================================//\n");
    n_header.write("//\n");
    n_header.write("//      1Engine autogenerated header.\n");
    n_header.write("//\n");
    n_header.write("// Please be gentle. Generated from " + n_filenames["core"] + "\n");
    n_header.write("//\n");
    n_header.write("//===============================================================================================//\n");
    # Generate guard ID for the header
    l_guardId = n_filenames["himpl"].upper().replace(".", "_").replace("/", "_") + "_";
    # Write the guard
    n_header.write("#ifndef " + l_guardId + "\n");
    n_header.write("#define " + l_guardId + "\n");
    # Windows specific file:
    n_header.write("#ifdef _WIN32\n");
    # Write the include
    n_header.write("#include \"glcorearb.h\"\n");
    n_header.write("#include \"" + n_filenames["core"] + "\"\n");

    # Add the CPP guard
    n_header.write("#ifdef __cplusplus\n");
    n_header.write("extern \"C\" {\n");
    n_header.write("#endif /*__cplusplus*/\n");

    # Write all the function prototypes
    for l_fnName in n_listing:
        # Get the function pointer type:
        l_ptrType = "PFN" + l_fnName.upper() + "PROC";
        # Write the header
        n_header.write("extern " + l_ptrType + " " + l_fnName + ";\n");

    # Write the loader prototypes
    n_header.write(textwrap.dedent("""
        //  wglgLoadFunctions() : Load OpenGL binding of Windows Junk.
        // This will load Win32 OpenGL extensions needed for Windows to not shit on you.
        // Returns:
        //  int: 0 or greater on success.
        //       -1 or less on failure.
        //       The absolute value refers to number of null functions.
        int wglgLoadFunctions();

        """));

    # Close the CPP guard
    n_header.write("#ifdef __cplusplus\n");
    n_header.write("} /*extern \"C\"*/\n");
    n_header.write("#endif /*__cplusplus*/\n");

    # Windows specific file:
    n_header.write("#endif /*_WIN32*/\n");

    # Close the guard
    n_header.write("#endif//" + l_guardId + "\n");
    
    return True;

def glgGenerateSource(n_filenames, n_source, n_listing):
    # Write the topper
    n_source.write("//===============================================================================================//\n");
    n_source.write("//\n");
    n_source.write("//      1Engine autogenerated source.\n");
    n_source.write("//\n");
    n_source.write("// Please be gentle. Generated from " + n_filenames["core"] + "\n");
    n_source.write("//\n");
    n_source.write("//===============================================================================================//\n");
    # Windows specific file:
    n_source.write("#ifdef _WIN32\n");
    # Write the include
    n_source.write("#include \"" + n_filenames["himpl"] + "\"\n");
    n_source.write("#include \"core/os.h\"\n");

    # Add the CPP guard
    n_source.write("#ifdef __cplusplus\n");
    n_source.write("extern \"C\" {\n");
    n_source.write("#endif /*__cplusplus*/\n");

    # First, write the function pointer definitions:
    for l_fnName in n_listing:
        # Get the function pointer type:
        l_ptrType = "PFN" + l_fnName.upper() + "PROC";
        # Write the header
        n_source.write(l_ptrType + " " + l_fnName + " = NULL;\n");

    # Write the private loaders:
    n_source.write(textwrap.dedent("""
        #ifdef _WIN32

        #ifdef _MSC_VER
        #pragma warning(disable: 4055)
        #pragma warning(disable: 4054)
        #endif

        static HMODULE g_module = NULL;

        static int glgInitializeGlobals()
        {
            // Load up the module:
            g_module = GetModuleHandleA("OpenGL32.dll");
            if (g_module == NULL)
                return 1;
            return 0;
        }

        static int glgTestPointer(const PROC pTest)
        {
            ptrdiff_t iTest;
            // If pTest is NULL, then didn't find it.
            if (!pTest)
                return 0;
            // Test the pointer against common failure values.
            iTest = (ptrdiff_t)pTest;
            if (iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1)
                return 0;
            // Not any of the previous? Return success.
            return 1;
        }

        static PROC glgGetProcAddress(const char* name)
        {
            PROC pFunc = wglGetProcAddress((LPCSTR)name);
            if (glgTestPointer(pFunc))
                return pFunc;
            return (PROC)GetProcAddress(g_module, (LPCSTR)name);
        }

        #endif//_WIN32

        // Loads the library.
        static int glgLoadLibrary()
        {
            int numFailed = 0;
        """));

    # Write all the loaders:
    for l_fnName in n_listing:
        # Get the function pointer type:
        l_ptrType = "PFN" + l_fnName.upper() + "PROC";
        # Write the header
        n_source.write("\t" + l_fnName + " = (" + l_ptrType + ")glgGetProcAddress(\"" + l_fnName + "\");\n");
        n_source.write("\tif (" + l_fnName + " == NULL) ++numFailed;\n");

    # Finish up the loader:
    n_source.write("    return numFailed;\n");
    n_source.write("} //glgLoadLibrary\n");

    # Write the public loaders:
    n_source.write(textwrap.dedent("""
        static const char * (WINAPI *wglGetExtensionsStringARB)(HDC) = NULL;

        int wglgLoadFunctions()
        {
            int numFailed = 0;
            int failed = 0;
            
            // Init the globals needed
            if (glgInitializeGlobals())
                return -1;

            // Pull GetExtensionsString for extension loading.
            wglGetExtensionsStringARB = (const char * (WINAPI *)(HDC))glgGetProcAddress("wglGetExtensionsStringARB");
            if (!wglGetExtensionsStringARB) {
                ++numFailed;
                failed = 1;
            }

            // Now, load the actual library.
            if (!failed) 
                numFailed += glgLoadLibrary();
            
            // We made it!
            if (failed == 0)
                return numFailed;
            else 
                return -numFailed;
        }
        """));

    # Close the CPP guard
    n_source.write("#ifdef __cplusplus\n");
    n_source.write("} /*extern \"C\"*/\n");
    n_source.write("#endif /*__cplusplus*/\n");

    # Windows specific file:
    n_source.write("#endif /*_WIN32*/\n");

    return True;

# Open the input file:
m_coreFile = open(m_filenames["core"], "rb");
m_functionListing = [];

# Read the source file
glgParseDefinitions(m_coreFile, m_functionListing);
# Done withe core file
m_coreFile.close();

# Open output files:
m_cimplFile = open(m_filenames["cimpl"], "w");
m_himplFile = open(m_filenames["himpl"], "w");
# Generate the definitions
if (m_himplFile != None):
    glgGenerateHeader(m_filenames, m_himplFile, m_functionListing);
if (m_cimplFile != None):
    glgGenerateSource(m_filenames, m_cimplFile, m_functionListing);

# Close the output files now.
m_cimplFile.close();
m_himplFile.close();
