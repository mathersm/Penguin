/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Dynamic_Library.h"
#include <iostream>


namespace
{
#if defined(__GNUG__) 
    void* get_library_function_linux(void* library_handle, const std::string& function_name)
    {
        // Call dlerror to clear any old error conditions
        dlerror();
        void* function_address = dlsym(library_handle, function_name.c_str());
        char* error_result = dlerror();
        if (error_result != nullptr)
        {
            std::cerr << "ERROR! Failed to find address for " << function_name.c_str() << " - " << error_result << '\n';
        }
        return function_address;
    }
#elif defined(_MSC_VER) 
    FARPROC get_library_function_windows(HMODULE library_handle, const std::string& function_name)
    {

        FARPROC function_address = GetProcAddress(library_handle, function_name.c_str());
        if (function_address == NULL)
        {
            std::cerr << "ERROR (" << GetLastError() << ")! - Failed to find address for " << function_name.c_str() << '\n';
        }
        return function_address;
    }
#endif


#if defined(__GNUG__) 
    void* load_library_linux(const std::string& library_path)
    {
        void* handle = dlopen(library_path.c_str(), RTLD_LAZY);
        if (handle == NULL)
        {
            std::cerr << "ERROR! Failed to load library " << library_path.c_str() << " - " << dlerror() << '\n';
        }
        return handle;
    }
#elif defined(_MSC_VER) 
    HMODULE load_library_windows(const std::string& library_path)
    {
        HMODULE module_handle = LoadLibrary(library_path.c_str());
        if (module_handle == NULL)
        {
            std::cerr << "ERROR (" << GetLastError() << ")! - Failed to load library " << library_path.c_str() << '\n';
        }
        return module_handle;
    }
#endif


#if defined(__GNUG__) 
    int unload_library_linux(void* library_handle)
    {
        int result = dlclose(library_handle);
        if (result != 0)
        {
            std::cerr << "ERROR! Failed to unload library - " << dlerror() << '\n';
        }
        return result;
    }
#elif defined(_MSC_VER) 
    int unload_library_windows(HMODULE library_handle)
    {
        BOOL result = FreeLibrary(library_handle);
        if (result == FALSE)
        {
            DWORD error = GetLastError();
            std::cerr << "ERROR (" << error << ")! - Failed to unload library!" << '\n';
        }
        return (result ? 0 : -1);
    }
#endif


    std::string fix_dynamic_library_filename(std::filesystem::path library_path)
    {
        if (library_path.has_extension() == false || library_path.extension().string().compare(Penguin::dynamic_library_extension) != 0)
        {
            // If there is either no extension, or an incorrect one, replace it
            library_path.replace_extension(Penguin::dynamic_library_extension);
        }
#if defined(__GNUG__)
        if (library_path.filename().string().compare(0, 3, Penguin::dynamic_library_prefix) != 0)
        {
            std::string new_filename(Penguin::dynamic_library_prefix);
            new_filename.append(library_path.filename().string());
            library_path.replace_filename(new_filename);
        }
#endif
        return library_path.string();
    }
}


namespace Penguin
{
    Function_Address
    get_library_function(Library_Handle library_handle, const std::string& function_name)
    {
#if defined(__GNUG__) 
        return get_library_function_linux(library_handle, function_name);
#elif defined(_MSC_VER) 
        return get_library_function_windows(library_handle, function_name);
#endif
    }

    Library_Handle
    load_library(const std::filesystem::path& library_path)
    {
#if defined(__GNUG__) 
        return load_library_linux(fix_dynamic_library_filename(library_path));
#elif defined(_MSC_VER) 
        return load_library_windows(fix_dynamic_library_filename(library_path));
#endif
    }


    int
    unload_library(Penguin::Library_Handle library_handle)
    {
#if defined(__GNUG__)
        return unload_library_linux(library_handle);
#elif defined(_MSC_VER) 
        return unload_library_windows(library_handle);
#endif
    }

}
