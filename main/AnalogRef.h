#pragma once

struct AnalogRef
{
    AnalogRef() = default;

    AnalogRef(int v, int var) : value(v), variance(var)
    {
    }

    int value = 0;
    int variance = 0;
};