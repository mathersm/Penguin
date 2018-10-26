/*
 * Copyright (c) 2018 Michael Mathers
 */
#include "Component_Container.h"


namespace Penguin
{
    Component_Container::Component_Container(void)
    {
    }


    Component_Container::Component_Container(const Component_Container& other)
    {
        *this = other;
    }


    Component_Container::Component_Container(Component_Container&& other)
    {
        *this = std::move(other);
    }


    Component_Container::~Component_Container(void)
    {
        this->container_.clear();
    }


    Component_Container&
    Component_Container::operator = (const Component_Container& other)
    {
        if (this != &other)
        {
            this->container_ = other.container_;
        }
        return *this;
    }


    Component_Container&
    Component_Container::operator = (Component_Container&& other) noexcept
    {
        if (this != &other)
        {
            this->container_ = std::move(other.container_);
        }
        return *this;
    }


    Component_Container::Result
    Component_Container::load_component(const std::string& key, const Component_Container_Item& component)
    {
        if (this->container_.find(key) == this->container_.end())
        {
            if (this->container_.insert(std::make_pair(key, component)).second == false)
            {
                return Result::FAIL_CONTAINER_INSERTION_FAILED;
            }
            else
            {
                return Result::OK;
            }
        }
        else
        {
            return Result::FAIL_KEY_ALREADY_IN_USE;
        }
    }


    Component_Container::Result
    Component_Container::load_component(std::string&& key, Component_Container_Item&& component)
    {
        if (this->container_.find(key) == this->container_.end())
        {
            if (this->container_.emplace(std::make_pair(key, component)).second == false)
            {
                return Result::FAIL_CONTAINER_INSERTION_FAILED;
            }
            else
            {
                return Result::OK;
            }
        }
        else
        {
            return Result::FAIL_KEY_ALREADY_IN_USE;
        }
    }


    Component_Container::Result
    Component_Container::unload_component(const std::string& key)
    {
        if (this->container_.erase(key) == 1)
        {
            return Result::OK;
        }
        else
        {
            return Result::FAIL_KEY_NOT_FOUND;
        }
    }
}
