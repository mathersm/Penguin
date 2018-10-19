/*
 * Copyright (c) 2018 Michael Mathers
 */
#ifndef PENGUIN_DYNAMIC_LIBRARY_H
#define PENGUIN_DYNAMIC_LIBRARY_H


#include "Penguin_export.h"
#include <filesystem>


#if defined(__GNUG__) 
# include <dlfcn.h>
#elif defined(_MSC_VER) 
# ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
# endif
# include <Windows.h>
#endif 


namespace Penguin
{
#if defined(__GNUG__) 
    using Library_Handle    = void*;
    using Function_Address  = void*;
    const std::string dynamic_library_extension("so");
    const std::string dynamic_library_prefix("lib");
#elif defined(_MSC_VER) 
    using Library_Handle    = HMODULE;
    using Function_Address  = FARPROC;
    const std::string dynamic_library_extension("dll");
#endif 


    Penguin_Export Function_Address    get_library_function(Library_Handle library_handle, const std::string& function_name);
    Penguin_Export Library_Handle      load_library(const std::filesystem::path& library_path);
    Penguin_Export int                 unload_library(Library_Handle library_handle);


    // Using C linkage here to provide a non-mangled name that can be found with a simplified look up
    extern "C" Penguin_Export int       test_library_function(void);
}


#endif // PENGUIN_DYNAMIC_LIBRARY_H
