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

    public:
        enum class Result
        {
            OK,
            FAIL_KEY_ALREADY_IN_USE,
            FAIL_CONTAINER_INSERTION_FAILED,
            FAIL_KEY_NOT_FOUND,
        };

    public:
        Component_Container(void);
        Component_Container(const Component_Container& other);
        Component_Container(Component_Container&& other);
        virtual ~Component_Container(void);

    public:
        Component_Container& operator = (const Component_Container& other);
        Component_Container& operator = (Component_Container&& other) noexcept;

    public:
        Result load_component(const std::string& key, const Component_Container_Item& component);
        Result load_component(std::string&& key, Component_Container_Item&& component);

        Result unload_component(const std::string& key);

    
    private:
        _component_map_type container_;
    };
}


#endif // PENGUIN_COMPONENT_CONTAINER_H
