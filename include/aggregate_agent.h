#ifndef AGGREGATE_AGENT_H_
#define AGGREGATE_AGENT_H_

#include "koopa.h"

#include <stack>

namespace ast {
    /**
     * handle aggregate, align initializers
     * @example int a[3][2][2] = { 1, 2, 1, 2, {5}, 1 }
     *      => { 
     *          { { 1, 2 }, { 1, 2 } }, 
     *          { { 5, 0 }, { 0, 0 } }, 
     *          { { 1, 0 }, { 0, 0 } }
     *      }
     * check C standard for details
     */
    class AggregateAgent {
    public:
        AggregateAgent(std::vector<int> dimensions);

        /**
         * @throw <const char*>  if initializer exceed
         */
        void fill(int element);

        /**
         * try to find the first aligned boundary encountered aggregate, otherwise
         * @throw <const char*>
         */
        void enter_aggregate();
        void leave_aggregate();

        koopa::Initializer* to_aggregate();

    private:
        int dimension_at;
        std::vector<int> dimensions;
        std::vector<int> filled_elements;

        /**
         * @example [2][3][4].element_count => { 24, 12, 4 }
         */
        std::vector<int> element_count;
        /**
         * mark the levels entered
         */
        std::stack<int> enter_layer_stack;

        koopa::Initializer* to_aggregate(int dimension_at, int& element_idx);
    };
}

#endif