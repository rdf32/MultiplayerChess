#include "ChessObjects.h"
#include <iostream>

// Constructor definition for Position
Position::Position(int rowVal, int colVal) : row(rowVal), col(colVal) {}

Board::Board(int rows, int cols) : m_rows(rows), m_cols(cols), m_turn(Piece::Color::WHITE) {
    m_board.resize(rows, std::vector<Piece*>(cols, nullptr));
	initializeBoard();
}

Board::~Board() {
    // Cleanup code (delete dynamically allocated pieces, etc.)
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            if (m_board[i][j] != nullptr) {
                delete m_board[i][j];
            }
        }
    }
}
void Board::initializeBoard() {// Place pawns for both colors
    for (int col = 0; col < m_cols; ++col) {
        m_board[1][col] = new Pawn(Pawn::Color::BLACK);
        m_board[6][col] = new Pawn(Piece::Color::WHITE);
    }

    m_board[0][0] = new Rook(Rook::Color::BLACK);
    m_board[0][1] = new Knight(Knight::Color::BLACK);
    m_board[0][2] = new Bishop(Bishop::Color::BLACK);
    m_board[0][3] = new Queen(Queen::Color::BLACK);
    m_board[0][4] = new King(King::Color::BLACK);
    m_board[0][5] = new Bishop(Bishop::Color::BLACK);
    m_board[0][6] = new Knight(Knight::Color::BLACK);
    m_board[0][7] = new Rook(Rook::Color::BLACK);

    m_board[7][0] = new Rook(Rook::Color::WHITE);
    m_board[7][1] = new Knight(Knight::Color::WHITE);
    m_board[7][2] = new Bishop(Bishop::Color::WHITE);
    m_board[7][3] = new Queen(Queen::Color::WHITE);
    m_board[7][4] = new King(King::Color::WHITE);
    m_board[7][5] = new Bishop(Bishop::Color::WHITE);
    m_board[7][6] = new Knight(Knight::Color::WHITE);
    m_board[7][7] = new Rook(Rook::Color::WHITE);

    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            if (m_board[row][col]) {
                m_board[row][col]->setPos(Position(row, col));
            }
        }
    }
}

void Board::printBoard() const {
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            if (m_board[i][j]) {
                std::cout << (m_board[i][j]->getColor() == Piece::Color::WHITE ? "W" : "B") << m_board[i][j]->getIdent() << " ";
            }
            else {
                std::cout << " . ";  // Empty square
            }
        }
        std::cout << std::endl;
    }
}

void Board::movePiece(const Position& from, const Position& to) {
    if (m_board[from.row][from.col]) {
        Piece* piece = m_board[from.row][from.col];
        if (piece->getColor() == m_turn) {
            if (!piece->hasMoved()) {
                piece->setMoved(true);
            }

            m_board[from.row][from.col] = nullptr;
            m_board[to.row][to.col] = piece;
            piece->setPos(to);
        }
        else {
            std::cout << "Not your turn / piece..." << std::endl;
        }
        
    }
    else {
        std::cout << "No piece in the starting location" << std::endl;
    }
}

Piece* Board::getPiece(const Position& pos) {
    return m_board[pos.row][pos.col];
}

// Constructor definition for Piece Types
Piece::Piece(Color color, std::string ident) : m_color(color), m_pos(0, 0), m_moved(false), m_ident(ident) {}

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

