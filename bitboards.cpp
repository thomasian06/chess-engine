

#include <iostream>
#include "bitboards.hpp"

int popBit(U64 * bb) {
    U64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return bitTable[(fold * 0x783a9b23) >> 26];
}

int countBits(U64 bb) {
    int count;
    for (count = 0; bb; count++, bb &= bb - 1);
    return count;
}

void printBitboard(U64 bb) {
    U64 shift = 1ULL;
    printf("\n");
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ(file, rank);
            int sq64 = SQ64(sq);
            if ((shift << sq64) & bb) {
                printf("X ");
            }
            else {
                printf("- ");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}