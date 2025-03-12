#include "ChessObjects.h"
#include <iostream>

Game::Game() : m_board(8, 8), m_turn(Piece::Color::WHITE) {}

void Game::makeMove(Position& from, Position& to) {
    Piece* piece = m_board.getPiece(from);
    if (piece) {
        if (m_turn == piece->getColor()) {
            // check if it is a valid move here as well
            if (!piece->hasMoved()) {
                piece->setMoved(true);
            }
            m_board.removePiece(from);
            m_board.setPiece(piece, to);
            piece->setPos(to);
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

Board::Board(int rows, int cols) : m_rows(rows), m_cols(cols) {
    m_state.resize(rows, std::vector<Piece*>(cols, nullptr));
	initializeBoard();
}

Board::~Board() {
    // Cleanup code (delete dynamically allocated pieces, etc.)
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            if (m_state[i][j] != nullptr) {
                delete m_state[i][j];
            }
        }
    }
}

Piece* Board::getPiece(const Position& pos) {
    return m_state[pos.row][pos.col];
}

void Board::removePiece(const Position& pos) {
    m_state[pos.row][pos.col] = nullptr;
}

void Board::setPiece(Piece* piece, const Position& pos) {
    m_state[pos.row][pos.col] = piece;
}

void Board::initializeBoard() {// Place pawns for both colors
    for (int col = 0; col < m_cols; ++col) {
        m_state[1][col] = new Pawn(Pawn::Color::BLACK);
        m_state[6][col] = new Pawn(Piece::Color::WHITE);
    }

    m_state[0][0] = new Rook(Rook::Color::BLACK);
    m_state[0][1] = new Knight(Knight::Color::BLACK);
    m_state[0][2] = new Bishop(Bishop::Color::BLACK);
    m_state[0][3] = new Queen(Queen::Color::BLACK);
    m_state[0][4] = new King(King::Color::BLACK);
    m_state[0][5] = new Bishop(Bishop::Color::BLACK);
    m_state[0][6] = new Knight(Knight::Color::BLACK);
    m_state[0][7] = new Rook(Rook::Color::BLACK);

    m_state[7][0] = new Rook(Rook::Color::WHITE);
    m_state[7][1] = new Knight(Knight::Color::WHITE);
    m_state[7][2] = new Bishop(Bishop::Color::WHITE);
    m_state[7][3] = new Queen(Queen::Color::WHITE);
    m_state[7][4] = new King(King::Color::WHITE);
    m_state[7][5] = new Bishop(Bishop::Color::WHITE);
    m_state[7][6] = new Knight(Knight::Color::WHITE);
    m_state[7][7] = new Rook(Rook::Color::WHITE);

    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            if (m_state[row][col]) {
                m_state[row][col]->setPos(Position(row, col));
            }
        }
    }
}

void Board::printBoard() const {
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            if (m_state[i][j]) {
                std::cout << (m_state[i][j]->getColor() == Piece::Color::WHITE ? "W" : "B") << m_state[i][j]->getIdent() << " ";
            }
            else {
                std::cout << " . ";  // Empty square
            }
        }
        std::cout << std::endl;
    }
}


// Constructor definition for Piece Types
Piece::Piece(Color color, std::string ident) : m_color(color), m_pos(0, 0), m_moved(false), m_ident(ident) {}


// Getter for position
Position Piece::getPos() const {
    return m_pos;
}

// Getter for color
Piece::Color Piece::getColor() const {
    return m_color;
}

// Setter for position
void Piece::setPos(const Position& pos) {
    m_pos = pos;
}

bool Piece::hasMoved() const {
    return m_moved;
}

void Piece::setMoved(bool moved) {
    m_moved = moved;
}

std::string Piece::getIdent() const {
    return m_ident;
}

Pawn::Pawn(Color color, std::string ident) : Piece(color, ident) {}

Rook::Rook(Color color, std::string ident) : Piece(color, ident) {}

King::King(Color color, std::string ident) : Piece(color, ident) {}

Queen::Queen(Color color, std::string ident) : Piece(color, ident) {}

Bishop::Bishop(Color color, std::string ident) : Piece(color, ident) {}

Knight::Knight(Color color, std::string ident) : Piece(color, ident) {}

// Constructor definition for Position
Position::Position(int rowVal, int colVal) : row(rowVal), col(colVal) {}
