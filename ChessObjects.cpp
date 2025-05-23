#include "ChessObjects.h"
#include <iostream>


// Constructor definition for Position
Position::Position(int rowVal, int colVal) : row(rowVal), col(colVal) {}

bool Position::operator==(const Position& other) const {
    return row == other.row && col == other.col;
}

std::ostream& operator<<(std::ostream& os, const Position& pos) {
    os << "(" << pos.row << ", " << pos.col << ")";
    return os;
}

std::size_t position_hash::operator()(const Position& p) const {
        // Hash the row and col individually
        auto h1 = std::hash<int>{}(p.row);  // Hash of the row
        auto h2 = std::hash<int>{}(p.col);  // Hash of the col

        // Combine the two hash values (XOR and shift)
        return h1 ^ (h2 << 1);  // XOR the hashes and shift the second hash
}

PositionType::PositionType(std::pair<int, int> npair, PositionType::MoveType nmtype) : pair(npair), mtype(nmtype) {}

bool PositionType::operator==(const PositionType& other) const {
    return pair.first == other.pair.first && pair.second == other.pair.second && mtype == other.mtype;
}

std::size_t positionType_hash::operator()(const PositionType& ptype) const {

    size_t posHash = pair_hash()(ptype.pair);
    size_t mtypeHash = std::hash<int>()(static_cast<int>(ptype.mtype));
    return posHash ^ (mtypeHash << 1);
}

Move::Move(Position from, Position to): m_from(from), m_to(to) {}

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

void Board::createPiece(Piece::PieceType::Type ptype, Piece::Color pcolor, Position& pos) {
    switch (ptype) {

    case Piece::PieceType::QUEEN:
        m_state[pos.row][pos.col] = new Queen(pcolor);
        m_state[pos.row][pos.col]->setPos(pos);
        break;

    case Piece::PieceType::BISHOP:
        m_state[pos.row][pos.col] = new Bishop(pcolor);
        m_state[pos.row][pos.col]->setPos(pos);
        break;

    case Piece::PieceType::KNIGHT:
        m_state[pos.row][pos.col] = new Knight(pcolor);
        m_state[pos.row][pos.col]->setPos(pos);
        break;

    case Piece::PieceType::ROOK:
        m_state[pos.row][pos.col] = new Rook(pcolor);
        m_state[pos.row][pos.col]->setPos(pos);
        break;

    default:
        std::cerr << "Invalid piece type!" << std::endl;
        break;
    }

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
    // Print the column numbers at the top
    std::cout << "   ";
    for (int j = 0; j < m_cols; ++j) {
        std::cout << j << "  ";
    }
    std::cout << std::endl;

    // Print each row with the row number on the left side
    for (int i = 0; i < m_rows; ++i) {
        // Print the row number
        std::cout << i << "  ";

        for (int j = 0; j < m_cols; ++j) {
            if (m_state[i][j]) {
                std::cout << (m_state[i][j]->getColor() == Piece::Color::WHITE ? "W" : "B")
                    << m_state[i][j]->getIdent() << " ";
            }
            else {
                std::cout << " . ";  // Empty square
            }
        }
        std::cout << std::endl;
    }
    std::cout << " " << std::endl;
}

// Constructor definition for Piece Types
Piece::Piece(Color color, std::string ident) : m_color(color), m_pos(0, 0), m_moved(false), m_ident(ident), m_defended(false) {}

// Getter for position
Position Piece::getPos() const {
    return m_pos;
}

Piece::Color Piece::getColor() const {
    return m_color;
}

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

bool Piece::isDefended(const std::vector<std::vector<Piece*>>& state) {
    Position position = getPos();

    // check for defending pawns
    int pawn_direction = (m_color == Piece::Color::WHITE) ? 1 : -1;  // White moves up, black moves down
    for (int offset : {-1, 1}) {
        int r = position.row + pawn_direction;
        int c = position.col + offset;
        if (r < 0 || r >= 8 || c < 0 || c >= 8) {
            continue; // Skip invalid positions off the board
        }
        Piece* pieceAtNewPos = state[r][c];
        if (pieceAtNewPos == nullptr) {
            continue;
        }
        if (state[r][c]->getType().type == Piece::PieceType::PAWN &&
            state[r][c]->getColor() == m_color) {
            return true;
        }
    }
    // check for defending knights
    std::vector<std::pair<int, int>> knight_directions = {
        {-2, 1}, {-2, -1}, {2, -1}, {2, 1},  // Vertical L-shapes
        {-1, 2}, {-1, -2}, {1, -2}, {1, 2}   // Horizontal L-shapes
    };

    for (const auto& direction : knight_directions) {
        int r = position.row + direction.first;
        int c = position.col + direction.second;

        // Check if the position is off the board
        if (r < 0 || r >= 8 || c < 0 || c >= 8) {
            continue; // Skip invalid positions off the board
        }

        // Check the piece at the new position
        Piece* pieceAtNewPos = state[r][c];
        if (pieceAtNewPos == nullptr) {
            continue;
        }
        if (pieceAtNewPos->getType().type == Piece::PieceType::KNIGHT &&
            pieceAtNewPos->getColor() == m_color) {
            return true; 
        }
    }

    // check for attacking diagonal
    std::vector<std::pair<int, int>> diagonal_directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    for (const auto& direction : diagonal_directions) {
        int r = position.row;
        int c = position.col;

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
                continue;
            }
            if (pieceAtNewPos->getColor() == m_color) {
                if (pieceAtNewPos->getType().type == Piece::PieceType::BISHOP ||
                    pieceAtNewPos->getType().type == Piece::PieceType::QUEEN) {
                    return true;
                }
                else {
                    break;
                }
            }
            else {
                break; // Friendly piece blocks the move
            }
        }
    }

    // check for attacking files and ranks
    std::vector<std::pair<int, int>> rankfile_directions = {
        {-1, 0}, {1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& direction : rankfile_directions) {
        int r = position.row;
        int c = position.col;

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
                continue;
            }
            if (pieceAtNewPos->getColor() == m_color) {
                if (pieceAtNewPos->getType().type == Piece::PieceType::ROOK ||
                    pieceAtNewPos->getType().type == Piece::PieceType::QUEEN) {
                    return true;
                }
                else {
                    break;
                }
            }
            else {
                break; // Friendly piece blocks the move
            }
        }
    }

    return false;
}

