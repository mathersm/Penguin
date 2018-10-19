/*
 * Copyright (c) 2018 Michael Mathers
 */
#include "Component_Container.h"
#include <iostream>


namespace Penguin
{
    Component_Container::Component_Container(void)
    {
    }


    Component_Container::~Component_Container(void)
    {
        this->component_map_.clear();
    }


    int
    Component_Container::load_component(const std::string& key, const std::filesystem::path& library_path, const std::string& entry_function, const std::string& command_line_args)
    {
        // Check for the presence of the key first before attempting to construct a new container item
        auto find_result = this->component_map_.find(key);
        if (find_result == this->component_map_.end())
        {
            auto insertion_result = this->component_map_.insert(std::make_pair(key, Penguin::Component_Container_Item(library_path, entry_function, command_line_args)));
            if (insertion_result.second == false)
            {
                std::cerr   << "ERROR! - Component container could not construct new component with key = " << key.c_str()
                            << ". Could not load " << library_path.c_str() << ":" << entry_function.c_str() << '\n';
                return -1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            std::cerr   << "ERROR! - Component container already contains a component with key = " << key.c_str()
                        << ". Could not load " << library_path.c_str() << ":" << entry_function.c_str() << '\n';
            return -1;
        }
    }


    int
    Component_Container::unload_component(const std::string& key)
    {
        if (this->component_map_.erase(key) == 1)
        {
            return 0;
        }
        else
        {
            std::cerr << "ERROR! - Component container doesn't contain a component with key = " << key.c_str()
                << ". Could not unload." << '\n';
            return -1;
        }
    }
}
