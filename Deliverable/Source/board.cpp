

#include <iostream>
#include "chessBoard.hpp"
#include "bitboards.hpp"
#include <stdexcept>
#include <string>
#define NDEBUG 
#include <assert.h>

bool chessBoard::checkBoard() {
    int t_pieceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPiece[2] = {0, 0};
    int t_majorPiece[2] = {0, 0};
    int t_minorPiece[2] = {0, 0};
    int t_material[2] = {0, 0};

    int pcount;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};
    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    for (int t_piece = wP; t_piece <= bK; t_piece++) {
        for (int t_piece_num = 0; t_piece_num < pos->pieceNum[t_piece]; t_piece_num++) {
            int sq120 = pos->pieceList[t_piece][t_piece_num];
            assert (pos->pieces[sq120] == t_piece);
        }
    }  

    for (int sq64 = 0; sq64 < 64; sq64++) {
        int sq120 = SQ120(sq64);
        int t_piece = pos->pieces[sq120];
        t_pieceNum[t_piece]++;
        int color = PieceCol[t_piece];
        if (PieceBig[t_piece]) t_bigPiece[color]++;
        if (PieceMin[t_piece]) t_minorPiece[color]++;
        if (PieceMaj[t_piece]) t_majorPiece[color]++;
        t_material[color] += PieceVal[t_piece];
    }

    for (int t_piece = wP; t_piece <= bK; t_piece++) {
        assert (t_pieceNum[t_piece] == pos->pieceNum[t_piece]);
    }

    pcount = CNT(t_pawns[WHITE]);
    assert (pcount == pos->pieceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    assert (pcount == pos->pieceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    assert (pcount == pos->pieceNum[wP] + pos->pieceNum[bP]);

    while (t_pawns[WHITE]) {
        int sq64 = POP(&t_pawns[WHITE]);
        assert (pos->pieces[SQ120(sq64)] == wP);
    }
    while (t_pawns[BLACK]) {
        int sq64 = POP(&t_pawns[BLACK]);
        assert (pos->pieces[SQ120(sq64)] == bP);
    }
    while (t_pawns[BOTH]) {
        int sq64 = POP(&t_pawns[BOTH]);
        assert ((pos->pieces[SQ120(sq64)] == wP) || (pos->pieces[SQ120(sq64)] == bP));
    }

    assert ((t_material[WHITE] == pos->material[WHITE]) && (t_material[BLACK] == pos->material[BLACK]));
    assert ((t_majorPiece[WHITE] == pos->majorPieces[WHITE]) && (t_majorPiece[BLACK] == pos->majorPieces[BLACK]));
    assert ((t_minorPiece[WHITE] == pos->minorPieces[WHITE]) && (t_minorPiece[BLACK] == pos->minorPieces[BLACK]));
    assert ((t_bigPiece[WHITE] == pos->bigPieces[WHITE]) && (t_bigPiece[BLACK] == pos->bigPieces[BLACK]));

    assert (pos->side == WHITE || pos->side == BLACK);
    assert (generatePosKey(pos) == pos->positionKey);

    assert (pos->enPassant == NO_SQ || (ranks[pos->enPassant] == RANK_6 && pos->side == WHITE) 
        || (ranks[pos->enPassant] == RANK_3 && pos->side == BLACK));

    assert (pos->pieces[pos->kingSquare[WHITE]] == wK);

    ////////
    if (pos->pieces[pos->kingSquare[BLACK]] != bK) {
        std::cout << "Failed Board" << std::endl;
        std::cout << "KingSquare: " << printSquare(pos->kingSquare[BLACK]) << std::endl;
        std::cout << "Piece on King Square: " << std::dec << pos->pieces[pos->kingSquare[BLACK]] << std::endl;
        printBoard();
    }
    ////////
    assert (pos->pieces[pos->kingSquare[BLACK]] == bK);

    return true;
} 

void chessBoard::updateLists() {

    for (int index = 0; index < NUM_SQUARES; index++) {
        int sq = index;
        int piece = pos->pieces[index];
        if (piece != OFFBOARD && piece != EMPTY) {
            int color = PieceCol[piece];
            if (PieceBig[piece] == true) {
                pos->bigPieces[color]++;
            }
            if (PieceMin[piece] == true) {
                pos->minorPieces[color]++;
            }
            if (PieceMaj[piece] == true) {
                pos->majorPieces[color]++;
            }
            pos->material[color] += PieceVal[piece];

            // piece lists
            pos->pieceList[piece][pos->pieceNum[piece]] = sq;
            pos->pieceNum[piece]++;
            
            if (piece == wK) {
                pos->kingSquare[WHITE] = sq;
                SETBIT(pos->kings[WHITE], SQ64(sq));
                SETBIT(pos->kings[BOTH], SQ64(sq));
            }
            if (piece == bK) {
                pos->kingSquare[BLACK] = sq; 
                SETBIT(pos->kings[BLACK], SQ64(sq));
                SETBIT(pos->kings[BOTH], SQ64(sq));
            }
            if (piece == wP) {
                SETBIT(pos->pawns[WHITE], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            } else if (piece == bP) {
                SETBIT(pos->pawns[BLACK], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            } else if (piece == wN) {
                SETBIT(pos->knights[WHITE], SQ64(sq));
                SETBIT(pos->knights[BOTH], SQ64(sq));
            } else if (piece == bN) {
                SETBIT(pos->knights[BLACK], SQ64(sq));
                SETBIT(pos->knights[BOTH], SQ64(sq));
            } else if (piece == wB) {
                SETBIT(pos->bishops[WHITE], SQ64(sq));
                SETBIT(pos->bishops[BOTH], SQ64(sq));
            } else if (piece == bB) {
                SETBIT(pos->bishops[BLACK], SQ64(sq));
                SETBIT(pos->bishops[BOTH], SQ64(sq));
            } else if (piece == wR) {
                SETBIT(pos->rooks[WHITE], SQ64(sq));
                SETBIT(pos->rooks[BOTH], SQ64(sq));
            } else if (piece == bR) {
                SETBIT(pos->rooks[BLACK], SQ64(sq));
                SETBIT(pos->rooks[BOTH], SQ64(sq));
            } else if (piece == wQ) {
                SETBIT(pos->queens[WHITE], SQ64(sq));
                SETBIT(pos->queens[BOTH], SQ64(sq));
            } else if (piece == bQ) {
                SETBIT(pos->queens[BLACK], SQ64(sq));
                SETBIT(pos->queens[BOTH], SQ64(sq));
            }
        }
    }
}

int chessBoard::parseFEN(std::string fen) {

    if (pos == nullptr) {
        throw std::invalid_argument("Error in board.cpp: parseFEN. One or more arguments nullptr.");
    }

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int sq120 = 0;

    resetBoard();

    int p = fen.find(" ");
    std::string temp = fen.substr(0, p);

    for (char &c : temp) {
        if (c == 'p') {
            piece = bP;
        } else if (c == 'r') {
            piece = bR;
        } else if (c == 'n') {
            piece = bN;
        } else if (c == 'b') {
            piece = bB;
        } else if (c == 'q') {
            piece = bQ;
        } else if (c == 'k') {
            piece = bK;
        } else if (c == 'P') {
            piece = wP;
        } else if (c == 'R') {
            piece = wR;
        } else if (c == 'N') {
            piece = wN;
        } else if (c == 'B') {
            piece = wB;
        } else if (c == 'Q') {
            piece = wQ;
        } else if (c == 'K') {
            piece = wK;
        } else if (c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8') {
            file += c - '0';
            piece = EMPTY;
        } else {
            rank--;
            file = FILE_A;
            continue;
        }

        sq120 = SQ120(rank * 8 + file);
        if (piece != EMPTY && rank >= RANK_1 && file <= FILE_H) {
            pos->pieces[sq120] = piece;
            file++;
        }
    }

    temp = fen.substr(p + 1);
    pos->side = (temp[0] == 'w') ? WHITE : BLACK;
    temp = temp.substr(2);
    for (int i = 0; i < 4; i++) {
        if (temp[i] == 'K') {
            pos->castlePermission |= WKCASTLE;
        } else if (temp[i] == 'Q') {
            pos->castlePermission |= WQCASTLE;
        } else if (temp[i] == 'k') {
            pos->castlePermission |= BKCASTLE;
        } else if (temp[i] == 'q') {
            pos->castlePermission |= BQCASTLE;
        } else {
            break;
        }
    }
    p = temp.find(" ");
    temp = temp.substr(p + 1);
    if (temp[0] != '-') {
        file = temp[0] - 'a';
        rank = temp[1] - '1';
        pos->enPassant = FR2SQ(file, rank);
    }

    pos->positionKey = generatePosKey();

    updateLists();
}

void chessBoard::resetBoard() {

    for (int i = 0; i < NUM_SQUARES; i++) {
        pos->pieces[i] = OFFBOARD;
    }
    for (int i = 0; i < 64; i++) {
        pos->pieces[SQ120(i)] = EMPTY;
    }
    for (int i = 0; i < 3; i++) {
        pos->bigPieces[i] = 0;
        pos->majorPieces[i] = 0;
        pos->minorPieces[i] = 0;
        pos->material[i] = 0;
        pos->pawns[i] = 0ULL;
        pos->knights[i] = 0ULL;
        pos->bishops[i] = 0ULL;
        pos->rooks[i] = 0ULL;
        pos->queens[i] = 0ULL;
    }
    for (int i = 0; i < 13; i++) {
        pos->pieceNum[i] = 0;
    }

    pos->kingSquare[WHITE] = NO_SQ;
    pos->kingSquare[BLACK] = NO_SQ;
    pos->side = BOTH;
    pos->enPassant = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->plyHistory = 0;

    pos->castlePermission = 0;

    pos->positionKey = 0ULL;

}

void chessBoard::printBoard() {
    std::string pieceString = ".PNBRQKpnbrqk";
    std::string sideString = "wb-";
    for (int r = RANK_8; r >= RANK_1; r--) {
        std::cout << r + 1 << ": ";
        for (int f = 0; f <= FILE_H; f++) {
            std::cout << pieceString[pos->pieces[FR2SQ(f, r)]] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "   a b c d e f g h" << std::endl;
    std::cout << "Side: " << sideString[pos->side] << std::endl;
    std::cout << "EnPassant: " << pos->enPassant << std::endl;
    std::cout << "Castle: ";
    if (pos->castlePermission & WKCASTLE) {
        std::cout << "K";
    } else {
        std::cout << "-";
    }
    if (pos->castlePermission & WQCASTLE) {
        std::cout << "Q";
    } else {
        std::cout << "-";
    }
    if (pos->castlePermission & BKCASTLE) {
        std::cout << "k";
    } else {
        std::cout << "-";
    }
    if (pos->castlePermission & BQCASTLE) {
        std::cout << "q";
    } else {
        std::cout << "-";
    }
    std::cout << std::endl;
    std::cout << "Position Key: ";
    std::cout << std::hex << pos->positionKey << std::endl;
    std::cout << std::dec;

}

U64 chessBoard::generatePosKey() {
    U64 finalKey = 0;
    int piece = EMPTY;

    for (int i = 0; i < NUM_SQUARES; i++) {
        piece = pos->pieces[i];
        if (piece != NO_SQ && piece != EMPTY) {
            finalKey ^= PieceKeys[piece][i];
        }
    }
    if (pos->side == WHITE) {
        finalKey ^= SideKey;
    }
    if (pos->enPassant != NO_SQ) {
        finalKey ^= PieceKeys[EMPTY][pos->enPassant];
    }

    finalKey ^= CastleKeys[pos->castlePermission];

    return finalKey;
}

void chessBoard::mirrorBoard() {
    int tempPiecesArray[64];
    int tempSide = pos->side ^ 1;
    int swapPiece[13] = {EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK};
    int tempCastlePermission = 0;
    int tempEnPassant = NO_SQ;

    if (pos->castlePermission & WKCASTLE) {tempCastlePermission |= BKCASTLE;}
    if (pos->castlePermission & WQCASTLE) {tempCastlePermission |= BQCASTLE;}
    if (pos->castlePermission & BKCASTLE) {tempCastlePermission |= WKCASTLE;}
    if (pos->castlePermission & BQCASTLE) {tempCastlePermission |= WQCASTLE;}

    if (pos->enPassant != NO_SQ) {
        tempEnPassant = mirror64(SQ64(pos->enPassant));
    }
    for (int i = 0; i < 64; i++) {
        tempPiecesArray[i] = pos->pieces[SQ120(mirror64(i))];
    }

    resetBoard();

    for (int i = 0; i < 64; i++) {
        pos->pieces[SQ120(i)] = swapPiece[tempPiecesArray[i]];
    }

    pos->side = tempSide;
    pos->castlePermission = tempCastlePermission;
    pos->enPassant = tempEnPassant;

    pos->positionKey = generatePosKey();
    updateLists();

    assert (checkBoard(pos));

}