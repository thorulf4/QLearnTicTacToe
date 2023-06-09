#include "qlearn.h"
#include "tictactoe.h"

#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;


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

int main(){
    srand(time(NULL));
    Learning<GameState, int> player1;
    Learning<GameState, int> player2;

    int win1 = 0;
    int win2 = 0;
    int draw = 0;

    for(int i = 0; i < 100000; i++){
        TicTacToe game{};
        GameState oldState2;
        int action2;

        std::cout << "Running game nr " << i;
        while(true){
            //Handle player1 turn
            GameState oldState1 = game.board;
            auto available_actions1 = game.get_actions();
            if(player1.is_new_state(game.board))
                player1.populate(game.board, available_actions1);
            int action1 = player1.suggest_best(game.board);
            game.play_action(action1);
            bool player1_won = game.check_winner(1);
            if(player1_won){
                std::cout << " --- Player 1 won\n";
                std::cout << "Player 1:\n";
                player1.finish(10000);
                std::cout << "Player 2:\n";
                player2.finish(-10000);
                win1++;
                break;   
            }else if(available_actions1.size() == 1){
                std::cout << " --- Draw\n";
                player1.finish(-500);
                player2.finish(-500);
                draw++;
                break;
            }

            if(available_actions1.size() != 9){
                player2.performed_action(oldState2, action2, oldState1, 0);
            }

            //Handle player2 turn
            oldState2 = game.board;
            auto available_actions2 = game.get_actions();
            if(player2.is_new_state(game.board))
                player2.populate(game.board, available_actions2);
            action2 = player2.suggest_best(game.board);
            game.play_action(action2);
            bool player2_won = game.check_winner(2);
            if(player2_won){
                std::cout << " --- Player 2 won\n";
                player1.finish(-10000);
                player2.finish(10000);
                win2++;
                break;   
            }
            
            player1.performed_action(oldState1, action1, game.board, 0);
        }
        print(game.board);
    }

    std::cout << "Stats:\n"
    << " Player 1 won: " << win1 << '\n'
    << " Player 2 won: " << win2 << '\n'
    << " Total draws: " << draw << '\n';

    while(true){
        TicTacToe game;

        while(!game.get_actions().empty()){
            print(game.board);
            int action;
            std::cin >> action;
            game.play_action(action);
            
            if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty())
                break;
            
            print(game.board);
            std::this_thread::sleep_for(500ms);
            game.play_action(player2.suggest_best(game.board));

            if(game.check_winner(1) || game.check_winner(2) || game.get_actions().empty()){
                print(game.board);
                break;
            }
        }
    }

    return 0;
}