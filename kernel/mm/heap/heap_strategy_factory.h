#pragma once

#include "config.h"

namespace mm {
namespace heap {

class heap_strategy;

namespace strategy_factory {
/**
 * @brief Assigns a heap implementation based on the algorithm type
 * 
 * @param algorithm
 * @param strategy_ptr - the heap implementation 
 */
void make_strategy(heap_algorithm algorithm, heap_strategy* strategy_ptr);
}  // namespace strategy_factory

}  // namespace heap
}  // namespace mm