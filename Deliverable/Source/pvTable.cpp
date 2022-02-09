

#include <iostream>
#include "chessBoard.hpp"
#include <assert.h>

void chessBoard::clearPVTable() {
    PVTable * table = pos->pvTable;
    PVEntry * pvEntry;
    for (pvEntry = table->pvTable; pvEntry < table->pvTable + table->numEntries; pvEntry++) {
        pvEntry->positionKey = 0ULL;
        pvEntry->move = noMove;
    }
}

void chessBoard::storePVMove(const int move) {
    int i = pos->positionKey % pos->pvTable->numEntries;
    assert (index >= 0 && index <= pos->pvTable->numEntries - 1);
    pos->pvTable->pvTable[i].move = move;
    pos->pvTable->pvTable[i].positionKey = pos->positionKey;
}

int chessBoard::probePVTable() {
    int i = pos->positionKey % pos->pvTable->numEntries;
    assert (index >= 0 && index <= pos->pvTable->numEntries - 1);
    if (pos->pvTable->pvTable[i].positionKey == pos->positionKey) {
        return pos->pvTable->pvTable[i].move;
    } 
    return noMove;
}

int chessBoard::getPVLine(const int depth) {
    assert (depth < MAXDEPTH);
    int move = probePVTable();
    int count = 0;
    while (move != noMove && count < depth) {
        assert (count < MAXDEPTH);
        if (moveExists(move)) {
            makeMove(move);
            pos->pvArray[count++] = move;
        } else {
            break;
        }
        move = probePVTable();
    }
    while (pos->ply > 0) {
        takeMove();
    }
    return count;
}
