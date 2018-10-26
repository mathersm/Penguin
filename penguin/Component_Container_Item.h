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


    class Component_Container_Item
    {
    public:
        Component_Container_Item(void);
        Component_Container_Item(const Component_Container_Item& other);
        Component_Container_Item(Component_Container_Item&& other);
        ~Component_Container_Item(void);

        Component_Container_Item& operator = (const Component_Container_Item& other);
        Component_Container_Item& operator = (Component_Container_Item&& other) noexcept;

        std::filesystem::path       get_library_path(void) const;
        std::string                 get_entry_function(void) const;
        std::string                 get_command_line_args(void) const;
        Penguin::Library_Handle     get_library_handle(void) const;
        std::shared_ptr<Component>  get_component(void) const;

    private:
        std::filesystem::path       library_path_;
        std::string                 entry_function_;
        std::string                 command_line_args_;
        Penguin::Library_Handle     library_handle_;
        std::shared_ptr<Component>  component_;
    };

}


#endif // PENGUIN_COMPONENT_CONTAINER_ITEM_H

