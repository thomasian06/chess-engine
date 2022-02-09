

#include <iostream>
#include "chessBoard.hpp"
#include <string.h>
#include <sstream>

int threeFoldRepetiton(const Board * pos) {
    int repeat = 0;
    for (int i = 0; i < pos->plyHistory; i++) {
        if (pos->history[i].positionKey == pos->positionKey) {
            repeat++;
        }
    }
    return repeat;
}

bool drawMaterial(const Board * pos) {
    if ((pos->pieceNum[wP] + pos->pieceNum[bP]) > 0) {
        return false;
    }
    if ((pos->pieceNum[wQ] + pos->pieceNum[bQ] + pos->pieceNum[wR] + pos->pieceNum[bR]) > 0) {
        return false;
    }
    if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) {
        return false;
    }
    if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) {
        return false;
    }
    if (pos->pieceNum[wN] && pos->pieceNum[wB]) {
        return false;
    }
    if (pos->pieceNum[bN] && pos->pieceNum[bB]) {
        return false;
    }
    return true;
}

bool chessBoard::checkResult() {
    if (pos->fiftyMove > 100) {
        std::cout << "1/2-1/2 {fifty move rule}" << std::endl;
        return true;
    }
    if (threeFoldRepetiton(pos) > 1) {
        std::cout << "1/2-1/2 {three-fold repetition}" << std::endl;
        return true;
    }
    if (drawMaterial(pos)) {
        std::cout << "1/2-1/2 {insufficient material}" << std::endl;
        return true;
    }

    MoveList list[1];
    generateAllMoves(list);
    bool found = false;
    for (int i = 0; i < list->count; i++) {
        if (!makeMove(list->moves[i].move)) {
            continue;
        }
        found = true;
        takeMove();
        break;
    }
    if (found) {
        return false;
    }
    bool check = sqAttacked(pos->kingSquare[pos->side], pos->side ^ 1);
    if (check) {
        if (pos->side == WHITE) {
            std::cout << "0 - 1 {black mates}" << std::endl;
        } else {
            std::cout << "0 - 1 {white mates}" << std::endl;
        }
    } else {
        std::cout << "1/2 - 1/2 {stalemate}" << std::endl;
    }
    return true;

}

void chessBoard::xboardLoop() {

    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);

    info->mode = XBOARDMODE;
    info->post = true;

    int depth = -1;
    int movesToGo[2] = {30, 30};
    int moveTime = -1;
    int t = -1;
    int increment = 0;
    int engineSide = BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = noMove;

    while (true) {

        if (pos->side == engineSide && !checkResult()) {
            std::cout << "Evaluating: " << std::endl;
            info->startTime = getTimeMs();
            info->depth = depth;

            if (t != -1) {
                info->timeSet = true;
                t /= movesToGo[pos->side];
                t -= 50;
                info->stopTime = info->startTime + t + increment;
            }

            if (depth == -1 || depth > MAXDEPTH) {
                info->depth = MAXDEPTH;
            }

            std::cout << "time: " << t << " start: " << info->startTime << " stop: " << info->stopTime <<
                " depth: " << info->depth << " timeset: " << info->timeSet << " moves to go: " << 
                movesToGo[pos->side] << " mps: " << mps << std::endl;
            searchPosition();

            if (mps != 0) {
                movesToGo[pos->side ^ 1]--;
                if (movesToGo[pos->side ^ 1] < 1) {
                    movesToGo[pos->side ^ 1] = mps;
                }
            }
        }

        std::string input;
        std::stringstream line;
        getline(std::cin, input);
        std::cout << "Command seen: " << input << std::endl;
        line << input;
        getline(line, input, ' ');

        if (input.compare("quit") == 0) {
            info->quit = true;
            break;
        }
        if (input.compare("force") == 0) {
            engineSide = BOTH;
            continue;
        }
        if (input.compare("protover") == 0) {
            std::cout << "feature ping=1 setboard=1 colors=0 usermove=1\n";
            std::cout << "feature done=1\n";
            continue;
        }
        if (input.compare("sd") == 0) {
            line >> input;
            depth = stoi(input);
            std::cout << "DEBUG depth: " << depth << std::endl;
            continue;
        }
        if (input.compare("st") == 0) {
            line >> input;
            moveTime = stoi(input);
            std::cout << "DEBUG movetime: " << moveTime << std::endl;
            continue;
        }
        if (input.compare("time") == 0) {
            line >> input;
            t = stoi(input);
            std::cout << "DEBUG time: " << t << std::endl;
        }
        if (input.compare("level") == 0) {
            sec = 0;
            moveTime = -1;
            line >> input;
            mps = stoi(input);
            line >> input;
            int found = input.find(':');
            if (found >= 0 && found < input.length()) {
                timeLeft = stoi(input.substr(0, found));
                sec = stoi(input.substr(found + 1));
            } else {
                timeLeft = stoi(input);
            }
            line >> input;
            increment = stoi(input);
            timeLeft *= 60000;
            timeLeft += sec * 1000;
            movesToGo[0] = movesToGo[1] = 30;
            if (mps != 0) {
                movesToGo[0] = movesToGo[1] = mps;
            }
            t = -1;
            std::cout << "DEBUG level timeLeft: " << timeLeft << " movesToGo: " << movesToGo[0] << " inc: " << increment << " mps: " << mps << std::endl;
            continue;
        }
        if (input.compare("ping") == 0) {
            line >> input;
            std::cout << "pong " << input << std::endl;
            continue;
        }
        if (input.compare("new") == 0) {
            engineSide == BLACK;
            parseFEN(START_FEN);
            depth = -1;
            t = -1;
            continue;
        }
        if (input.compare("setboard") == 0) {
            engineSide = BOTH;
            line >> input;
            parseFEN(input);
            continue;
        }
        if (input.compare("go") == 0) {
            engineSide = pos->side;
            continue;
        }
        if (input.compare("usermove") == 0) {
            movesToGo[pos->side]--;
            line >> input;
            move = parseMove(input);
            if (move == noMove) {
                continue;
            }
            makeMove(move);
            // engineSide = pos->side;
            pos->ply = 0;
            continue;
        }
    }
}

