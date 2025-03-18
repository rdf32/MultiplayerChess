// MultiplayerChess.cpp : Defines the entry point for the application.
//
#include <random>
#include <iostream>
#include "ChessObjects.h"

class Player {
public:

    Player();
    ~Player() = default;
    Piece::Color getColor();
    void setColor(Piece::Color color);

private:
    Piece::Color m_color;

};

Player::Player(): m_color(Piece::Color::WHITE) {}

Piece::Color Player::getColor() {
    return m_color;
}

void Player::setColor(Piece::Color color) {
    m_color = color;
}


class Game {
public:
    Game(Player& player_1, Player& player_2);

    virtual ~Game() = default;

    void makeMove(Move& move);

    void playGame();

    Board m_board;


private:
    Piece::Color m_turn;
    std::vector<Move*> history;
};

Game::Game(Player& player_1, Player& player_2) : m_board(8, 8), m_turn(Piece::Color::WHITE), history(NULL) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    Piece::Color player1Color = (dis(gen) == 0) ? Piece::Color::WHITE : Piece::Color::BLACK;
    Piece::Color player2Color = (player1Color == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;

    player_1.setColor(player1Color);
    player_2.setColor(player2Color);

}

void Game::playGame() {
    while (true) {
        int startRow, startCol, endRow, endCol;
        m_board.printBoard();

        std::cout << (m_turn == Piece::Color::WHITE ? "White" : "Black") << " to move" << std::endl;
        std::cout << "Enter the row and column of the piece you want to move (row col): ";
        std::cin >> startRow >> startCol;
        if (startRow < 0 || startRow > 7 || startCol < 0 || startCol > 7) {
            std::cout << "Invalid starting position! Please enter values between 0 and 7 for both row and column." << std::endl;
            playGame();
        }

        std::cout << "Enter the row and column for the destination (row col): ";
        std::cin >> endRow >> endCol;

        // Validate the destination position
        if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
            std::cout << "Invalid destination position! Please enter values between 0 and 7 for both row and column." << std::endl;
            playGame();
        }
        Position fromPos(startRow, startCol);
        Position toPos(endRow, endCol);

        Move move(fromPos, toPos);

        makeMove(move);
        m_board.printBoard();


    }
}
// could have makeMove return a boolean on success or false on not
void Game::makeMove(Move& move) {
    Piece* piece = m_board.getPiece(move.m_from);
    if (piece) {
        if (m_turn == piece->getColor()) {
            // after each move need to check if put a king in check and or if checkmate was performed
            // maybe create an attacked board -> binary 2d array stating if the location is attacked or not
            // then check if the king square is attacked to see if in check
            // can only make a move that isn't a king move if king isn't in check
            // check if it is a valid move here as well
            // check what type of move it is here looking for certain cases
            // looking for en pessant, promotion, castling, or standard
            // could maybe make a member function on all the pieces that returns the type of move
            // call the member function moveType(Position& from, Position& to)
            // if gonna make a validMoves() function for Piece -> could call it every time
            // validMoves should create an internal set of current valid moves that could be checked against
            // setPiece is called -> would just need to also use setPiece during initialize Board
            // need to invalid to the move types
            // then could set up a switch here with the default for standard moves
            // then other cases for the other types of moves because some require moving two pieces
            // need to handle promotions - but probably will have board handle this


            m_board.removePiece(move.m_from);
            m_board.setPiece(piece, move.m_to);
            piece->setPos(move.m_to);

            if (!piece->hasMoved()) {
                piece->setMoved(true);
            }
            m_turn = m_turn == Piece::Color::WHITE ? Piece::Color::BLACK : Piece::Color::WHITE;
        }
        else {
            std::cout << "That is not your piece or not your turn..." << std::endl;
        }

    }
    else {
        std::cout << "No piece on that square..." << std::endl;
    }
}








void test_001() {
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
    chessBoard.printBoard();
}

void test_002() {

    Player player_1;
    Player player_2;

    Game chessGame(player_1, player_2);
    chessGame.playGame();

}

int main() {

    test_002();
	return 0;
}
