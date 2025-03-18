#include "ChessObjects.h"
#include <iostream>


// Constructor definition for Position
Position::Position(int rowVal, int colVal) : row(rowVal), col(colVal) {}

Move::Move(Position to, Position from): m_from(from), m_to(to) {}

Game::Game() : m_board(8, 8), m_turn(Piece::Color::WHITE) {}

void Game::makeMove(Position& from, Position& to) {
    Piece* piece = m_board.getPiece(from);
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


            m_board.removePiece(from);
            m_board.setPiece(piece, to);
            piece->setPos(to);
            // Mark the piece as moved
            //if (piece != nullptr) {
            //    piece->setMoved();
            //}

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

std::vector<std::vector<Piece*>> Board::getState() {
    return m_state;
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

Piece::PieceType Pawn::getType() const {
    return { PieceType::PAWN };
}

std::unordered_set<std::pair<int, int>, pair_hash> Pawn::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    // need to check for:
    // first move : move one square or two
    // capture diagonal
    // promotion - can be a capture and a promotion at the same time so should be separate checks


    //Regular moves : Check if the square directly in front is empty or not.
    //    Double moves : On the pawn's first move, check if both the square directly in front and the square two squares in front are empty.
    //    Captures : Check diagonals for opponent pieces to capture.
    //    En passant : Handle this only if the pawn's last move was a two-square move and it is adjacent to an opponent's pawn.

    std::unordered_set<std::pair<int, int>, pair_hash> positions;

    int direction = (m_color == Piece::Color::WHITE) ? -1 : 1;  // White moves up, black moves down

    int row = m_pos.row + direction;
    int col = m_pos.col;

    if (row >= 0 && row < 8 && col >= 0 && col < 8 && state[row][col] == nullptr) {
        positions.insert({ row, col });
    }

    if (!m_moved) {
        row = m_pos.row + 2 * direction;
        if (row >= 0 && row < 8 && col >= 0 && col < 8 && state[row][col] == nullptr) {
            positions.insert({ row, col }); // double move if pawn has not moved
        }
    }

    std::vector<std::pair<int, int>> captureDirs = { {-1, -1}, {-1, 1} }; // White captures up-left and up-right
    if (m_color == Piece::Color::BLACK) {
        captureDirs = { {1, -1}, {1, 1} }; // Black captures down-left and down-right
    }
    for (const auto& dir : captureDirs) {
        row = m_pos.row + dir.first;
        col = m_pos.col + dir.second;
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            Piece* pieceAtNewPos = state[row][col];
            if (pieceAtNewPos != nullptr && pieceAtNewPos->getColor() != m_color) {
                positions.insert({ row, col }); // diagonal captures
            }
        }
    }
    // en passant
    if (lastMove && 
        state[lastMove->m_to.row][lastMove->m_to.col]->getType().type == Piece::PieceType::PAWN &&
        state[lastMove->m_to.row][lastMove->m_to.col]->getColor() != m_color &&
        abs(lastMove->m_from.row - lastMove->m_to.row) == 2 &&
        lastMove->m_to.row == m_pos.row) {
        if ((m_pos.col + 1 == lastMove->m_to.col) || (m_pos.col - 1 == lastMove->m_to.col)) {
            row = m_pos.row + direction;
            col = lastMove->m_to.col;
            positions.insert({ row, col });
        }
    }

    return positions;
}

King::King(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType King::getType() const {
    return { PieceType::KING };
}

std::unordered_set<std::pair<int, int>, pair_hash> King::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    //The king and the rook involved must not have moved yet.
    //    There must be no pieces between the king and the rook.
    //    The king must not be in check.
    //    The king must not pass through a square that is attacked by an enemy piece.
    //    The king must not end up in check after castling.
    std::unordered_set<std::pair<int, int>, pair_hash> positions;

    std::vector<std::pair<int, int>> directions = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
    };

    std::vector<std::vector<int>> squaresAttacked(8, std::vector<int>(8));

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = state[i][j];
            if (piece != nullptr && piece->getColor() != m_color) {
                auto attackedPositions = piece->validMoves(state, lastMove);

                for (const auto& pos : attackedPositions) {
                    squaresAttacked[pos.first][pos.second] = 1;
                }
            }
        }
    }

    for (const auto& direction : directions) {
        int r = m_pos.row + direction.first;
        int c = m_pos.col + direction.second;

        // Check if the position is within bounds
        if (r >= 0 && r < 8 && c >= 0 && c < 8 && !squaresAttacked[r][c]) {
            Piece* pieceAtNewPos = state[r][c];
            if (pieceAtNewPos == nullptr || pieceAtNewPos->getColor() != m_color) {
                positions.insert({ r, c }); // Empty square, valid move or enemy piece
            }
        }
    }
    if (!m_moved && !squaresAttacked[m_pos.row][m_pos.col]) {
        // Check if castling is possible (with White King-side and Queen-side Rooks)

        if (state[m_pos.row][7] && 
            !state[m_pos.row][7]->hasMoved() &&
            state[m_pos.row][5] == nullptr && 
            state[m_pos.row][6] == nullptr &&
            !squaresAttacked[m_pos.row][5] &&
            !squaresAttacked[m_pos.row][6]) {

            positions.insert({ m_pos.row, m_pos.col + 2 });  // King-side castling
        }

        if (state[m_pos.row][0] &&
            !state[m_pos.row][0]->hasMoved() &&
            state[m_pos.row][1] == nullptr &&
            state[m_pos.row][2] == nullptr &&
            state[m_pos.row][3] == nullptr &&
            !squaresAttacked[m_pos.row][2] &&
            !squaresAttacked[m_pos.row][3]) {

            positions.insert({ m_pos.row, m_pos.col - 2 });  // Queen-side castling
        }
    }
    return positions;
}

