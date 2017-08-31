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
#define PENGUIN_PROFILER_ACCURACY std::micro
#define PENGUIN_PROFILER_ACCURACY_TEXT "us"

namespace Penguin
{
    class Penguin_Export Profiler
    {
    public:
        typedef std::chrono::high_resolution_clock                          _clock_type;
        typedef std::chrono::duration<double, PENGUIN_PROFILER_ACCURACY>    _duration_type;
        typedef std::chrono::time_point<_clock_type>                        _time_point_type;

        Profiler(const std::string& id);
        virtual ~Profiler(void);

        static void dump_results(void);

    protected:
    private:
        static std::map<std::string, std::vector<_duration_type>> profiler_reports;

        std::string identifier_;
        _time_point_type start_time_;
        _time_point_type finish_time_;
    };
}


#if defined(PENGUIN_PROFILING_ON) && (PENGUIN_PROFILING_ON > 0)
#define PENGUIN_PROFILE(str) Penguin::Profiler _profiler(str)
#define PENGUIN_PROFILE_DUMP Penguin::Profiler::dump_results()
#else
# define PENGUIN_PROFILE(str)
# define PENGUIN_PROFILE_DUMP
#endif

#endif // PENGUIN_PROFILER_H