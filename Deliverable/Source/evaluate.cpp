

#include <iostream>
#include "chessBoard.hpp"
#include <assert.h>
#define NDEBUG 

#define endgameMaterial (1 * PieceVal[wR] + 2 * PieceVal[wN] + 2 * PieceVal[wP])

const int isolated = -10;
const int pawnPassed[8] = {0, 5, 10, 20, 35, 60, 100, 200};
const int rookOpenFile = 10;
const int rookSemiOpenFile = 5;
const int queenOpenFile = 5;
const int queenSemiOpenFile = 3;
const int bishopPair = 20;

const int pawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int pawnTableE[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,
15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,
30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,
45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,
60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,
70	,	70	,	70	,	70	,	70	,	70	,	70	,	70	
};

const int knightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int bishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int rookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

const int kingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	20	,	20	,	20	,	20	,	0	,	-10	,
	0	,	20	,	25	,	25	,	25	,	25	,	20	,	0	,
	0	,	20	,	25	,	30	,	30	,	25	,	20	,	0	,
	0	,	20	,	25	,	30	,	30	,	25	,	20	,	0	,
	0	,	20	,	25	,	25	,	25	,	25	,	20	,	0	,
	-10	,	0	,	20	,	20	,	20	,	20	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

const int kingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};

bool materialDraw (const Board * pos) {
    if (!pos->pieceNum[wR] && !pos->pieceNum[bR] && !pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
        if (!pos->pieceNum[bB] && !pos->pieceNum[wB]) {
            if (pos->pieceNum[wN] < 3 && pos->pieceNum[bN] < 3) {  return true; }
        } else if (!pos->pieceNum[wN] && !pos->pieceNum[bN]) {
            if (abs(pos->pieceNum[wB] - pos->pieceNum[bB]) < 2) { return true; }
        } else if ((pos->pieceNum[wN] < 3 && !pos->pieceNum[wB]) || (pos->pieceNum[wB] == 1 && !pos->pieceNum[wN])) {
            if ((pos->pieceNum[bN] < 3 && !pos->pieceNum[bB]) || (pos->pieceNum[bB] == 1 && !pos->pieceNum[bN]))  { return true; }
        }
	} else if (!pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
        if (pos->pieceNum[wR] == 1 && pos->pieceNum[bR] == 1) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB]) < 2 && (pos->pieceNum[bN] + pos->pieceNum[bB]) < 2)	{ return true; }
        } else if (pos->pieceNum[wR] == 1 && !pos->pieceNum[bR]) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB] == 0) && (((pos->pieceNum[bN] + pos->pieceNum[bB]) == 1) || ((pos->pieceNum[bN] + pos->pieceNum[bB]) == 2))) { return true; }
        } else if (pos->pieceNum[bR] == 1 && !pos->pieceNum[wR]) {
            if ((pos->pieceNum[bN] + pos->pieceNum[bB] == 0) && (((pos->pieceNum[wN] + pos->pieceNum[wB]) == 1) || ((pos->pieceNum[wN] + pos->pieceNum[wB]) == 2))) { return true; }
        }
    }
    return true;
}

int chessBoard::evaluatePosition() {
    
    int piece;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    if (!pos->pieceNum[wP] && !pos->pieceNum[bP] && materialDraw(pos)) {
        return 0;
    }

    piece = wP;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        if (pos->material[BLACK] <= endgameMaterial) {
            score += pawnTableE[SQ64(sq)];
        } else {
            score += pawnTable[SQ64(sq)];
        }

        if ((isolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            score += isolated;
        }

        if ((whitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            if (pos->material[BLACK] <= endgameMaterial) {
                score += 2 * pawnPassed[ranks[sq]];
            } else {
                score += pawnPassed[ranks[sq]];
            }
        }

    }

    piece = bP;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        if (pos->material[WHITE] <= endgameMaterial) {
            score -= pawnTableE[mirror64(SQ64(sq))];
        } else {
            score -= pawnTable[mirror64(SQ64(sq))];
        }

        if ((isolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            score -= isolated;
        }

        if ((whitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            if (pos->material[BLACK] <= endgameMaterial) {
                score -= 2 * pawnPassed[7 - ranks[sq]];
            } else {
                score -= pawnPassed[7 - ranks[sq]];
            }
        }

    }

    piece = wN;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score += knightTable[SQ64(sq)];
    }

    piece = bN;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score -= knightTable[mirror64(SQ64(sq))];
    }

    piece = wB;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score += bishopTable[SQ64(sq)];
    }

    piece = bB;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score -= bishopTable[mirror64(SQ64(sq))];
    }

    piece = wR;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score += rookTable[SQ64(sq)];
        if (!(pos->pawns[BOTH] & fileBBMask[files[sq]])) {
            score += rookOpenFile;
        } else if (!(pos->pawns[WHITE] & fileBBMask[files[sq]])) {
            score += rookSemiOpenFile;
        }
    }

    piece = bR;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        score -= rookTable[mirror64(SQ64(sq))];
        if (!(pos->pawns[BOTH] & fileBBMask[files[sq]])) {
            score -= rookOpenFile;
        } else if (!(pos->pawns[BLACK] & fileBBMask[files[sq]])) {
            score -= rookSemiOpenFile;
        }
    }

    piece = wQ;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        if (!(pos->pawns[BOTH] & fileBBMask[files[sq]])) {
            score += queenOpenFile;
        } else if (!(pos->pawns[WHITE] & fileBBMask[files[sq]])) {
            score += queenSemiOpenFile;
        }
    }

    piece = bQ;
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        int sq = pos->pieceList[piece][i];
        assert (sqOnBoard(sq));
        if (!(pos->pawns[BOTH] & fileBBMask[files[sq]])) {
            score -= queenOpenFile;
        } else if (!(pos->pawns[BLACK] & fileBBMask[files[sq]])) {
            score -= queenSemiOpenFile;
        }
    }

    piece = wK;
    sq = pos->pieceList[piece][0];
    if (pos->material[BLACK] <= endgameMaterial) {
        score += kingE[SQ64(sq)];
    } else {
        score += kingO[SQ64(sq)];
    }

    piece = bK;
    sq = pos->pieceList[piece][0];
    if (pos->material[WHITE] <= endgameMaterial) {
        score -= kingE[mirror64(SQ64(sq))];
    } else {
        score -= kingO[mirror64(SQ64(sq))];
    }

    if (pos->pieceNum[wB] >= 2) {
        score += bishopPair;
    }
    if (pos->pieceNum[bB] >= 2) {
        score += bishopPair;
    }

    if (pos->side == WHITE) {
        return score;
    } else if (pos->side == BLACK) {
        return -score;
    } else {
        return 0;
    }

}