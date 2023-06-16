#pragma once
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <ranges>

template<typename State, typename Action>
class Learning{
public:
    float learning_rate{0.1};
    float discount_factor{0.5};

    float sample_delta{0};

    float initial_q = 100;

    bool converged{true};

    struct StateAction{
        State state;
        Action action;
        State end_state;
        float reward;
    };

    std::vector<StateAction> trace;

    std::map<State, std::map<Action, float>> q_values;

    void performed_action(State state, Action action, State result, float reward, int step_count = 0){
        trace.push_back({state, action, result, reward});
    }

    void finish(float reward){
        for(int i = trace.size() - 1; i>=0; i--){
            StateAction& step = trace[i];
            auto& q_value = q_values.at(step.state).at(step.action);
            auto left_over = (1.0-learning_rate) * q_value;
            q_value = left_over + learning_rate * (best_q(step.end_state) + step.reward);
            
        }
        trace.clear();
    }

    Action suggest_best(const State& state){
        std::vector<Action> actions;
        float best_q = -std::numeric_limits<float>::max();
        auto& action_pairs = q_values.at(state);
        for(auto& [action, value] : action_pairs){
            if(value - sample_delta > best_q){
                actions.clear();
                actions.push_back(action);
                best_q = value;
            }else if(best_q - sample_delta <= value && value <= best_q + sample_delta){
                actions.push_back(action);
            }

        }

        if(actions.empty())
            throw std::exception();

        int index = rand() % actions.size();
        return actions[index];
    }

    bool is_new_state(const State& state){
        return q_values.find(state) == q_values.end();
    }

    void populate(const State& state, const std::vector<Action>& actions){
        auto& values = q_values[state];
        for(const Action& act : actions){
            if(values.find(act) == values.end())
                values[act] = initial_q;
        }
    }
private:
    float best_q(const State& state){
        if(q_values.find(state) == q_values.end())
            return 0;

        float best_value = -std::numeric_limits<float>::max();
        for(const auto& [_, value] : q_values.at(state))
            best_value = std::max(best_value, value);
        return discount_factor * best_value;
    }
};