void chessBoard::consoleLoop() {
    std::cout << "Welcome to CHEST in Console Mode!\n";
    std::cout << "Type 'help' for commands.\n\n";

    info->mode = CONSOLEMODE;
    if (info->mode == CONSOLEMODE) {std::cout << "correct game mode: DEBUG\n";}
    info->post = true;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MAXDEPTH;
    int moveTime = 3000;
    int engineSide = BOTH;
    int move = noMove;

    engineSide = BLACK;
    parseFEN(START_FEN);
    printBoard();

    while (true) {

        if(pos->side == engineSide && !checkResult()) {  
			info->startTime = getTimeMs();
			info->depth = depth;
			
			if(moveTime != 0) {
				info->timeSet = true;
				info->stopTime = info->startTime + moveTime;
			} 	
			
			searchPosition();

		}	

        std::cout << "\nCHEST > ";
        std::string input;
        std::stringstream line;
        getline(std::cin, input);
        line << input;
        getline(line, input, ' ');

        if (input.compare("help") == 0) {
            std::cout << "Commands:\n";
			std::cout << "quit - quit game\n";
			std::cout << "force - computer will not think\n";
			std::cout << "print - show board\n";
			std::cout << "post - show thinking\n";
			std::cout << "nopost - do not show thinking\n";
			std::cout << "new - start new game\n";
			std::cout << "go - set computer thinking\n";
			std::cout << "depth x - set depth to x\n";
			std::cout << "time x - set thinking time to x seconds (depth still applies if set)\n";
			std::cout << "view - show current depth and movetime settings\n";
			std::cout << "** note ** - to reset time and depth, set to 0\n";
			std::cout << "enter moves using b7b8q notation\n\n\n";
			continue;
        }
        if (input.compare("quit") == 0) {
            info->quit = true;
            break;
        }
        if (input.compare("post") == 0) {
            info->post = true;
            std::cout << "Post on." << std::endl; 
            continue;
        }
        if (input.compare("print") == 0) {
            printBoard();
            continue;
        }
        if (input.compare("nopost") == 0) {
            info->post = false;
            std::cout << "Post off." << std::endl; 
            continue;
        }
        if (input.compare("force") == 0) {
            engineSide = BOTH;
            continue;
        }
        if (input.compare("view") == 0) {
            if (depth == MAXDEPTH) {
                std::cout << "Depth not set";
            } else {
                std::cout << "Depth: " << depth;
            }
            if (moveTime != 0) {
                std::cout << " moveTime: " << moveTime/1000 << std::endl;
            } else {
                std::cout << " moveTime not set\n";
            }
            continue;
        }
        if (input.compare("depth") == 0) {
            line >> input;
            depth = stoi(input);
            if (depth == 0) {
                depth = MAXDEPTH;
            }
            continue;
        }
        if (input.compare("time") == 0) {
            line >> input;
            moveTime = stoi(input);
            moveTime *= 1000;
            continue;
        }
        if (input.compare("new") == 0) {
            engineSide = BLACK;
            parseFEN(START_FEN);
            continue;
        }
        if (input.compare("go") == 0) {
            engineSide = pos->side;
            continue;
        }
        move = parseMove(input);
        if (move == noMove) {
            std::cout << "Command unknown: " << input << std::endl;
            continue;
        }
        makeMove(move);
        pos->ply = 0;
    }

}