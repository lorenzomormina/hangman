#pragma once

#include <random>

struct Random
{
    std::random_device rd;
    std::default_random_engine gen;
    std::uniform_int_distribution<size_t> distrib;

    Random();
    void setSize(size_t size);
    int getNumber();
};


Random::Random()
{
    gen.seed(rd());
}

void Random::setSize(size_t size)
{
    distrib.param(std::uniform_int_distribution<size_t>::param_type(0, size - 1));
}

int Random::getNumber()
{
    return distrib(gen);
}