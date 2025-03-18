// MultiplayerChess.cpp : Defines the entry point for the application.
//
#include <iostream>
#include "ChessObjects.h"


void test_001() {
    Pawn p(Pawn::Color::WHITE);
    std::cout << "Piece color: " << (p.getColor() == Piece::Color::WHITE ? "White" : "Black") << std::endl;
    Position pos = p.getPos();
    std::cout << "Position: (" << pos.row << ", " << pos.col << ")" << std::endl;

    p.setPos(Position(3, 4));
    pos = p.getPos();
    std::cout << "New Position: (" << pos.row << ", " << pos.col << ")" << std::endl;
}

void test_002() {
    Game chessGame;
    chessGame.m_board.printBoard();
    Position from = Position(6, 0);
    Position to = Position(5, 0);
    chessGame.makeMove(from, to);
    std::cout << "" << std::endl;
    chessGame.m_board.printBoard();

    Position from_1 = Position(5, 0);
    Position to_1 = Position(4, 0);
    chessGame.makeMove(from_1, to_1);
    std::cout << "" << std::endl;
    chessGame.m_board.printBoard();

    Position from_2 = Position(1, 0);
    Position to_2 = Position(2, 0);
    chessGame.makeMove(from_2, to_2);
    std::cout << "" << std::endl;
    chessGame.m_board.printBoard();


    Piece* piece = chessGame.m_board.getPiece(Position(5, 0));
    if (piece) {
        Position pieceLoc = piece->getPos();
        std::cout << pieceLoc.row << "," << pieceLoc.col << std::endl;
        std::cout << piece->hasMoved() << std::endl;
    }
    else {
        std::cout << "null piece" << std::endl;
    }


    Piece* piece2 = chessGame.m_board.getPiece(Position(0, 0));

    if (piece2) {
        Position pieceLoc2 = piece2->getPos();
        std::cout << pieceLoc2.row << "," << pieceLoc2.col << std::endl;
        std::cout << piece2->hasMoved() << std::endl;
    }
    else {
        std::cout << "null piece" << std::endl;
    }
   
}

void test_003() {
    Board chessBoard(8, 8);

    std::vector<std::vector<Piece*>> state = chessBoard.getState();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (state[i][j]) {
                std::cout << state[i][j]->getIdent() << std::endl;
                auto positions = state[i][j]->validMoves(state, nullptr);

                for (auto& pos : positions) {
                    std::cout << state[i][j]->getIdent() << " " << pos.first << " " << pos.second << std::endl;

                }
            }


        }
    }
}

int main() {
    //test_001();
    //test_002();
    test_003();

	return 0;
}
