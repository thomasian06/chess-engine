

#include <iostream>
#include "chessBoard.hpp"
#include <assert.h>
#include <climits>
#include <iomanip>
#define NDEBUG 

#define MATE 29000
#define INF 30000

void chessBoard::checkUp() {
    // check if time's up, or interrupt from GUI
    if (info->timeSet && getTimeMs() > info->stopTime) {
        info->stopped = true;  
    }
    readInput();
}

static void pickNextMove(int n, MoveList * list) {
    int bestScore = 0;
    int b = n;
    for (int i = n; i < list->count; i++) {
        if (list->moves[i].score > bestScore) {
            bestScore = list->moves[i].score;
            b = i;
        }
    }
    Move temp = list->moves[n];
    list->moves[n] = list->moves[b]; 
    list->moves[b] = temp;
}

bool chessBoard::isRepetition() {
    for (int i = pos->plyHistory - pos->fiftyMove; i < pos->plyHistory - 1; i++) {
        assert (i >= 0 && index < MAX_GAME_MOVES);
        if (pos->positionKey == pos->history[i].positionKey) {
            return true;
        }
    }
    return false;
}

void chessBoard::clearForSearch() {
    
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < NUM_SQUARES; j++) {
            pos->searchHistory[i][j] = 0;
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < MAXDEPTH; j++) {
            pos->searchHistory[i][j] = 0;
        }
    }
    clearPVTable();
    pos->ply = 0;
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;

}

int chessBoard::quiescence(int alpha, int beta) {

    assert (checkBoard());

    if ((info->nodes & 2047) == 0) {
        checkUp();
    }

    info->nodes++;

    if (isRepetition() || pos->fiftyMove >= 100) {
        return 0;
    }
    if (pos->ply > MAXDEPTH - 1) {
        return evaluatePosition();
    }

    int score = evaluatePosition();

    if (score >= beta) {
        return beta;
    }

    if (score > alpha) {
        alpha = score;
    }

    MoveList list[1];
    generateAllCaptures(list);

    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = noMove;
    score = INT_MIN;
    int pvMove = probePVTable();

    for (int i = 0; i < list->count; i++) {

        pickNextMove(i, list);

        if (!makeMove(list->moves[i].move)) {
            continue;
        }
        
        legal++;
        score = -quiescence(-beta, -alpha);
        takeMove();

        if (info->stopped) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                if (legal == 1) {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
            alpha = score;
            bestMove = list->moves[i].move;
        }
    }

    if (alpha!= oldAlpha) {
        storePVMove(bestMove);
    }

    return alpha;
}

int chessBoard::alphaBeta(int alpha, int beta, int depth, bool doNull) {
    
    assert (checkBoard());

    if (depth == 0) {
        return quiescence(alpha, beta);
    }

    if ((info->nodes & 2047) == 0) {
        checkUp();
    }

    info->nodes++;

    if (isRepetition() || pos->fiftyMove >= 100) {
        return 0;
    }
    if (pos->ply > MAXDEPTH - 1) {
        return evaluatePosition();
    }

    bool check = sqAttacked(pos->kingSquare[pos->side], pos->side ^ 1);
    if (check) {
        depth++;
    }
    int score = INT_MIN;

    if (doNull && !check && pos->ply && (pos->bigPieces[pos->side] > 0) && depth >= 4) {
        makeNullMove();
        score = -alphaBeta(-beta, -beta + 1, depth - 4, false);
        takeNullMove();
        if (info->stopped) {
            return 0;
        }
        if (score >= beta) {
            return beta;
        }
    }

    MoveList list[1];
    generateAllMoves(list);

    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = noMove;
    score = INT_MIN;
    int pvMove = probePVTable();

    if (pvMove != noMove) {
        for (int i = 0; i < list->count; i++) {
            if (list->moves[i].move = pvMove) {
                list->moves[i].score = 2000000;
                break;
            }
        }
    }

    for (int i = 0; i < list->count; i++) {

        pickNextMove(i, list);

        if (!makeMove(list->moves[i].move)) {
            continue;
        }
        
        legal++;
        score = -alphaBeta(-beta, -alpha, depth - 1, true);
        takeMove();

        if (info->stopped) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {
                if (legal == 1) {
                    info->fhf++;
                }
                info->fh++;

                if (!(list->moves[i].move & mFlagCap)) {
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list->moves[i].move;
                }

                return beta;
            }
            alpha = score;
            bestMove = list->moves[i].move;
            if (!(list->moves[i].move & mFlagCap)) {
                pos->searchHistory[pos->pieces[fromSQ(bestMove)]][toSQ(bestMove)] += depth;
            }
        }
    }

    if (legal == 0) {
        if (check) {
            return -MATE + pos->ply;
        } else {
            return 0;
        }
    }

    if (alpha != oldAlpha) {
        storePVMove(bestMove);
    }

    return alpha;
}

void chessBoard::searchPosition() {
    
    int bestMove = noMove;
    int bestScore = -INF;
    int pvMoves = 0;
    int pvNum = 0;

    clearForSearch();

    // iterative deepening
    for (int i = 1; i <= info->depth; i++) {
        bestScore = alphaBeta(-INF, INF, i, true);

        if (info->stopped) {
            break;
        }

        pvMoves = getPVLine(i);
        bestMove = pos->pvArray[0];
        
        if (info->mode == UCIMODE) {
            std::cout << "info depth " << i << " score cp " << bestScore << " nodes " << info->nodes <<
                " time " << getTimeMs() - info->startTime;
        } else if (info->mode == XBOARDMODE && info->post) {
            std::cout << i << " " << bestScore << " " << (getTimeMs() - info->startTime)/10 <<
                " " << info->nodes;
        } else if (info->post) {
            std::cout << "score: " << bestScore << " depth: " << i << " nodes: " << info->nodes << 
                " time: " << getTimeMs() - info->startTime << " (ms)";
        }
        if (info->mode == UCIMODE || info->post) {
            pvMoves = getPVLine(i);
            std::cout << " pv";
            for (int j = 0; j < pvMoves; j++) {
                std::cout << " " << printMove(pos->pvArray[j]);
            }
            std::cout << "\n";
            //std::cout << "Ordering: " << (info->fhf / info->fh) << std::endl;
        }
    }
    if (info->mode == UCIMODE) {
        std::cout << "bestmove " << printMove(bestMove) << "\n";
    } else if (info->mode == XBOARDMODE) {
        std::cout << "move " << printMove(bestMove) << "\n";
        makeMove(bestMove);
    } else {
        std::cout << "\n\n***CHEST makes move: " << printMove(bestMove) << "***\n\n" << std::endl;
        makeMove(bestMove);
        printBoard();
    }
}