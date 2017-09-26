/*
* Copyright (c) 2017 Michael Mathers
*/
#ifndef PENGUIN_PROFILER_H
#define PENGUIN_PROFILER_H


#include "Penguin_export.h"
#include <chrono>
#include <map>
#include <vector>


#define PENGUIN_PROFILING_ON 0
#define PENGUIN_CORE_PROFILING_ON 0
#define PENGUIN_PROFILER_ACCURACY std::micro
#define PENGUIN_PROFILER_ACCURACY_TEXT "us"


namespace Penguin
{
    class Penguin_Export Profiler
    {
    public:
        using _clock_type       = std::chrono::high_resolution_clock;
        using _duration_type    = std::chrono::duration<double, PENGUIN_PROFILER_ACCURACY>;
        using _time_point_type  = std::chrono::time_point<_clock_type>;
        using _report_type      = std::vector<_duration_type>;

        explicit Profiler(const std::string& id);
        virtual ~Profiler(void);

        static void dump_results(void);

    protected:
    private:
        static std::map<std::string, _report_type> profiler_reports;

        std::string identifier_;
        _time_point_type start_time_;
    };
}


#if defined(PENGUIN_PROFILING_ON) && (PENGUIN_PROFILING_ON > 0)
#define PENGUIN_PROFILE(str) Penguin::Profiler _profiler(str)
#define PENGUIN_PROFILE_DUMP Penguin::Profiler::dump_results()
# if defined(PENGUIN_CORE_PROFILING_ON) && (PENGUIN_CORE_PROFILING_ON > 0)
#  define PENGUIN_CORE_PROFILE(str) Penguin::Profiler _core_profiler(str)
# else
#  define PENGUIN_CORE_PROFILE(str)
# endif
#else
# define PENGUIN_PROFILE(str)
# define PENGUIN_CORE_PROFILE(str)
# define PENGUIN_PROFILE_DUMP
#endif

#endif // PENGUIN_PROFILER_H