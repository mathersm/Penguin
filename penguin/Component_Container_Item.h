/*
 * Copyright (c) 2018 Michael Mathers
 */
#ifndef PENGUIN_COMPONENT_CONTAINER_ITEM_H
#define PENGUIN_COMPONENT_CONTAINER_ITEM_H


#include "Penguin_export.h"
#include "Component.h"
#include "Dynamic_Library.h"


namespace Penguin
{
    typedef Component* (*_component_entry_point)(const std::string);


    struct Component_Container_Item
    {
        std::filesystem::path       library_path;
        std::string                 entry_function;
        std::string                 command_line_args;
        Penguin::Library_Handle     library_handle;
        std::shared_ptr<Component>  component;

        Component_Container_Item(void) = delete;

        Component_Container_Item(const Component_Container_Item& other);
        Component_Container_Item(const std::filesystem::path& lib_path, const std::string& entry_func, const std::string& command_line);

        ~Component_Container_Item(void);

        Component_Container_Item& operator = (const Component_Container_Item& other);
    };

}


#endif // PENGUIN_COMPONENT_CONTAINER_ITEM_H

