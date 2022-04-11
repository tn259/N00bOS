#pragma once

#include "config.h"

class heap_strategy;

class heap_strategy_factory {
public:
    void make_strategy(heap_algorithm algorithm, heap_strategy* strategy_ptr);
};