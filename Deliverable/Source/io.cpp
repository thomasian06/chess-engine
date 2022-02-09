

#include <iostream>
#include "chessBoard.hpp"
#include <string>
#include <assert.h>
#include "windows.h"

std::string printSquare(const int sq) {
    int file = files[sq];
    int rank = ranks[sq];
    std::string out = "";
    out += 'a' + file;
    out += '1' + rank;
    return out;
}

std::string printMove(const int move) {
    std::string out;
    int fileFrom = files[fromSQ(move)];
    int rankFrom = ranks[fromSQ(move)];
    int fileTo = files[toSQ(move)];
    int rankTo = ranks[toSQ(move)];
    int promoted = promoted(move);
    
    if (promoted) {
        char pchar = 'q';
        if (isKn(promoted)) {
            pchar = 'n';
        } else if (isRQ(promoted) && !isBQ(promoted)) {
            pchar = 'r';
        } else if (isBQ(promoted) && !isRQ(promoted)) {
            pchar = 'b';
        }
        out = 'a' + fileFrom;
        out += '1' + rankFrom;
        out += 'a' + fileTo;
        out += '1' + rankTo;
        out += pchar;
    } else {
        out = 'a' + fileFrom;
        out += '1' + rankFrom;
        out += 'a' + fileTo;
        out += '1' + rankTo;
    }
    return out;
}

int chessBoard::parseMove(std::string str) {

    if (str[1] > '8' || str[1] < '1' || str[3] > '8' || str[3] < '1' || 
        str[0] > 'h' || str[0] < 'a' || str[2] > 'h' || str[2] < 'a') { return noMove; }
    int a = str[0] - 'a';
    int b = str[1] - '1';
    int c = str[2] - 'a';
    int d = str[3] - '1';
    int from = FR2SQ(a, b);
    int to = FR2SQ(c, d);

    assert (sqOnBoard(from) && sqOnBoard(to));

    MoveList list[1];
    generateAllMoves(list);
    //int moveNum = 0; int move = 0; int promPiece = EMPTY;
    for (int i = 0; i < list->count; i++) {
        int move = list->moves[i].move;
        if (fromSQ(move) == from && toSQ(move) == to) {
            int promPiece = promoted(move);
            if (promPiece != EMPTY) {
                if (isRQ(promPiece) && !isBQ(promPiece) && str[4] == 'r') {
                    return move;
                } else if (!isRQ(promPiece) && isBQ(promPiece) && str[4] == 'b') {
                    return move;
                } else if (isRQ(promPiece) && isBQ(promPiece) && str[4] == 'q') {
                    return move;
                } else if (isKn(promPiece) && str[4] == 'n') {
                    return move;
                }
                continue;
            }
            return move;
        }
    }
    return noMove;
}

void printMoveList(const MoveList * list) {
    std::cout << "MoveList: " << list->count << std::endl;
    for (int i = 0; i < list->count; i++) {
        int move = list->moves[i].move;
        int score = list->moves[i].score;
        std::cout << "Move: " << i + 1 << " > " << printMove(move) << " (score: " << score << ")" << std::endl;
    }
    std::cout << "MoveList Total " << list->count << " Moves: " << std::endl;
}


int inputWaiting() {
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) {
            return 1;
        }
        return dw;
    } else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return (dw <= 1) ? 0 : dw;
    }
}

void chessBoard::readInput() {
    int bytes;
    std::string input;

    if (inputWaiting()) {    
		info->stopped = true;
		do {
		    getline(std::cin, input);
            bytes = sizeof(input);
		} while (bytes<0);
        if (input.compare("quit") == 0)    {
            info->quit = true;
        }
		return;
    }
}