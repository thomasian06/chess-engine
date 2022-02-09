

#include "chessBoard.hpp"

bool sqOnBoard(const int sq) {
    return (files[sq] == OFFBOARD) ? 0:1;
}

bool sideValid(const int side) {
    return (side == WHITE || side == BLACK) ? 1:0;
}

bool fileRankValid(const int fr) {
    return (fr >= 0 && fr < 8) ? 1:0;
}

bool pieceValidEmpty(const int piece) {
    return (piece >= EMPTY && piece <= bK) ? 1:0;
}

bool pieceValid(const int piece) {
    return (piece >= wP && piece <= bK) ? 1:0;
}