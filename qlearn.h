#pragma once
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <iostream>
#include<cstdlib>
#include<ctime>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec){
    for(const T& t : vec){
        os << t << ' ';
    }
    return os;
}

template<typename State, typename Action>
class Learning{
public:
    float learning_rate{0.1};
    float discount_factor{0.9};

    float sample_delta{1000};

    float initial_q = 5000;

    struct StateAction{
        State state;
        Action action;
        float reward;
    };

    std::vector<StateAction> trace;

    std::map<State, std::map<Action, float>> q_values;

    void performed_action(State state, Action action, State result, float reward, int step_count = 0){
        trace.push_back({state, action, reward});

        // auto& q_value = q_values[state][action];

        // auto left_over = (1.0-learning_rate) * q_value;
        // auto optimal = std::pow(discount_factor, step_count) * max_future(result);
        // q_value = left_over + learning_rate * (optimal + reward);
    }

    void finish(float reward){
        float last_q;
        for(int i = trace.size() - 1; i>=0; i--){
            StateAction& step = trace[i];
            auto& q_value = q_values.at(step.state).at(step.action);
            auto old = q_value;

            auto left_over = (1.0-learning_rate) * q_value;
            float future;
            if(i == trace.size() - 1)
                future = std::pow(discount_factor, i) * reward;
            else
                future = std::pow(discount_factor, i) * last_q;
            q_value = left_over + learning_rate * (future + step.reward);

            last_q = q_value;
            std::cout << "Changed " << q_value - old << '\n';
        }
        trace.clear();
    }

    //void finish_trace(State state, ) last state should have a reward too

    Action suggest_best(const State& state){
        std::vector<Action> actions;
        float best_q = -std::numeric_limits<float>::max();
        auto& action_pairs = q_values.at(state);
        std::cout << "\n\nActions:\n";
        for(auto& [action, value] : action_pairs){
            std::cout << action << " -> " << value << '\n';
            if(value - sample_delta > best_q){
                actions.clear();
                actions.push_back(action);
                best_q = value;
            }else if(value + sample_delta >= best_q){
                actions.push_back(action);
            }
        }
        std::cout << "Usable " << actions;

        if(actions.empty())
            throw std::exception();

        int index = rand() % actions.size();
        std::cout << " using " << index <<'\n';

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
    float max_future(const State& state){
        if(q_values.find(state) == q_values.end())
            return 0;

        const auto& actions = q_values.at(state);
        if(actions.empty())
            return 0;

        float min = std::numeric_limits<float>::min();
        for(const auto& [_, value] : actions){
            if(value > min)
                min = value;
        }
        return min;
    }
};