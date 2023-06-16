#include "qlearn.h"
#include "tictactoe.h"

#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

template<typename T>
T get_random(std::vector<T>&& vec){
    return std::move(vec.at(rand() % vec.size()));
}

const char* str(int place){
    switch(place){
        case 0: return " ";
        case 1: return "x";
        case 2: return "o";
    }
    throw std::exception();
}

void print(const GameState& state){
    std::cout << "┏━━━┳━━━┳━━━┓\n";
    std::cout << "┃ " << str(state[0]) << " ┃ " << str(state[1]) << " ┃ " << str(state[2]) << " ┃\n";
    std::cout << "┣━━━╋━━━╋━━━┫\n";
    std::cout << "┃ " << str(state[3]) << " ┃ " << str(state[4]) << " ┃ " << str(state[5]) << " ┃\n";
    std::cout << "┣━━━╋━━━╋━━━┫\n";
    std::cout << "┃ " << str(state[6]) << " ┃ " << str(state[7]) << " ┃ " << str(state[8]) << " ┃\n";
    std::cout << "┗━━━┻━━━┻━━━┛\n";
}

void print_game(const GameState& state){
    std::cout << "┏━━━┳━━━┳━━━┓\n";
    std::cout << "┃ " << (state[0]==0?"0":str(state[0])) << " ┃ " << (state[1]==0?"1":str(state[1])) << " ┃ " << (state[2]==0?"2":str(state[2])) << " ┃\n";
    std::cout << "┣━━━╋━━━╋━━━┫\n";
    std::cout << "┃ " << (state[3]==0?"3":str(state[3])) << " ┃ " << (state[4]==0?"4":str(state[4])) << " ┃ " << (state[5]==0?"5":str(state[5])) << " ┃\n";
    std::cout << "┣━━━╋━━━╋━━━┫\n";
    std::cout << "┃ " << (state[6]==0?"6":str(state[6])) << " ┃ " << (state[7]==0?"7":str(state[7])) << " ┃ " << (state[8]==0?"8":str(state[8])) << " ┃\n";
    std::cout << "┗━━━┻━━━┻━━━┛\n";
}

int main(){
    srand(time(NULL));
    Learning<GameState, int> player1;
    Learning<GameState, int> player2;

    // Stats
    int win1 = 0;
    int win2 = 0;
    int draw = 0;
    int unique_states1 = 0;
    int unique_states2 = 0;

    // Use too many runs to ensure good results
    for(int i = 0; i < 10000000; i++){
        
        TicTacToe game{};
        GameState oldState2;
        // std::cin >> test;
        int action2;
        if((i%10000) == 0)
            std::cout << "Running game nr " << i << '\n';
        while(true){
            //Handle player1 turn
            GameState oldState1 = game.board;
            auto available_actions1 = game.get_actions();
            if(player1.is_new_state(game.board)){
                player1.populate(game.board, available_actions1);
                unique_states1++;
            }
            int action1 = player1.suggest_best(game.board);
            game.play_action(action1);
            bool player1_won = game.check_winner(1);
            if(player1_won){
                player1.performed_action(oldState1, action1, game.board, 250);
                player2.performed_action(oldState2, action2, game.board, -250);
                win1++;
                break;   
            }else if(available_actions1.size() == 1){
                player1.performed_action(oldState1, action1, game.board, -10);
                player2.performed_action(oldState2, action2, game.board, -10);
                draw++;
                break;
            }
            if(available_actions1.size() != 9){
                player2.performed_action(oldState2, action2, game.board, 0);
            }

            //Handle player2 turn
            oldState2 = game.board;
            auto available_actions2 = game.get_actions();
            if(player2.is_new_state(game.board)){
                player2.populate(game.board, available_actions2);
                unique_states2++;
            }
            action2 = player2.suggest_best(game.board);
            game.play_action(action2);
            bool player2_won = game.check_winner(2);
            if(player2_won){
                player1.performed_action(oldState1, action1, game.board, -250);
                player2.performed_action(oldState2, action2, game.board, 250);
                win2++;
                break;   
            }
            
            player1.performed_action(oldState1, action1, game.board, 0);
        }

        player1.finish(0);
        player2.finish(0);
    }

    std::cout << "Stats:\n"
    << " Player 1 won: " << win1 << '\n'
    << " Player 2 won: " << win2 << '\n'
    << " Total draws: " << draw << '\n'
    << " States1 : " << unique_states1 << '\n'
    << " States2 : " << unique_states2 << '\n';
    
    player2.sample_delta = 0;
    while(true){
        TicTacToe game;
        
        // Play as first player
        // while(!game.get_actions().empty()){
        //     print_game(game.board);
        //     int action;
        //     std::cin >> action;
        //     game.play_action(action);
            
        //     if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty())
        //         break;
            
        //     print_game(game.board);
        //     std::this_thread::sleep_for(500ms);
        //     game.play_action(player2.suggest_best(game.board));

        //     if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty()){
        //         print_game(game.board);
        //         break;
        //     }
        // }

        // Play second
        while(!game.get_actions().empty()){
            game.play_action(player1.suggest_best(game.board));
            print_game(game.board);
            
            if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty())
                break;

            int action;
            std::cin >> action;
            game.play_action(action);
            print_game(game.board);
            std::this_thread::sleep_for(500ms);

            if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty()){
                print_game(game.board);
                break;
            }
        }
    }

    return 0;
}