Queen::Queen(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Queen::getType() const {
    return { PieceType::QUEEN };
}

std::unordered_set<std::pair<int, int>, pair_hash> Queen::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<std::pair<int, int>, pair_hash> positions;
    
    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, 1}, {1, 1}, {1, -1}, {-1, -1}
    };

    for (const auto& direction : directions) {
        int r = m_pos.row;
        int c = m_pos.col;
        
        while (true) {
            r += direction.first;
            c += direction.second;

            // Check if the position is off the board
            if (r < 0 || r >= 8 || c < 0 || c >= 8) {
                break; // Outside the board, stop moving
            }

            // Check the piece at the new position
            Piece* pieceAtNewPos = state[r][c];
            if (pieceAtNewPos == nullptr) {
                positions.insert({ r, c }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ r, c }); // Opponent piece, valid capture
                break; // Stop moving after capturing
            }
            else if (pieceAtNewPos->getColor() == m_color) {
                break; // Friendly piece blocks the move
            }
        }
    }

    return positions;
}

Rook::Rook(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Rook::getType() const {
    return { PieceType::ROOK };
}

std::unordered_set<std::pair<int, int>, pair_hash> Rook::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<std::pair<int, int>, pair_hash> positions;

    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& direction : directions) {
        int r = m_pos.row;
        int c = m_pos.col;

        while (true) {
            r += direction.first;
            c += direction.second;

            // Check if the position is off the board
            if (r < 0 || r >= 8 || c < 0 || c >= 8) {
                break; // Outside the board, stop moving
            }

            // Check the piece at the new position
            Piece* pieceAtNewPos = state[r][c];
            if (pieceAtNewPos == nullptr) {
                positions.insert({ r, c }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ r, c }); // Opponent piece, valid capture
                break; // Stop moving after capturing
            }
            else if (pieceAtNewPos->getColor() == m_color) {
                break; // Friendly piece blocks the move
            }
        }
    }

    return positions;
}

Bishop::Bishop(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Bishop::getType() const {
    return { PieceType::BISHOP };
}

std::unordered_set<std::pair<int, int>, pair_hash> Bishop::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {
    
    std::unordered_set<std::pair<int, int>, pair_hash> positions;
    
    std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    for (const auto& direction : directions) {
        int r = m_pos.row;
        int c = m_pos.col;

        while (true) {
            r += direction.first;
            c += direction.second;

            // Check if the position is off the board
            if (r < 0 || r >= 8 || c < 0 || c >= 8) {
                break; // Outside the board, stop moving
            }

            // Check the piece at the new position
            Piece* pieceAtNewPos = state[r][c];
            if (pieceAtNewPos == nullptr) {
                positions.insert({ r, c }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ r, c }); // Opponent piece, valid capture
                break; // Stop moving after capturing
            }
            else if (pieceAtNewPos->getColor() == m_color) {
                break; // Friendly piece blocks the move
            }
        }
    }

    return positions;
}


Knight::Knight(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Knight::getType() const {
    return { PieceType::KNIGHT };
}

std::unordered_set<std::pair<int, int>, pair_hash> Knight::validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<std::pair<int, int>, pair_hash> positions;

    std::vector<std::pair<int, int>> directions = {
        {-2, 1}, {-2, -1}, {2, -1}, {2, 1},  // Vertical L-shapes
        {-1, 2}, {-1, -2}, {1, -2}, {1, 2}   // Horizontal L-shapes
    };

    // Check each of the 8 possible moves
    for (const auto& direction : directions) {
        int r = m_pos.row + direction.first;
        int c = m_pos.col + direction.second;

        // Check if the position is off the board
        if (r < 0 || r >= 8 || c < 0 || c >= 8) {
            continue; // Skip invalid positions off the board
        }

        // Check the piece at the new position
        Piece* pieceAtNewPos = state[r][c];
        if (pieceAtNewPos == nullptr) {
            positions.insert({ r, c }); // Empty square, valid move
        }
        else if (pieceAtNewPos->getColor() != m_color) {
            positions.insert({ r, c }); // Opponent piece, valid capture
        }
        else if (pieceAtNewPos->getColor() == m_color) {
            // Friendly piece blocks the move
            continue;
        }
    }

    return positions;
}