std::pair<int, int> calcDirectionToKing(const Position& piecePos, const Position& kingPos) {
    int row_out, col_out;
    int row_diff = kingPos.row - piecePos.row;
    int col_diff = kingPos.col - piecePos.col;

    if (row_diff == 0) {
        row_out = 0;
    }
    else {
        row_out = row_diff / abs(row_diff);
    }

    if (col_diff == 0) {
        col_out = 0;
    }
    else {
        col_out = col_diff / abs(col_diff);
    }

    return { row_out, col_out };
}

Pawn::Pawn(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Pawn::getType() const {
    return { PieceType::PAWN };
}

std::unordered_set<PositionType, positionType_hash> Pawn::lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) {
    std::unordered_set<PositionType, positionType_hash> attackedSquares;
    attackedSquares.insert({ { m_pos.row, m_pos.col }, PositionType::MoveType::STND });
    return attackedSquares;
}

std::unordered_set<PositionType, positionType_hash> Pawn::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<PositionType, positionType_hash> positions;

    int direction = (m_color == Piece::Color::WHITE) ? -1 : 1;  // White moves up, black moves down

    int row = m_pos.row + direction;
    int col = m_pos.col;

    if (row >= 1 && row < 7 && col >= 0 && col < 8 && state[row][col] == nullptr) {
        positions.insert({ { row, col }, PositionType::MoveType::STND });
    } else if (row == 0 && m_color == Piece::Color::WHITE && col >= 0 && col < 8 && state[row][col] == nullptr) {
        positions.insert({ { row, col }, PositionType::MoveType::PROM });
    } else if (row == 7 && m_color == Piece::Color::BLACK && col >= 0 && col < 8 && state[row][col] == nullptr) {
        positions.insert({ { row, col }, PositionType::MoveType::PROM });
    }

    if (!m_moved) {
        row = m_pos.row + 2 * direction;
        if (row >= 0 && row < 8 && col >= 0 && col < 8 && state[row][col] == nullptr) {
            positions.insert({ { row, col }, PositionType::MoveType::STND }); // double move if pawn has not moved
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
                if (row == 0 && m_color == Piece::Color::WHITE) {
                    positions.insert({ { row, col }, PositionType::MoveType::PROM });
                }
                else if (row == 7 && m_color == Piece::Color::BLACK) {
                    positions.insert({ { row, col }, PositionType::MoveType::PROM });
                }
                else {
                    positions.insert({ { row, col }, PositionType::MoveType::CAPT }); // diagonal captures
                }
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
            positions.insert({ { row, col }, PositionType::MoveType::ENPASS });
        }
    }

    return positions;
}

