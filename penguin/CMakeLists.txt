# Create a library
add_library (Penguin SHARED
    Exception.cpp
    Exception.h
    Monitor.cpp
    Monitor.h
    Penguin_export.h
    Profiler.cpp
    Profiler.h
    Semaphore.cpp
    Semaphore.h
    Unbounded_Queue.h
    Version.h)

include (GenerateExportHeader)
generate_export_header(Penguin
    BASE_NAME Penguin
    EXPORT_MACRO_NAME Penguin_Export
    EXPORT_FILE_NAME Penguin_export.h
    STATIC_DEFINE Penguin_BUILT_AS_STATIC)

if (UNIX)
target_link_libraries(Penguin pthread)
endif (UNIX)
