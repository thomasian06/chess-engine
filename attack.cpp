

#include "chessBoard.hpp"
#include <assert.h>

const int knightDirection[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int rookDirection[4] = {-1, -10, 1, 10};
const int bishopDirection[4] = {-9, -11, 11, 9};
const int kingDirection[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

bool chessBoard::sqAttacked(const int sq, const int side) {

    assert (sqOnBoard(sq));
    assert (sideValid(side));
    assert (checkBoard(pos));

    // check pawns
    if (side == WHITE) {
        if (pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
            return true;
        }
    } else {
        if (pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
            return true;
        }
    }
    // check knights
    for (int i = 0; i < 8; i++) {
        int piece = pos->pieces[sq + knightDirection[i]];
        if (piece != OFFBOARD && isKn(piece) && PieceCol[piece] == side) {
            return true;
        }
    }
    // check rooks and queens
    for (int i = 0; i < 4; i++) {
        int dir = rookDirection[i];
        int tempSq = sq + dir;
        int piece = pos->pieces[tempSq];
        while (piece != OFFBOARD) {
            if (piece != EMPTY) {
                if (isRQ(piece) && PieceCol[piece] == side) {
                    return true;
                }
                break;
            }
            tempSq += dir;
            piece = pos->pieces[tempSq];
        }
    }
    // check bishops and queens
    for (int i = 0; i < 4; i++) {
        int dir = bishopDirection[i];
        int tempSq = sq + dir;
        int piece = pos->pieces[tempSq];
        while (piece != OFFBOARD) {
            if (piece != EMPTY) {
                if (isBQ(piece) && PieceCol[piece] == side) {
                    return true;
                }
                break;
            }
            tempSq += dir;
            piece = pos->pieces[tempSq];
        }
    }
    // check kings
    for (int i = 0; i < 8; i++) {
        int piece = pos->pieces[sq + kingDirection[i]];
        if (piece != OFFBOARD && isKi(piece) && PieceCol[piece] == side) {
            return true;
        }
    }

    return false;

}