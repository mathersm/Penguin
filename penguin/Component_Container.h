/*
 * Copyright (c) 2018 Michael Mathers
 */
#ifndef PENGUIN_COMPONENT_CONTAINER_H
#define PENGUIN_COMPONENT_CONTAINER_H


#include "Penguin_export.h"
#include "Component_Container_Item.h"
#include <map>


namespace Penguin
{
    class Penguin_Export Component_Container
    {
    public:
        using _component_map_type = std::map<std::string, Component_Container_Item>;

        Component_Container(void);
        virtual ~Component_Container(void);

        int load_component(const std::string& key, const std::filesystem::path& library_path, const std::string& entry_function, const std::string& command_line_args);
        // int load_component(const Component_Container_Item& component_config);
        // int load_components_from_file(const std::filesystem::path& configuration_file);

        int unload_component(const std::string& key);

    private:
        Component_Container(const Component_Container&) = delete;
        Component_Container& operator = (const Component_Container&) = delete;

    private:
        _component_map_type component_map_;
    };
}


#endif // PENGUIN_COMPONENT_CONTAINER_H
