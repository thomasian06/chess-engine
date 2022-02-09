
#include <iostream>
#include "chessBoard.hpp"
#define NDEBUG 
#include <assert.h>

#define hashPiece(piece, sq) (pos->positionKey ^= (PieceKeys[(piece)][(sq)]))
#define hashCastle (pos->positionKey ^= (CastleKeys[(pos->castlePermission)]))
#define hashSide (pos->positionKey ^= (SideKey))
#define hashEP (pos->positionKey ^= (PieceKeys[EMPTY][(pos->enPassant)]))

#define inCheck (sqAttacked(pos->kingSquare[pos->side], pos->side ^ 1, pos))

const int castlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void clearPiece(const int sq, Board * pos) {

    assert(sqOnBoard(sq));

    int piece = pos->pieces[sq];

    assert(pieceValid(piece));

    int color = PieceCol[piece];
    int t_pieceNum = -1;

    hashPiece(piece, sq);

    pos->pieces[sq] = EMPTY;
    pos->material[color] -= PieceVal[piece];
    
    if (PieceBig[piece]) {
        pos->bigPieces[color]--;
        if (PieceMaj[piece]) {
            pos->majorPieces[color]--;
        } else {
            pos->minorPieces[color]--;
        }
    } else {
        CLRBIT(pos->pawns[color], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }
    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        if (pos->pieceList[piece][i] == sq) {
            t_pieceNum = i;
            break;
        }
    }
    assert (t_pieceNum != -1);
    pos->pieceNum[piece]--;
    pos->pieceList[piece][t_pieceNum] = pos->pieceList[piece][pos->pieceNum[piece]];
}

static void addPiece(const int sq, Board * pos, const int piece) {
    assert (pieceValid(piece));
    assert (sqOnBoard(sq));
    int color = PieceCol[piece];
    hashPiece(piece, sq);
    pos->pieces[sq] = piece;
    if (PieceBig[piece]) {
        pos->bigPieces[color]++;
        if (PieceMaj[piece]) {
            pos->majorPieces[color]++;
        } else { // minor piece
            pos->minorPieces[color]++;
        }
    } else {
        SETBIT(pos->pawns[color], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }
    pos->material[color] += PieceVal[piece];
    pos->pieceList[piece][pos->pieceNum[piece]++] = sq;
}

static void movePiece(const int from, const int to, Board * pos) {
    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    int piece = pos->pieces[from];
    int color = PieceCol[piece];
    int t_pieceNum = true;
    #ifdef DEBUG
        t_pieceNum = false;
    #endif
    hashPiece(piece, from);
    pos->pieces[from] = EMPTY;

    hashPiece(piece, to);
    pos->pieces[to] = piece;

    if (!PieceBig[piece]) {
        CLRBIT(pos->pawns[color], SQ64(from));
        CLRBIT(pos->pawns[BOTH], SQ64(from));
        SETBIT(pos->pawns[color], SQ64(to));
        SETBIT(pos->pawns[BOTH], SQ64(to));
    }

    for (int i = 0; i < pos->pieceNum[piece]; i++) {
        if (pos->pieceList[piece][i] == from) {
            pos->pieceList[piece][i] = to;
            #ifdef DEBUG
                t_pieceNum = true;
            #endif
        }
    }
    assert (t_pieceNum);
}

bool chessBoard::makeMove(int move) {

    assert (checkBoard(pos));

    int from = fromSQ(move);
    int to = toSQ(move);
    int side = pos->side;

    assert (sqOnBoard(from));
    assert (sqOnBoard(to));
    assert (sideValid(side));
    assert (pieceValid(pos->pieces[from]));

    pos->history[pos->plyHistory].positionKey = pos->positionKey;

    if (move & mFlagEP) {
        if (side == WHITE) {
            clearPiece(to - 10, pos);
        } else {
            clearPiece(to + 10, pos);
        }
    } else if (move & mFlagCA) {
        if (to == C1) {
            movePiece(A1, D1, pos);
        } else if (to == G1) {
            movePiece(H1, F1, pos);
        } else if (to == C8) {
            movePiece(A8, D8, pos);
        } else if (to = G8) {
            movePiece(H8, F8, pos);
        } else {
            assert (false);
        }
    }

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }
    hashCastle;

    pos->history[pos->plyHistory].move = move;
    pos->history[pos->plyHistory].fiftyMove = pos->fiftyMove;
    pos->history[pos->plyHistory].enPassant = pos->enPassant;
    pos->history[pos->plyHistory].castlePermission = pos->castlePermission;

    pos->castlePermission &= castlePerm[from];
    pos->castlePermission &= castlePerm[to];
    pos->enPassant = NO_SQ;

    hashCastle;

    int captured = captured(move);
    pos->fiftyMove++;

    if (captured != EMPTY) {
        assert (pieceValid(captured));
        clearPiece(to, pos);
        pos->fiftyMove = 0;
    }

    pos->plyHistory++;
    pos->ply++;

    if (piecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if (move & mFlagPS) {
            if (side == WHITE) {
                pos->enPassant = from + 10;
                assert (ranks[pos->enPassant] == RANK_3);
            } else {
                pos->enPassant = from - 10;
                assert (ranks[pos->enPassant] == RANK_6);
            }
            hashEP;
        }
    }

    movePiece(from, to, pos);

    int prPiece = promoted(move);
    if (prPiece != EMPTY) {
        assert (pieceValid(prPiece) && !piecePawn[prPiece]);
        clearPiece(to, pos);
        addPiece(to, pos, prPiece);
    }

    if (pieceKing[pos->pieces[to]]) {
        pos->kingSquare[pos->side] = to;
    }

    pos->side ^= 1;
    hashSide;

    assert(checkBoard(pos));

    if (sqAttacked(pos->kingSquare[side], pos->side)) {
        takeMove();
        return false;
    }

    return true;

}

