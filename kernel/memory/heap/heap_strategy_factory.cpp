#include "config.h"

#include "heap_strategy_factory.h"
#include "heap.h"

#include "find_first_fit_strategy.h"

void heap_strategy_factory::make_strategy(heap_algorithm algorithm, heap_strategy* strategy_ptr) {
    switch (algorithm) {
        case heap_algorithm::find_first:
            find_first_fit::set_strategy(strategy_ptr);
            break;
        default:
            // TODO(tn259) error case here
            break;
    }
    strategy_ptr->create = &default_heap_create;
}