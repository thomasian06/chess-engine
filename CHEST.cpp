

#include <iostream>
#include "chessBoard.hpp"
#include "bitboards.hpp"
#include <assert.h>
#include <string>

#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1"
#define BISHOPS "6k1/1b6/4n4/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"
#define CASTLE2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FENTEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"
#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
#define WAC3 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main() {

    std::cout << "Starting CHEST v1.1 ..." << std::endl;
    
    chessBoard cBoard;

    std::cout << std::dec;

    std::cout << "Welcome to CHEST! Type 'console' for console mode.\n\n";
    std::string input;
    while (true) {
        getline(std::cin, input);
        if (input.compare("uci") == 0) {
            cBoard.uciLoop();
        } else if (input.compare("xboard") == 0) {
            cBoard.xboardLoop();
        } else if (input.compare("console") == 0) {
            cBoard.consoleLoop();
        } else if (input.compare("quit") == 0) {
            break;
        }
    }

    cBoard.~chessBoard();

    return 0;
}