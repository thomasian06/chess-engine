

#include <iostream>
#include "chessBoard.hpp"
#define NDEBUG 
#include <assert.h>
#include <string>

#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define sqOffBoard(sq) (files[(sq)] == OFFBOARD)

int loopSlidePiece[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
int loopSlideIndex[2] = {0, 4};

int loopNonSlidePiece[6] = {wN, wK, 0, bN, bK, 0};
int loopNonSlideIndex[2] = {0, 3};

int pieceDirection[13][8] = {
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9, 0, 0, 0, 0},
	{-1, -10, 1, 10, 0, 0, 0, 0},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{0, 0, 0, 0, 0, 0, 0 },
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9, 0, 0, 0, 0},
	{-1, -10, 1, 10, 0, 0, 0, 0},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{-1, -10, 1, 10, -9, -11, 11, 9}
};

int numDirection[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

const int victimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
int MvvLvaScores[13][13]; 

bool chessBoard::moveExists(const int move) {
    MoveList list[1];
    generateAllMoves(list);

    for (int i = 0; i < list->count; i++) {
        if (!makeMove(list->moves[i].move)) {
            continue;
        }
        takeMove();
        if (list->moves[i].move == move) {
            return true;
        }
    }
    return false;
}

void chessBoard::addQuietMove(int move, MoveList * list) {

    assert (sqOnBoard(fromSQ(move)));
    assert (sqOnBoard(toSQ(move)));

    if (pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    } else if (pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = 
            pos->searchHistory[pos->pieces[fromSQ(move)]][toSQ(move)];
    }

    list->moves[list->count].move = move;
    list->count++;
}

void chessBoard::addCaptureMove(int move, MoveList * list) {
    assert (sqOnBoard(fromSQ(move)));
    assert (sqOnBoard(toSQ(move)));
    assert (pieceValid(captured(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[captured(move)][pos->pieces[fromSQ(move)]] + 1000000;
    list->count++;
}

void chessBoard::addEnPassantMove(int move, MoveList * list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}

void chessBoard::addWhitePawnCaptureMove(const int from, const int to, const int cap, MoveList * list) {
    assert (pieceValidEmpty(cap));
    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    if (ranks[from] == RANK_7) {
        addCaptureMove(MOVE(from, to, cap, wQ, 0), list);
        addCaptureMove(MOVE(from, to, cap, wR, 0), list);
        addCaptureMove(MOVE(from, to, cap, wN, 0), list);
        addCaptureMove(MOVE(from, to, cap, wB, 0), list);
    } else {
        addCaptureMove(MOVE(from, to, cap, EMPTY, 0), list);
    }
}

void chessBoard::addBlackPawnCaptureMove(const int from, const int to, const int cap, MoveList * list) {
    assert (pieceValidEmpty(cap));
    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    if (ranks[from] == RANK_2) {
        addCaptureMove(MOVE(from, to, cap, bQ, 0), list);
        addCaptureMove(MOVE(from, to, cap, bR, 0), list);
        addCaptureMove(MOVE(from, to, cap, bN, 0), list);
        addCaptureMove(MOVE(from, to, cap, bB, 0), list);
    } else {
        addCaptureMove(MOVE(from, to, cap, EMPTY, 0), list);
    }
}

void chessBoard::addWhitePawnMove(const int from, const int to, MoveList * list) {
    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    if (ranks[from] == RANK_7) {
        addQuietMove(MOVE(from, to, EMPTY, wQ, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, wR, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, wN, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, wB, 0), list);
    } else {
        addQuietMove(MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void chessBoard::addBlackPawnMove(const int from, const int to, MoveList * list) {
    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    if (ranks[from] == RANK_2) {
        addQuietMove(MOVE(from, to, EMPTY, bQ, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, bR, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, bN, 0), list);
        addQuietMove(MOVE(from, to, EMPTY, bB, 0), list);
    } else {
        addQuietMove(MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void chessBoard::generateAllMoves(MoveList * list) {

    assert (checkBoard());

    list->count = 0;
    int piece = EMPTY;
    int side = pos->side;
    // int dir = 0;
    // int index = 0;
    int pieceIndex = 0;

    if (side == WHITE) { // white pawns/castling
        for (int pieceNum = 0; pieceNum < pos->pieceNum[wP]; pieceNum++) {
            int sq = pos->pieceList[wP][pieceNum];
            assert (sqOnBoard(sq));
            // move forward
            if (pos->pieces[sq + 10] == EMPTY) {
                addWhitePawnMove(sq, sq + 10, list);
                if (ranks[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    addQuietMove(MOVE(sq, sq + 20, EMPTY, EMPTY, mFlagPS), list);
                }
            }
            // diagonal capture
            if (!sqOffBoard(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                addWhitePawnCaptureMove(sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if (!sqOffBoard(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                addWhitePawnCaptureMove(sq, sq + 11, pos->pieces[sq + 11], list);
            }
            // enPassant
            if (pos->enPassant != NO_SQ) {
                if (sq + 9 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq + 9, EMPTY, EMPTY, mFlagEP), list);
                }
                if (sq + 11 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq + 11, EMPTY, EMPTY, mFlagEP), list);
                }
            }
        }
        // castling
        if (pos->castlePermission & WKCASTLE) {
            if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if (!sqAttacked(E1, BLACK) && !sqAttacked(F1, BLACK)) {
                    addQuietMove(MOVE(E1, G1, EMPTY, EMPTY, mFlagCA), list);
                }
            }
        }
        if (pos->castlePermission & WQCASTLE) {
            if (pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if (!sqAttacked(E1, BLACK) && !sqAttacked(D1, BLACK)) {
                    addQuietMove(MOVE(E1, C1, EMPTY, EMPTY, mFlagCA), list);
                }
            }
        }
    } else { // black pawns/castling
        for (int pieceNum = 0; pieceNum < pos->pieceNum[bP]; pieceNum++) {
            int sq = pos->pieceList[bP][pieceNum];
            assert (sqOnBoard(sq));
            // move forward
            if (pos->pieces[sq - 10] == EMPTY) {
                addBlackPawnMove(sq, sq - 10, list);
                if (ranks[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
                    addQuietMove(MOVE(sq, sq - 20, EMPTY, EMPTY, mFlagPS), list);
                }
            }
            // diagonal capture
            if (!sqOffBoard(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
                addBlackPawnCaptureMove(sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if (!sqOffBoard(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
                addBlackPawnCaptureMove(sq, sq - 11, pos->pieces[sq - 11], list);
            }
            // enPassant
            if (pos->enPassant != NO_SQ) {
                if (sq - 9 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq - 9, EMPTY, EMPTY, mFlagEP), list);
                }
                if (sq - 11 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq - 11, EMPTY, EMPTY, mFlagEP), list);
                }
            }
        }
        // castling
        if (pos->castlePermission & BKCASTLE) {
            if (pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if (!sqAttacked(E8, WHITE) && !sqAttacked(F8, WHITE)) {
                    addQuietMove(MOVE(E8, G8, EMPTY, EMPTY, mFlagCA), list);
                }
            }
        }
        if (pos->castlePermission & BQCASTLE) {
            if (pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if (!sqAttacked(E8, WHITE) && !sqAttacked(D8, WHITE)) {
                    addQuietMove(MOVE(E8, C8, EMPTY, EMPTY, mFlagCA), list);
                }
            }
        }
    }

    pieceIndex = loopSlideIndex[side];
    piece = loopSlidePiece[pieceIndex++];
    while (piece != 0) {
        assert (pieceValid(piece));
        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; pieceNum++) {
            int sq = pos->pieceList[piece][pieceNum];
            assert (sqOnBoard(sq));
            for (int i = 0; i < numDirection[piece]; i++) {
                int dir = pieceDirection[piece][i];
                int t_sq = sq + dir;
                while (!sqOffBoard(t_sq)) {
                    if (pos->pieces[t_sq] != EMPTY) {
                        if (PieceCol[pos->pieces[t_sq]] == side ^ 1) {
                            addCaptureMove(MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }
        piece = loopSlidePiece[pieceIndex++];
    }

    pieceIndex = loopNonSlideIndex[side];
    piece = loopNonSlidePiece[pieceIndex++];
    while (piece != 0) {
        assert (pieceValid(piece));
        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; pieceNum++) {
            int sq = pos->pieceList[piece][pieceNum];
            assert (sqOnBoard(sq));
            for (int i = 0; i < numDirection[piece]; i++) {
                int dir = pieceDirection[piece][i];
                int t_sq = sq + dir;
                if (sqOffBoard(t_sq)) {
                    continue;
                }
                if (pos->pieces[t_sq] != EMPTY) {
                    if (PieceCol[pos->pieces[t_sq]] == side ^ 1) {
                        addCaptureMove(MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
                addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
            }
        }
        piece = loopNonSlidePiece[pieceIndex++];
    }

}

void chessBoard::generateAllCaptures(MoveList * list) {

    assert (checkBoard());

    list->count = 0;
    int piece = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int dir = 0;
    int index = 0;
    int pieceIndex = 0;

    if (side == WHITE) { // white pawns/castling
        for (int pieceNum = 0; pieceNum < pos->pieceNum[wP]; pieceNum++) {
            int sq = pos->pieceList[wP][pieceNum];
            assert (sqOnBoard(sq));
            // diagonal capture
            if (!sqOffBoard(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                addWhitePawnCaptureMove(sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if (!sqOffBoard(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                addWhitePawnCaptureMove(sq, sq + 11, pos->pieces[sq + 11], list);
            }
            // enPassant
            if (pos->enPassant != NO_SQ) {
                if (sq + 9 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq + 9, EMPTY, EMPTY, mFlagEP), list);
                }
                if (sq + 11 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq + 11, EMPTY, EMPTY, mFlagEP), list);
                }
            }
        }
    } else { // black pawns/castling
        for (int pieceNum = 0; pieceNum < pos->pieceNum[bP]; pieceNum++) {
            int sq = pos->pieceList[bP][pieceNum];
            assert (sqOnBoard(sq));
            
            // diagonal capture
            if (!sqOffBoard(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
                addBlackPawnCaptureMove(sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if (!sqOffBoard(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
                addBlackPawnCaptureMove(sq, sq - 11, pos->pieces[sq - 11], list);
            }
            // enPassant
            if (pos->enPassant != NO_SQ) {
                if (sq - 9 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq - 9, EMPTY, EMPTY, mFlagEP), list);
                }
                if (sq - 11 == pos->enPassant) {
                    addEnPassantMove(MOVE(sq, sq - 11, EMPTY, EMPTY, mFlagEP), list);
                }
            }
        }
    }

    pieceIndex = loopSlideIndex[side];
    piece = loopSlidePiece[pieceIndex++];
    while (piece != 0) {
        assert (pieceValid(piece));
        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; pieceNum++) {
            int sq = pos->pieceList[piece][pieceNum];
            assert (sqOnBoard(sq));
            for (int i = 0; i < numDirection[piece]; i++) {
                int dir = pieceDirection[piece][i];
                int t_sq = sq + dir;
                while (!sqOffBoard(t_sq)) {
                    if (pos->pieces[t_sq] != EMPTY) {
                        if (PieceCol[pos->pieces[t_sq]] == side ^ 1) {
                            addCaptureMove(MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    t_sq += dir;
                }
            }
        }
        piece = loopSlidePiece[pieceIndex++];
    }

    pieceIndex = loopNonSlideIndex[side];
    piece = loopNonSlidePiece[pieceIndex++];
    while (piece != 0) {
        assert (pieceValid(piece));
        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; pieceNum++) {
            int sq = pos->pieceList[piece][pieceNum];
            assert (sqOnBoard(sq));
            for (int i = 0; i < numDirection[piece]; i++) {
                int dir = pieceDirection[piece][i];
                int t_sq = sq + dir;
                if (sqOffBoard(t_sq)) {
                    continue;
                }
                if (pos->pieces[t_sq] != EMPTY) {
                    if (PieceCol[pos->pieces[t_sq]] == side ^ 1) {
                        addCaptureMove(MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
            }
        }
        piece = loopNonSlidePiece[pieceIndex++];
    }

}