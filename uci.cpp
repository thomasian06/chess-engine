

#include <iostream>
#include "chessBoard.hpp"
#include <string>
#include <cstring>
#include <sstream>

#define INPUTBUFFER 400 * 6

void chessBoard::parseGo(std::string inLine) {

    int depth = -1;
    int movesToGo = 30;
    int moveTime = -1;
    int t = -1;
    int inc = 0;
    info->timeSet = false;

    std::stringstream line;
    line << inLine;
    std::string word;
    line >> word;
    while (line >> word) {
        if (word.compare("infinite") == 0) {
            continue;
        } else if ((word.compare("binc") == 0) && pos->side == BLACK) {
            line >> word;
            inc = std::stoi(word, nullptr);
        } else if ((word.compare("winc") == 0) && pos->side == WHITE) {
            line >> word;
            inc = std::stoi(word, nullptr);
        } else if ((word.compare("wtime") == 0) && pos->side == WHITE) {
            line >> word;
            t = std::stoi(word, nullptr);
        } else if ((word.compare("btime") == 0) && pos->side == BLACK) {
            line >> word;
            t = std::stoi(word, nullptr);
        } else if (word.compare("movestogo") == 0) {
            line >> word;
            movesToGo = std::stoi(word, nullptr);
        } else if (word.compare("movetime") == 0) {
            line >> word;
            moveTime = std::stoi(word, nullptr);
        } else if (word.compare("depth") == 0) {
            line >> word;
            depth = std::stoi(word, nullptr);
        }
    }

    if (moveTime != -1) {
        t = moveTime;
        movesToGo = 1;
    }
    info->startTime = getTimeMs();
    info->depth = depth;

    if (t != -1) {
        info->timeSet = true;
        t /= movesToGo;
        t -= 50;
        info->stopTime = info->startTime + t + inc;
    }

    if (depth == -1) {
        info->depth = MAXDEPTH;
    }

    std::cout << "time: " << t << " start: " << info->startTime << " stop: " << info->stopTime <<
        " depth: " << info->depth << " timeset: " << info->timeSet << std::endl;
    searchPosition();

}

void chessBoard::parsePosition(std::string lineIn) {
    std::stringstream line;
    line << lineIn;
    std::string word;
    getline(line, word, ' ');
    getline(line, word, ' ');
    if (word.compare("startpos") == 0) {
        parseFEN(START_FEN);
        // std::cout << "startpos called" << std::endl;
        // std::cout << "Board: " << std::endl;
        // printBoard(pos);
    } else if (word.compare("fen") == 0) {
        getline(line, word);
        parseFEN(word);
        line.seekg(0);
        // std::cout << "Parsed fen " << word << std::endl;
        // std::cout << "Board: " << std::endl;
        // printBoard(pos);
    } else {
        parseFEN(START_FEN);
        // std::cout << "startpos called" << std::endl;
        // std::cout << "Board: " << std::endl;
        // printBoard(pos);
    }
    getline(line, word, 'm');
    if(!getline(line, word, ' ')) {
        printBoard();
        return;
    }
    while(getline(line, word, ' ')) {
        int move = parseMove(word);
        if (move == noMove) {
            break;
        }
        makeMove(move);
        pos->ply = 0;
    }
    printBoard();
}

void chessBoard::uciLoop() {

    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);

    info->mode = UCIMODE;

    std::string line;
    std::cout << "id name " << NAME << std::endl;
    std::cout << "id author Ian Thomas" << std::endl;
    std::cout << "uciok\n";

    while (true) {
        if (!std::getline(std::cin, line)) {
            continue;
        }
        if (line[0] == '\n') {
            continue;
        }
        std::stringstream stream;
        stream << line;
        std::string word;
        stream >> word;
        if (word.compare("isready") == 0) {
            std::cout << "readyok\n";
            continue;
        } else if (word.compare("position") == 0) {
            parsePosition(line);
        } else if (word.compare("ucinewgame") == 0) {
            parsePosition("position startpos\n");
        } else if (word.compare("go") == 0) {
            parseGo(line);
        } else if (word.compare("quit") == 0) {
            info->quit = true;
            break;
        } else if (word.compare("uci") == 0) {
            std::cout << "id name " << NAME << std::endl;
            std::cout << "id author Ian Thomas" << std::endl;
            std::cout << "uciok\n";
        }
        if (info->quit) {
            break;
        }
    }
}