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
        , start_time_(std::chrono::high_resolution_clock::now())
    {
    }


    Profiler::~Profiler(void)
    {
        // Capture the finish time
        std::chrono::time_point<std::chrono::high_resolution_clock> finish_time = std::chrono::high_resolution_clock::now();

        // Add to the overall results
        // auto result = Profiler::profiler_reports_.find(this->identifier_);
        //if (result != Profiler::profiler_reports_.end())
        // {
        _duration_type diff = std::chrono::duration_cast<_duration_type>(finish_time - this->start_time_);
        Profiler::profiler_reports[this->identifier_].push_back(diff);
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
            float percentage = total.count() / overall.count() * 100.0f;
            std::cout << report.second.size() << " : ";
            std::cout << report.first.c_str() << " : ";
            std::cout << percentage << "% - [ ";
            std::cout << min.count() << " ( ";
            std::cout << avg.count() << " ) ";
            std::cout << max.count() << " ] " << std::endl;
        }
        std::cout << "Total execution time = " << overall.count() << "ns" << std::endl;
    }
}