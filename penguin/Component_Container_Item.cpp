/*
 * Copyright (c) 2018 Michael Mathers
 */
#include "Component_Container_Item.h"
#include <sstream>


namespace Penguin
{
    Component_Container_Item::Component_Container_Item(const Component_Container_Item& other)
    {
        *this = other;
    }


    Component_Container_Item::Component_Container_Item(const std::filesystem::path& lib_path, const std::string& entry_func, const std::string& command_line)
        : library_path(lib_path)
        , entry_function(entry_func)
        , command_line_args(command_line)
    {
        this->library_handle = Penguin::load_library(this->library_path);
        if (this->library_handle == NULL)
        {
            std::stringstream error_stream;
            error_stream << "Unable to load library - " << this->library_path.c_str();
            throw std::runtime_error(error_stream.str().c_str());
        }

        Penguin::Function_Address function_address = Penguin::get_library_function(this->library_handle, this->entry_function);
        if (function_address == NULL)
        {
            std::stringstream error_stream;
            error_stream << "Unable to find function - " << this->entry_function.c_str() << " in library - " << this->library_path.c_str();
            Penguin::unload_library(this->library_handle);
            throw std::runtime_error(error_stream.str().c_str());
        }

        _component_entry_point entry_point = (_component_entry_point)function_address;
        this->component = std::shared_ptr<Component>(entry_point(this->command_line_args));

        if (this->component == nullptr)
        {
            std::stringstream error_stream;
            error_stream << "Unable to create container item using - " << this->entry_function.c_str() << " in library - " << this->library_path.c_str();
            Penguin::unload_library(this->library_handle);
            throw std::runtime_error(error_stream.str().c_str());
        }
    }


    Component_Container_Item::~Component_Container_Item(void)
    {
        if (this->component.use_count() <= 1)
        {
            // Down to 1 reference or less implies we are the only ones left. Hence, delete and unload
            this->component.reset();
            Penguin::unload_library(this->library_handle);
        }
    }


    Component_Container_Item&
    Component_Container_Item::operator = (const Component_Container_Item& other)
    {
        this->library_path = other.library_path;
        this->entry_function = other.entry_function;
        this->command_line_args = other.command_line_args;
        this->library_handle = other.library_handle;
        this->component = other.component;
        return *this;
    }
}

