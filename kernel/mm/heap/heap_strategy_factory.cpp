#include "heap_strategy_factory.h"

#include "config.h"
#include "find_first_fit_strategy.h"
#include "heap.h"

namespace mm {
namespace heap {
namespace strategy_factory {

void make_strategy(heap_algorithm algorithm, heap_strategy* strategy_ptr) { // NOLINT(readability-convert-member-functions-to-static)
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

}  // namespace strategy_factory
}  // namespace heap
}  // namespace mm