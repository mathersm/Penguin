/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Profiler.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <ios>


namespace Penguin
{
    std::map<std::string, Profiler::_report_type> Profiler::profiler_reports;


    Profiler::Profiler(const std::string& id)
        : identifier_(id)
        , start_time_(_clock_type::now())
    {
    }


    Profiler::~Profiler(void)
    {
        // Capture the finish time and add to the overall results
        Profiler::profiler_reports[this->identifier_].push_back(_duration_type(_clock_type::now() - this->start_time_));
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
        std::cout << "Calls : ID :  % of Total - [min , avg , max]" << std::endl;
        for (const auto& report : Profiler::profiler_reports)
        {
            _duration_type min = *std::min_element(std::begin(report.second), std::end(report.second));
            _duration_type max = *std::max_element(std::begin(report.second), std::end(report.second));
            _duration_type total = std::accumulate(std::begin(report.second), std::end(report.second), _duration_type(0.0f));
            _duration_type avg =  total / report.second.size();
            double percentage = total.count() / overall.count() * 100.0;
            std::cout << std::setw(4)   << std::right   << report.second.size() << " : ";
            std::cout << std::setw(48)  << std::left    << report.first.c_str() << " : ";
            std::cout << std::setw(8)   << std::right   << std::fixed << std::setprecision(3) << percentage << "% - [ ";
            std::cout << std::setw(16)  << std::right   << std::defaultfloat << std::setprecision(10) << min.count() << " , ";
            std::cout << std::setw(16)  << std::right   << std::defaultfloat << std::setprecision(10) << avg.count() << " , ";
            std::cout << std::setw(16)  << std::right   << std::defaultfloat << std::setprecision(10) << max.count() << " ] " << std::endl;
        }
        std::cout << "Total execution time = " << std::defaultfloat << overall.count() << PENGUIN_PROFILER_ACCURACY_TEXT << std::endl;
    }
}
