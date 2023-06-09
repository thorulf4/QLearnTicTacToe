#pragma once
#include <array>
#include <vector>
#include <assert.h>

using GameState = std::array<int, 9>;

class TicTacToe{
public:
    GameState board{0,0,0,0,0,0,0,0,0};
    int turn = 1;
    
    std::vector<int> get_actions(){
        std::vector<int> actions;
        for(int i = 0; i < board.size(); i++){
            if(board[i] == 0){
                actions.push_back(i);
            }
        }
        return actions;
    }

    void play_action(int action){
        if(board[action] != 0)
            throw std::exception();

        board[action] = turn;
        turn = turn==1?2:1;
    }

    bool check_winner(int turn){
        return board[0] == turn && board[1] == turn && board[2] == turn ||
        board[3] == turn && board[4] == turn && board[5] == turn ||
        board[6] == turn && board[7] == turn && board[8] == turn ||

        board[0] == turn && board[3] == turn && board[6] == turn ||
        board[1] == turn && board[4] == turn && board[7] == turn ||
        board[2] == turn && board[5] == turn && board[8] == turn ||

        board[0] == turn && board[4] == turn && board[8] == turn ||
        board[2] == turn && board[4] == turn && board[6] == turn;
    }

};