void chessBoard::takeMove() {
    
    assert (checkBoard(pos));

    pos->plyHistory--;
    pos->ply--;

    int move = pos->history[pos->plyHistory].move;
    int from = fromSQ(move);
    int to = toSQ(move);

    assert (sqOnBoard(from));
    assert (sqOnBoard(to));

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }
    hashCastle;

    pos->castlePermission = pos->history[pos->plyHistory].castlePermission;
    pos->fiftyMove = pos->history[pos->plyHistory].fiftyMove;
    pos->enPassant = pos->history[pos->plyHistory].enPassant;

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }
    hashCastle;

    pos->side ^= 1;
    hashSide;

    if (mFlagEP & move) {
        if (pos->side == WHITE) {
            addPiece(to - 10, pos, bP);
        } else {
            addPiece(to + 10, pos, wP);
        }
    } else if (mFlagCA & move) {
        if (to == C1) {
            movePiece(D1, A1, pos);
        } else if (to == G1) {
            movePiece(F1, H1, pos);
        } else if (to == C8) {
            movePiece(D8, A8, pos);
        } else if (to = G8) {
            movePiece(F8, H8, pos);
        } else {
            assert (false);
        }
    }

    movePiece(to, from, pos);

    if (pieceKing[pos->pieces[from]]) {
        pos->kingSquare[pos->side] = from;
    }

    int captured = captured(move);
    if (captured != EMPTY) {
        assert (pieceValid(captured));
        addPiece(to, pos, captured);
    }

    if (promoted(move) != EMPTY) {
        assert(pieceValid(promoted(move)) && !piecePawn[promoted(move)]);
        clearPiece(from, pos);
        addPiece(from, pos, ((PieceCol[promoted(move)] == WHITE) ? wP : bP));
    }

    assert(checkBoard(pos));

}

void chessBoard::makeNullMove() {
    assert (checkBoard(pos));
    assert (!incheck);

    pos->ply++;
    pos->history[pos->plyHistory].positionKey = pos->positionKey;

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }

    pos->history[pos->plyHistory].move = noMove;
    pos->history[pos->plyHistory].fiftyMove = pos->fiftyMove;
    pos->history[pos->plyHistory].enPassant = pos->enPassant;
    pos->history[pos->plyHistory].castlePermission = pos->castlePermission;
    pos->enPassant = NO_SQ;

    pos->side ^= 1;
    pos->plyHistory++;
    hashSide;
    assert (checkBoard(pos));

    return;
}

void chessBoard::takeNullMove() {
    assert (checkBoard(pos));

    pos->plyHistory--;
    pos->ply--;

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }

    pos->castlePermission = pos->history[pos->plyHistory].castlePermission;
    pos->fiftyMove = pos->history[pos->plyHistory].fiftyMove;
    pos->enPassant = pos->history[pos->plyHistory].enPassant;

    if (pos->enPassant != NO_SQ) {
        hashEP;
    }
    pos->side ^= 1;
    hashSide;

    assert (checkBoard(pos));

    return;
}