#ifndef PENGUIN_SAMPLE_DIE_H
#define PENGUIN_SAMPLE_DIE_H


#include <random>


namespace Penguin::Sample
{
    template <typename T>
    class Die
    {
    public:
        using _face_type = T;

        Die(const std::initializer_list<_face_type> face_list);

        const _face_type& roll(void);

    protected:
        Die(void) = delete;
        Die(const Die&) = delete;
        Die& operator = (const Die&) = delete;

    private:
        std::random_device                      random_device_;
        std::default_random_engine              random_engine_;
        std::uniform_int_distribution<size_t>   distribution_;
        std::vector<_face_type>                 faces_map_;
    };


    template <typename T>
    Die<T>::Die(const std::initializer_list<_face_type> face_list)
        : random_engine_(this->random_device_())
        , distribution_(0, face_list.size() - 1)
    {
        this->faces_map_.insert(this->faces_map_.end(), face_list);
    }


    template <typename T>
    const typename Die<T>::_face_type&
    Die<T>::roll(void)
    {
        return this->faces_map_[this->distribution_(this->random_engine_)];
    }
}


#endif // PENGUIN_SAMPLE_DIE_H