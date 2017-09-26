/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_LANGUAGE_H
#define PENGUIN_LANGUAGE_H


#if defined(__GNUG__)
# include <experimental/string_view>
#elif defined(_MSC_VER)
# include <string_view>
#endif


namespace Penguin
{
#if defined(__GNUG__)
    using string_view = std::experimental::string_view;
#elif defined(_MSC_VER)
    using string_view = std::string_view;
#endif
}


#endif // PENGUIN_LANGUAGE_H
