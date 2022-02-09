

#include <iostream>
#include <assert.h>
#include "chessBoard.hpp"
#include "bitboards.hpp"

int Sq120ToSq64[NUM_SQUARES];
int Sq64ToSQ120[64];
U64 SetMask[64];
U64 ClearMask[64];
U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

bool PieceBig[13] = {false, false, true, true, true, true, true, false, true, true, true, true, true};
bool PieceMaj[13] = {false, false, false, false, true, true, true, false, false, false, true, true, true};
bool PieceMin[13] = {false, false, true, true, false, false, false, false, true, true, false, false, false};
int PieceVal[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int PieceCol[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

int files[NUM_SQUARES];
int ranks[NUM_SQUARES];

U64 fileBBMask[8];
U64 rankBBMask[8];
U64 blackPassedMask[64];
U64 whitePassedMask[64];
U64 isolatedMask[64];

bool piecePawn[13] = {false, true, false, false, false, false, false, true, false, false, false, false, false};
bool pieceKnight[13] = {false, false, true, false, false, false, false, false, true, false, false, false, false};
bool pieceKing[13] = {false, false, false, false, false, false, true, false, false, false, false, false, true};
bool pieceRook[13] = {false, false, false, false, true, true, false, false, false, false, true, true, false};
bool pieceBishop[13] = {false, false, false, true, false, true, false, false, false, true, false, true, false};
bool pieceSlides[13] = {false, false, false, true, true, true, false, false, false, true, true, true, false};

const int Mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};

const int pvSize = 0x100000 * 1;

void initializeBitMasks();
void initializeSq120To64();
void initializeHashKeys();
void initializeFilesRanks();
void initializePawnEvaluationMasks();
void initializeMvvLva();
void initializePVTable(PVTable * table);

chessBoard::chessBoard() {
    initializeSq120To64();
    initializeBitMasks();
    initializeHashKeys();
    initializeFilesRanks();
    initializeMvvLva();
    initializePawnEvaluationMasks();
    info->quit = false;
    initializePVTable(pos->pvTable);
}

chessBoard::~chessBoard() {
    delete[] pos->pvTable->pvTable;
    delete[] pos;
    delete[] info;
}

void initializeSq120To64() {
    int sq = A1;
    int sq64 = 0;
    for (int i = 0; i < NUM_SQUARES; i++) {
        Sq120ToSq64[i] = 64;
    }
    for (int i = 0; i < 64; i++) {
        Sq64ToSQ120[i] = 120;
    }
    for (int rank = RANK_1; rank <= RANK_8; rank++) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            Sq64ToSQ120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

void initializeBitMasks() {
    
    for (int i = 0; i < 64; i++) {
        SetMask[i] = 0ULL;
        ClearMask[i] = 0ULL;

        SetMask[i] |= (1ULL << i);
        ClearMask[i] = ~SetMask[i];
    }

}

void initializeHashKeys() {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 120; j++) {
            PieceKeys[i][j] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (int i = 0; i < 16; i++) {
        CastleKeys[i] = RAND_64;
    }
}

void initializeFilesRanks() {
    int sq = A1;
    int sq64 = 0;
    for (int i = 0; i < NUM_SQUARES; i++) {
        files[i] = OFFBOARD;
        ranks[i] = OFFBOARD;     
    }
    for (int r = RANK_1; r <= RANK_8; r++) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            sq = FR2SQ(f, r);
            files[sq] = f;
            ranks[sq] = r;
        }
    }
}

void initializePawnEvaluationMasks() {
    for (int s = 0; s < 8; s++) {
        fileBBMask[s] = 0ULL;
        rankBBMask[s] = 0ULL;
    }
    for (int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            int sq = r * 8 + f;
            fileBBMask[f] |= (1ULL << sq);
            rankBBMask[r] |= (1ULL << sq);
        }
    }

    for (int s = 0; s < 64; s++) {
        whitePassedMask[s] = 0ULL;
        blackPassedMask[s] = 0ULL;
        for (int f = -1; f <= 1; f++) {
            int file = files[SQ120(s)];
            int rank = ranks[SQ120(s)];
            if (file + f >= FILE_A && file + f <= FILE_H) {
                for (int r = rank + 1; r <= RANK_8; r++) {
                    whitePassedMask[s] |= fileBBMask[file + f] & rankBBMask[r];
                }
                for (int r = rank - 1; r >= RANK_1; r--) {
                    blackPassedMask[s] |= fileBBMask[file + f] & rankBBMask[r];
                }   
            }
        }
        isolatedMask[s] = 0ULL;
        for (int f = -1; f <= 1; f += 2) {
            int file = files[SQ120(s)];
            int rank = ranks[SQ120(s)];
            if (file + f >= FILE_A && file + f <= FILE_H) {
                for (int r = RANK_1; r <= RANK_8; r++) {
                    isolatedMask[s] |= fileBBMask[file + f] & rankBBMask[r];
                }
            }
        }
    }
}

void initializeMvvLva() {
    for (int a = wP; a <= bK; a++) {
        for (int v = wP; v <= bK; v++) {
            MvvLvaScores[v][a] = victimScore[v] + 6 - (victimScore[a] / 100);
        }
    }
}

void initializePVTable(PVTable * table) {
    table->numEntries = pvSize / sizeof(PVEntry);
    table->numEntries -= 2;
    table->pvTable = new PVEntry[table->numEntries];
    PVEntry * pvEntry;
    for (pvEntry = table->pvTable; pvEntry < table->pvTable + table->numEntries; pvEntry++) {
        pvEntry->positionKey = 0ULL;
        pvEntry->move = noMove;
    }
    //std::cout << "PVTable initialization complete with " << table->numEntries << " entries." << std::endl;
}