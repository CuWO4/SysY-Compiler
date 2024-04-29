#include "../include/aggregate_agent.h"

namespace ast {
    AggregateAgent::AggregateAgent(std::vector<int> dimensions) 
        : dimension_at(-1), dimensions(dimensions) {

        std::vector<int> element_count;
        int total_element_count = 1;
        for (auto it = dimensions.rbegin(); it != dimensions.rend(); it++) {
            total_element_count *=* it;
            element_count.push_back(total_element_count);
        }
        this->element_count = std::vector<int>(
            element_count.rbegin(), element_count.rend()
        );
    }

    void AggregateAgent::fill(int element) {
        if (filled_elements.size() > element_count[0]) {
            throw "initializer exceeds";
        }
        filled_elements.push_back(element);
    }

    void AggregateAgent::enter_aggregate() {
        int enter_layer = 0;
        do {
            dimension_at++;
            enter_layer++;
            if (dimension_at >= dimensions.size()) {
                throw "improper initializer";
            }
        } while (filled_elements.size() % element_count[dimension_at] != 0);
        enter_layer_stack.push(enter_layer);
    }

    void AggregateAgent::leave_aggregate() {
        while (filled_elements.size() % element_count[dimension_at] != 0) {
            filled_elements.push_back(0);
        }

        dimension_at -= enter_layer_stack.top();
        enter_layer_stack.pop();
    }

    koopa::Initializer* AggregateAgent::to_aggregate() {
        int element_idx = 0;
        return to_aggregate(0, element_idx);
    }

    koopa::Initializer* AggregateAgent::to_aggregate(
        int dimension_at, int& element_idx
    ) {
        std::vector<koopa::Initializer*> initializers;

        for (int i = 0; i < dimensions[dimension_at]; i++) {
            if (dimension_at == dimensions.size() - 1) {
                initializers.push_back(
                    new koopa::ConstInitializer(filled_elements[element_idx++])
                );
            }
            else {
                initializers.push_back(
                    to_aggregate(dimension_at + 1, element_idx)
                );
            }
        }

        return new koopa::Aggregate(initializers);
    }
}