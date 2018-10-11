#ifndef PENGUIN_VARIADIC_TYPE_H
#define PENGUIN_VARIADIC_TYPE_H


#include <vector>


namespace Penguin
{
    template <typename... Types>
    std::tuple<Types...>
    {
        typename Types::type 
    };


    class ShipComponent
    {
    public:
    protected:
    private:
    };


    class Ship
    {
    public:
        Ship(std::initializer_list<ShipComponent> component_list)
        {
            this->components_.insert(this->components_.end(), component_list);
        };
    protected:

    private:
        std::vector<ShipComponent> components_;
    };


    using Fleet = std::vector<Ship>;

    void assign_damage(int damage, const Fleet& fleet);
}


#endif // PENGUIN_VARIADIC_TYPE_H