King::King(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType King::getType() const {
    return { PieceType::KING };
}


std::unordered_set<PositionType, positionType_hash> King::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    //The king and the rook involved must not have moved yet.
    //    There must be no pieces between the king and the rook.
    //    The king must not be in check.
    //    The king must not pass through a square that is attacked by an enemy piece.
    //    The king must not end up in check after castling.
    //    The king can not capture a piece if it is defended
    std::unordered_set<PositionType, positionType_hash> positions;

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
                std::unordered_set<PositionType, positionType_hash> attackedPositions;
                if (piece->getType().type == Piece::PieceType::KING) {
                    Position pos = piece->getPos();
                    attackedPositions = {
                        {{pos.row, pos.col - 1}, PositionType::MoveType::STND}, {{pos.row, pos.col + 1 }, PositionType::MoveType::STND}, 
                        {{pos.row - 1, pos.col}, PositionType::MoveType::STND}, {{pos.row + 1, pos.col }, PositionType::MoveType::STND},
                        {{pos.row - 1, pos.col - 1}, PositionType::MoveType::STND}, {{pos.row - 1, pos.col + 1}, PositionType::MoveType::STND}, 
                        {{pos.row + 1, pos.col - 1}, PositionType::MoveType::STND}, {{pos.row + 1, pos.col + 1}, PositionType::MoveType::STND}
                    };
                }
                else {
                    attackedPositions = piece->validMoves(state, lastMove);
                }
                
                for (const auto& pos : attackedPositions) {
                    if (pos.pair.first >= 0 && pos.pair.first < 8 && pos.pair.second >= 0 && pos.pair.second < 8) {
                        squaresAttacked[pos.pair.first][pos.pair.second] = 1;
                    }
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
            if (pieceAtNewPos == nullptr ) {
                positions.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move or enemy piece
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                if (!pieceAtNewPos->isDefended(state)) {
                    positions.insert({ { r, c }, PositionType::MoveType::CAPT });
                }
                // can only capture if the piece is not defended
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

            positions.insert({ { m_pos.row, m_pos.col + 2 }, PositionType::MoveType::KCASTLE });  // King-side castling
        }

        if (state[m_pos.row][0] &&
            !state[m_pos.row][0]->hasMoved() &&
            state[m_pos.row][1] == nullptr &&
            state[m_pos.row][2] == nullptr &&
            state[m_pos.row][3] == nullptr &&
            !squaresAttacked[m_pos.row][2] &&
            !squaresAttacked[m_pos.row][3]) {

            positions.insert({ { m_pos.row, m_pos.col - 2 }, PositionType::MoveType::QCASTLE });  // Queen-side castling
        }
    }
    return positions;
}

Queen::Queen(Color color, std::string ident) : Piece(color, ident) {}

Piece::PieceType Queen::getType() const {
    return { PieceType::QUEEN };
}

std::unordered_set<PositionType, positionType_hash> Queen::lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) {
    std::unordered_set<PositionType, positionType_hash> attackedSquares;
    std::pair<int, int> direction = calcDirectionToKing(getPos(), kingPos);

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
            attackedSquares.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
        }
        else if (pieceAtNewPos->getColor() != m_color) {
            attackedSquares.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
            break; // Stop moving after capturing
        }
        else if (pieceAtNewPos->getColor() == m_color) {
            break; // Friendly piece blocks the move
        }
    }
    attackedSquares.insert({ { m_pos.row, m_pos.col }, PositionType::MoveType::STND });
    return attackedSquares;
}

std::unordered_set<PositionType, positionType_hash> Queen::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<PositionType, positionType_hash> positions;
    
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
                positions.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
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

std::unordered_set<PositionType, positionType_hash> Rook::lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) {
    std::unordered_set<PositionType, positionType_hash> attackedSquares;
    std::pair<int, int> direction = calcDirectionToKing(getPos(), kingPos);

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
            attackedSquares.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
        }
        else if (pieceAtNewPos->getColor() != m_color) {
            attackedSquares.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
            break; // Stop moving after capturing
        }
        else if (pieceAtNewPos->getColor() == m_color) {
            break; // Friendly piece blocks the move
        }
    }
    attackedSquares.insert({ { m_pos.row, m_pos.col }, PositionType::MoveType::STND });
    return attackedSquares;
}

std::unordered_set<PositionType, positionType_hash> Rook::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<PositionType, positionType_hash> positions;

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
                positions.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
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

std::unordered_set<PositionType, positionType_hash> Bishop::lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) {
    std::unordered_set<PositionType, positionType_hash> attackedSquares;
    std::pair<int, int> direction = calcDirectionToKing(getPos(), kingPos);

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
            attackedSquares.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
        }
        else if (pieceAtNewPos->getColor() != m_color) {
            attackedSquares.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
            break; // Stop moving after capturing
        }
        else if (pieceAtNewPos->getColor() == m_color) {
            break; // Friendly piece blocks the move
        }
    }
    attackedSquares.insert({ { m_pos.row, m_pos.col }, PositionType::MoveType::STND });
    return attackedSquares;
}

std::unordered_set<PositionType, positionType_hash> Bishop::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {
    
    std::unordered_set<PositionType, positionType_hash> positions;
    
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
                positions.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
            }
            else if (pieceAtNewPos->getColor() != m_color) {
                positions.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
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

std::unordered_set<PositionType, positionType_hash> Knight::lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) {
    std::unordered_set<PositionType, positionType_hash> attackedPieces;
    attackedPieces.insert({ { m_pos.row, m_pos.col }, PositionType::MoveType::STND });
    return attackedPieces;
}

std::unordered_set<PositionType, positionType_hash> Knight::validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_set<PositionType, positionType_hash> positions;

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
            positions.insert({ { r, c }, PositionType::MoveType::STND }); // Empty square, valid move
        }
        else if (pieceAtNewPos->getColor() != m_color) {
            positions.insert({ { r, c }, PositionType::MoveType::CAPT }); // Opponent piece, valid capture
        }
        else if (pieceAtNewPos->getColor() == m_color) {
            // Friendly piece blocks the move
            continue;
        }
    }

    return positions;
}


