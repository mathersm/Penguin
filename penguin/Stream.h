/*
 * Copyright (c) 2025 Michael Mathers
 */
#ifndef PENGUIN_STREAM_H
#define PENGUIN_STREAM_H


namespace Penguin
{
    template <typename STREAM, typename T>
    void stream_out(STREAM& stream, const T& output)
    {
        stream << output;
    }

    template <typename STREAM, typename T, typename ...U>
    void stream_out(STREAM& stream, const T& output, const U&... more_output)
    {
        stream << output;
        return stream_out(stream, more_output...);
    }
}


#endif // PENGUIN_STREAM_H