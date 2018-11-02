/*
* Copyright (c) 2018 Michael Mathers
*/
#include <penguin/Version.h>
#include <vector>

//
//namespace Penguin::Eclipse
//{
//    enum class WeaponDice
//    {
//        NONE,
//        ION,
//        PLASMA,
//        SOLITON,
//        ANTI_MATTER,
//        RIFT,
//    };
//
//    struct ShipComponent
//    {
//        const int power_required;
//        const int initiative;
//        const int computer;
//        const int drive;
//        const int power_supplied;
//        const WeaponDice weapon;
//
//        constexpr ShipComponent(
//            int power_req,
//            int init,
//            int comp,
//            int dr,
//            int power_sup,
//            WeaponDice wep)
//            : power_required(power_req)
//            , initiative(init)
//            , computer(comp)
//            , drive(dr)
//            , power_supplied(power_sup)
//            , weapon(wep)
//        {}
//    };
//
//    constexpr ShipComponent electron_computer(0, 1, 0);
//    constexpr ShipComponent positron_computer(1, 2, 0);
//    constexpr ShipComponent gluon_computer(2, 3, 0);
//
//    constexpr ShipComponent ion_drive(1, 0, 1);
//    constexpr ShipComponent fusion_drive(2, 0, 2);
//
//
//    enum class Team
//    {
//        WHITE,
//        BLACK,
//    };
//
//    using Ship = std::vector<ShipComponent>;
//
//    Team calculate_outcome(std::vector<Ship> white_fleet, std::vector<Ship> black_fleet);
//}



#include <random>
#include <iostream>
#include <array>
#include "Die.h"


namespace Penguin
{
    struct ShipComponent
    {
        int power_;
        int initiative_;
        int computer_;
        int movement_;

        ShipComponent(void) = default;
        ShipComponent(const ShipComponent&) = default;
        
        ShipComponent& operator = (const ShipComponent&) = default;
    };


    const ShipComponent EmptySlot;


    template <size_t S>
    class Ship
    {
    public:
        Ship(void);
        Ship(const std::initializer_list<ShipComponent> component_list);

        const ShipComponent& get_component(size_t slot) const;
        bool is_valid(void) const;

    protected:
    private:
        std::array<ShipComponent, S> component_slots_;
    };


    template <size_t S>
    Ship<S>::Ship(void)
    {
        this->component_slots_.fill(EmptySlot);
    }


    template <size_t S>
    Ship<S>::Ship(const std::initializer_list<ShipComponent> component_list)
    {
        if (component_list.size() > S)
        {
            throw std::runtime_error("Component count exceeds available ship slots");
        }

        this->component_slots_.fill(EmptySlot);
        std::copy(component_list.begin(), component_list.end(), this->component_slots_.begin());
    }


    template <size_t S>
    const ShipComponent&
    Ship<S>::get_component(size_t slot) const
    {
        return this->component_slots_[slot];
    }


    template <size_t S>
    bool
    Ship<S>::is_valid(void) const
    {
        return true;
    }

}


int main(int argc, char* argv[])
{

    // Roll a dice and print the result
    Penguin::Die<int> d6{ 1, 2, 3, 4, 5, 6};

    std::cout << '\n' << "Rolling a D6" << '\n';
    for (int i = 1; i <= 10; i++)
    {
        std::cout << "Roll #" << i << " = " << d6.roll() << '\n';
    }


    Penguin::Die<std::string> rift_die{ "XXXO", "X", "-", "-", "X", "O" };

    std::cout << '\n' << "Rolling a Rift Die" << '\n';
    for (int i = 1; i <= 10; i++)
    {
        std::cout << "Roll #" << i << " = " << rift_die.roll().c_str() << '\n';
    }


    Penguin::ShipComponent sc1, sc2, sc3, sc4;

    Penguin::ShipComponent ion_source;
    ion_source.power_ = 3;

    Penguin::ShipComponent electron_computer;
    electron_computer.computer_ = 1;

    Penguin::ShipComponent ion_drive;
    ion_drive.power_ = -1;
    ion_drive.movement_ = 1;

    Penguin::ShipComponent ion_cannon;
    ion_cannon.power_ = -1;


    Penguin::Ship<5> star_base;
    Penguin::Ship<4> interceptor = { ion_source, ion_drive, ion_cannon };
    Penguin::Ship<6> cruiser = { ion_source, electron_computer, ion_drive, ion_cannon };

    return 0;
}
