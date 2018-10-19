/*
* Copyright (c) 2018 Michael Mathers
*/
#include <penguin/Component_Container.h>
#include <future>


int main(int argc, char *argv[])
{
    Penguin::Component_Container container;

    auto component_loader = [&](const std::string& key, const std::filesystem::path& library, const std::string& entry_point, const std::string& args)
    {
        container.load_component(key, library, entry_point, args);
    };

    // Load components asynchronously, some may take longer than others or may produce errors
    auto load_a = std::async(std::launch::async, component_loader, "Component A", "Test_Component", "Test_Component_Factory", "");
    auto load_b = std::async(std::launch::async, component_loader, "Component B", "Test_Component", "Test_Component_Factory", "");

    return 0;
}