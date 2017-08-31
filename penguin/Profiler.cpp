/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Profiler.h"
#include <algorithm>
#include <numeric>
#include <iostream>


namespace Penguin
{
    std::map<std::string, std::vector<Profiler::_duration_type>> Profiler::profiler_reports;

    Profiler::Profiler(const std::string& id)
        : identifier_(id)
        , start_time_(_clock_type::now())
    {
    }


    Profiler::~Profiler(void)
    {
        // Capture the finish time
        this->finish_time_ = _clock_type::now();

        // Add to the overall results
        Profiler::profiler_reports[this->identifier_].push_back(_duration_type(this->finish_time_ - this->start_time_));
        //}
    }


    void
    Profiler::dump_results(void)
    {
        _duration_type overall = _duration_type(0.0f);
        for (auto report : Profiler::profiler_reports)
        {
            overall += std::accumulate(std::begin(report.second), std::end(report.second), _duration_type(0.0f));
        }
        std::cout << std::endl;
        std::cout << "Penguin::Profiler" << std::endl;
        std::cout << "Calls : ID [min (avg) max]" << std::endl;
        for (auto report : Profiler::profiler_reports)
        {
            _duration_type min = *std::min_element(std::begin(report.second), std::end(report.second));
            _duration_type max = *std::max_element(std::begin(report.second), std::end(report.second));
            _duration_type total = std::accumulate(std::begin(report.second), std::end(report.second), _duration_type(0.0f));
            _duration_type avg =  total / report.second.size();
            double percentage = total.count() / overall.count() * 100.0;
            std::cout << report.second.size() << " : ";
            std::cout << report.first.c_str() << " : ";
            std::cout << percentage << "% - [ ";
            std::cout << min.count() << " ( ";
            std::cout << avg.count() << " ) ";
            std::cout << max.count() << " ] " << std::endl;
        }
        std::cout << "Total execution time = " << overall.count() << PENGUIN_PROFILER_ACCURACY_TEXT << std::endl;
    }
}