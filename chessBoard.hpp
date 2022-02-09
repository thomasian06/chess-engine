

#include <iostream>
#include <cstdlib>
#include <string>

#define NDEBUG 

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

typedef unsigned long long U64;
#define NAME "IanChess 1.0"
#define NUM_SQUARES 120
#define MAX_GAME_MOVES 2048
#define MAX_POSITION_MOVES 256
#define MAXDEPTH 64
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    (U64)rand() & (U64)0xf << 60 )

enum {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, OFFBOARD = -1};
enum {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};
enum {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};
enum {WHITE, BLACK, BOTH};
enum {UCIMODE, XBOARDMODE, CONSOLEMODE};
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};
enum {WKCASTLE = 1, WQCASTLE = 2, BKCASTLE = 4, BQCASTLE = 8};

struct Move {
    int move;
    int score;
};

struct MoveList {
    Move moves[MAX_GAME_MOVES];
    int count;
};

struct PVEntry {
    U64 positionKey;
    int move;
};

struct PVTable {
    PVEntry * pvTable;
    int numEntries;
};

struct Undo {
    int move;
    int castlePermission;
    int enPassant;
    int fiftyMove;
    U64 positionKey;
};

struct Board {
    int pieces[NUM_SQUARES]; // array of all the pieces
    U64 pawns[3]; // array of all the pawns
    U64 knights[3];
    U64 bishops[3];
    U64 rooks[3];
    U64 queens[3];
    U64 kings[3];
    int kingSquare[2]; // holds the position of the black and white kings
    int side; // holds the current side to move
    int enPassant; // holds the current en passant square if there is one
    int fiftyMove; // holds a counter for the 50 move limit
    int ply; // stores the ply
    int plyHistory; // determining repetitions
    int castlePermission;
    U64 positionKey; // unicode key to store position
    int pieceNum[13]; // holds the number of pieces left on the board
    int bigPieces[2]; // holds number of anything that isnt a pawn
    int majorPieces[2]; // rooks and queens
    int minorPieces[2]; // bishops and knights
    int material[2];
    Undo history[MAX_GAME_MOVES];
    int pieceList[13][10]; // stores a list of all the pieces and the number of each
    PVTable pvTable[1];
    int pvArray[MAXDEPTH];
    int searchHistory[13][NUM_SQUARES];
    int searchKillers[2][MAXDEPTH];
};

struct SearchInfo {
    int startTime;
    int stopTime;
    int depth;
    bool depthSet;
    bool timeSet;
    int movesToGo;
    bool infinite;

    long nodes;

    bool quit;
    bool stopped;

    float fh;
    float fhf;

    int mode;
    bool post;
};

/* Game move */
#define fromSQ(m) ((m) & 0x7F)
#define toSQ(m) (((m)>>7) & 0x7F)
#define captured(m) (((m)>>14) & 0xF)
#define promoted(m) (((m)>>20) & 0xF)

#define mFlagEP 0x40000
#define mFlagPS 0x80000
#define mFlagCA 0x1000000
#define mFlagCap 0x7C000
#define mFlagProm 0xF00000

#define noMove 0

/* Macros */
#define FR2SQ(f, r) ((21 + f) + (r * 10))
#define SQ64(sq120) Sq120ToSq64[sq120]
#define SQ120(sq64) Sq64ToSQ120[sq64]

#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

#define isRQ(p) pieceRook[(p)]
#define isBQ(p) pieceBishop[(p)]
#define isKn(p) pieceKnight[(p)]
#define isKi(p) pieceKing[(p)]

#define mirror64(sq) (Mirror64[(sq)])

/* GLOBALS */
extern int Sq120ToSq64[NUM_SQUARES];
extern int Sq64ToSQ120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];

extern bool PieceBig[13];
extern bool PieceMaj[13];
extern bool PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

extern int files[NUM_SQUARES];
extern int ranks[NUM_SQUARES];

extern U64 fileBBMask[8];
extern U64 rankBBMask[8];
extern U64 blackPassedMask[64];
extern U64 whitePassedMask[64];
extern U64 isolatedMask[64];

extern bool piecePawn[13];
extern bool pieceKnight[13];
extern bool pieceKing[13];
extern bool pieceRook[13];
extern bool pieceBishop[13];
extern bool pieceSlides[13];

extern const int Mirror64[64];

//io.cpp
extern std::string printMove(const int move);
extern std::string printSquare(const int square);
extern void printMoveList(const MoveList * list);

//validate.cpp
extern bool sqOnBoard(const int sq);
extern bool sideValid(const int side);
extern bool fileRankValid(const int sq);
extern bool pieceValidEmpty(const int piece);
extern bool pieceValid(const int piece);

extern const int victimScore[13];
extern int MvvLvaScores[13][13]; 

//misc.cpp
extern long getTimeMs();

class chessBoard {
    public:
        chessBoard();
        ~chessBoard();

        // xboard.cpp
        void xboardLoop();
        void consoleLoop();

        // uci.cpp
        void uciLoop();

    private:
        // member variables
        Board pos[1];
        SearchInfo info[1];

        // functions

        // evaluate.cpp
        int evaluatePosition();

        // search.cpp
        bool isRepetition();
        void searchPosition();
        int quiescence(int alpha, int beta);
        int alphaBeta(int alpha, int beta, int depth, bool doNull);
        void checkUp();
        void clearForSearch();

        // xboard.cpp
        bool checkResult();

        // uci.cpp
        void parseGo(std::string inLine);
        void parsePosition(std::string inLine);

        // moveGen.cpp
        void addQuietMove(int move, MoveList * list);
        void addCaptureMove(int move, MoveList * list);
        void addEnPassantMove(int move, MoveList * list);
        void addWhitePawnCaptureMove(const int from, const int to, const int cap, MoveList * list);
        void addBlackPawnCaptureMove(const int from, const int to, const int cap, MoveList * list);
        void addWhitePawnMove(const int from, const int to, MoveList * list);
        void addBlackPawnMove(const int from, const int to, MoveList * list);
        void generateAllMoves(MoveList * list);
        void generateAllCaptures(MoveList * list);
        bool moveExists(const int move);

        // pvTable.cpp
        void clearPVTable();
        void storePVMove(const int);
        int probePVTable();
        int getPVLine(const int);

        // io.cpp
        void readInput();
        int parseMove(std::string str);

        // makeMove.cpp
        bool makeMove(int move);
        void takeMove();
        void makeNullMove();
        void takeNullMove();

        // attack.cpp
        bool sqAttacked(const int sq, const int side);

        // board.cpp
        int parseFEN(std::string fen);
        void resetBoard();
        void printBoard();
        void updateLists();
        bool checkBoard();
        void mirrorBoard();
        U64 generatePosKey();
};

#endif