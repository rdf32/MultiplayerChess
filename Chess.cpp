// MultiplayerChess.cpp : Defines the entry point for the application.
//
#include <random>
#include <iostream>
#include <queue> 
#include <deque>
#include <unordered_map>

#include "ChessObjects.h"

class Player {
public:

    Player();
    ~Player() = default;
    Piece::Color getColor();
    void setColor(Piece::Color color);
    Position getKingpos();
    void setKingpos(Position& pos);
    std::vector<Piece*> attackingPieces(const std::vector<std::vector<Piece*>>& state);
    std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash> legalMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove);



private:
    Piece::Color m_color;
    std::queue<Move*> moveQueue;
    Position m_kingPos;
};

Player::Player(): m_color(Piece::Color::WHITE), m_kingPos(Position(0, 0)) {}

Piece::Color Player::getColor() {
    return m_color;
}

void Player::setColor(Piece::Color color) {
    m_color = color;
}

Position Player::getKingpos() {
    return m_kingPos;
}

void Player::setKingpos(Position& pos) {
    m_kingPos = pos;
}

// some sort of bug in this function
std::vector<Piece*> Player::attackingPieces(const std::vector<std::vector<Piece*>>& state) {
    std::vector<Piece*> piecesAttacking;

    Position kingPos = getKingpos();

    // check for attacking pawns
    int pawn_direction = (m_color == Piece::Color::WHITE) ? -1 : 1;  // White moves up, black moves down
    for (int offset : {-1, 1}) {
        int r = kingPos.row + pawn_direction;
        int c = kingPos.col + offset;
        if (r < 0 || r >= 8 || c < 0 || c >= 8) {
            continue; // Skip invalid positions off the board
        }
        Piece* pieceAtNewPos = state[r][c];
        if (pieceAtNewPos == nullptr) {
            continue;
        }
        if (state[r][c]->getType().type == Piece::PieceType::PAWN &&
            state[r][c]->getColor() != m_color) {
            piecesAttacking.push_back(state[r][c]);
        }
    }
    // check for attacking knights
    std::vector<std::pair<int, int>> knight_directions = {
        {-2, 1}, {-2, -1}, {2, -1}, {2, 1},  // Vertical L-shapes
        {-1, 2}, {-1, -2}, {1, -2}, {1, 2}   // Horizontal L-shapes
    };

    for (const auto& direction : knight_directions) {
        int r = kingPos.row + direction.first;
        int c = kingPos.col + direction.second;

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
            pieceAtNewPos->getColor() != m_color) {
            piecesAttacking.push_back(pieceAtNewPos); // Empty square, valid move
        }
    }

    // check for attacking diagonal
    std::vector<std::pair<int, int>> diagonal_directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    for (const auto& direction : diagonal_directions) {
        int r = kingPos.row;
        int c = kingPos.col;

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
            if (pieceAtNewPos->getColor() != m_color) {
                if (pieceAtNewPos->getType().type == Piece::PieceType::BISHOP ||
                    pieceAtNewPos->getType().type == Piece::PieceType::QUEEN) {
                    piecesAttacking.push_back(pieceAtNewPos);
                    break;
                }
                else {
                    break;
                }
            } else {
                break; // Friendly piece blocks the move
            }
        }
    }

    // check for attacking files and ranks
    std::vector<std::pair<int, int>> rankfile_directions = {
        {-1, 0}, {1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& direction : rankfile_directions) {
        int r = kingPos.row;
        int c = kingPos.col;

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
            if (pieceAtNewPos->getColor() != m_color) {
                if (pieceAtNewPos->getType().type == Piece::PieceType::ROOK ||
                    pieceAtNewPos->getType().type == Piece::PieceType::QUEEN) {
                    piecesAttacking.push_back(pieceAtNewPos);
                    break;
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

    return piecesAttacking;
}
std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash> Player::legalMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) {

    std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash> legalPieceMoves;
    std::vector<Piece*> piecesAttacking = attackingPieces(state);
    std::cout << "number of attacking pieces: " << piecesAttacking.size() << std::endl;

    Position kingPos = getKingpos();
    for (size_t i = 0; i < piecesAttacking.size(); i++) {
        std::cout << "Piece attacking: " << piecesAttacking[i]->getIdent() << (piecesAttacking[i]->getColor() == Piece::Color::WHITE ? "W" : "B") << std::endl;
    }

    if (piecesAttacking.size() == 0) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Piece* piece = state[i][j];
                if (piece == nullptr) {
                    continue;
                }
                else if (piece->getColor() == m_color) {
                    Position from_pos = piece->getPos();
                    std::unordered_set<PositionType, positionType_hash> pieceMoves = piece->validMoves(state, lastMove);
                    legalPieceMoves[from_pos].insert(pieceMoves.begin(), pieceMoves.end());
                }
            }
        }
    }
    else if (piecesAttacking.size() == 1) {
        std::unordered_set<std::pair<int, int>, pair_hash> attackedSquares;
        for (const auto& piece : piecesAttacking) {
            std::unordered_set<PositionType, positionType_hash> attPieceMoves = piece->validMoves(state, lastMove);
            for (auto& pos : attPieceMoves) {
                attackedSquares.insert(pos.pair);
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Piece* piece = state[i][j];
                if (piece == nullptr) {
                    continue;
                }
                else if (piece->getColor() == m_color) {
                    Position from_pos = piece->getPos();
                    std::unordered_set<PositionType, positionType_hash> pieceMoves = piece->validMoves(state, lastMove);
                    for (const auto& pos : pieceMoves) {
                        if (piece->getType().type == Piece::PieceType::KING) {
                            legalPieceMoves[from_pos].insert(pos);
                        }
                        else {
                            if (attackedSquares.find(pos.pair) != attackedSquares.end()) {
                                legalPieceMoves[from_pos].insert(pos);
                            }
                        }
                    }
                }
            }
        }
    }
    else if (piecesAttacking.size() > 1) {
        Piece* piece = state[kingPos.row][kingPos.col];
        Position from_pos = piece->getPos();
        std::unordered_set<PositionType, positionType_hash> pieceMoves = piece->validMoves(state, lastMove);
        legalPieceMoves[from_pos].insert(pieceMoves.begin(), pieceMoves.end());
    }

    return legalPieceMoves;
}

class Game {
public:
    Game(Player& player_1, Player& player_2);

    virtual ~Game() = default;

    void makeMove(Player& currentPlayer, Move& move, PositionType::MoveType mtype, Move* lastMove);

    void playGame();

    Move* getLastMove();

    void addMoveToHistory(const Move& move);

    std::vector<std::vector<Piece*>> getState();

private:
    Board m_board;
    Piece::Color m_turn;
    std::deque<Move> history; // push_front(), pop_front(), push_back(), pop_back()
    Player whitePieces;
    Player blackPieces;
};

Game::Game(Player& player_1, Player& player_2) : m_board(8, 8), m_turn(Piece::Color::WHITE) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    Piece::Color player1Color = (dis(gen) == 0) ? Piece::Color::WHITE : Piece::Color::BLACK;
    Piece::Color player2Color = (player1Color == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;

    Position player_1_kp = (player1Color == Piece::Color::WHITE) ? Position(7, 4) : Position(0, 4);
    Position player_2_kp = (player2Color == Piece::Color::WHITE) ? Position(7, 4) : Position(0, 4);


    player_1.setColor(player1Color);
    player_1.setKingpos(player_1_kp);
    player_2.setColor(player2Color);
    player_2.setKingpos(player_2_kp);

    whitePieces = (player1Color == Piece::Color::WHITE) ? player_1 : player_2;
    blackPieces = (player1Color == Piece::Color::BLACK) ? player_1 : player_2;

}

std::vector<std::vector<Piece*>> Game::getState() {
    return m_board.getState();
}

Move* Game::getLastMove() {
    if (!history.empty()) {
        return &history.back();
    }
    else {
        return nullptr;
    }
}

void Game::addMoveToHistory(const Move& move) {
    history.push_back(move);
}

// request moves from whitePieces or blackPieces
// push move onto their queue
// check whos turn it is
// pop move off player whos turn it is
// get all their valid moves
    // to get valid moves check if they are in checkmate
        // if they are in checkmate (current board state) return no valid moves: game ends
        // check if they are in check (current board state)
        // determine if it is double check
        // if they are in check return only valid moves that can be made while in check
// if their move is valid, perform the move 
// push the move onto the move history stack and change the turn to the other player
// if the move is not valid, pop another move from the queue or prompt another input (start from top)
// if the move is a king, update king position of respective player


//Piece
//Color m_color;
//Position m_pos;
//bool m_moved;
//std::string m_ident;

// Player 
//Piece::Color m_color;
//std::queue<Move*> moveQueue;
//Position kingPos;

//Game
//Board m_board;
//Piece::Color m_turn;
//std::deque<Move> history; // push_front(), pop_front(), push_back(), pop_back()
//Player whitePieces;
//Player blackPieces;

void printLegalMoves(const std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash>& legalMoves) {
    // Iterate through the map
    for (const auto& entry : legalMoves) {
        const Position& key = entry.first;
        const std::unordered_set<PositionType, positionType_hash>& valueSet = entry.second;

        // Print the key (Position)
        std::cout << "Key: " << key << " -> ";

        // Print the values (set of Positions)
        std::cout << "Values: { ";
        for (const auto& pos : valueSet) {
            std::cout << "(" << pos.pair.first << ", " << pos.pair.second << ")" << " ";
        }
        std::cout << "}" << std::endl;
    }
}

void Game::playGame() {
    while (true) {

        Player currentPlayer = (m_turn == Piece::Color::WHITE) ? whitePieces : blackPieces;

        std::vector<std::vector<Piece*>> gameState = getState();
        Move* lastMove = getLastMove();
        std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash> legalMoves = currentPlayer.legalMoves(gameState, lastMove);


        printLegalMoves(legalMoves);


        int startRow, startCol, endRow, endCol;
        m_board.printBoard();

        std::cout << (currentPlayer.getColor() == Piece::Color::WHITE ? "White" : "Black") << " to move" << std::endl;
        std::cout << "Enter the row and column of the piece you want to move (row col): ";
        std::cin >> startRow >> startCol;

        std::cout << "Enter the row and column for the destination (row col): ";
        std::cin >> endRow >> endCol;


        Position fromPos(startRow, startCol);
        Position toPos(endRow, endCol);

        if (legalMoves.find(fromPos) != legalMoves.end()) {
            for (const auto& pos : legalMoves[fromPos]) {
                if (pos.pair.first == toPos.row && pos.pair.second == toPos.col) {
                    Move move(fromPos, toPos);
                    makeMove(currentPlayer, move, pos.mtype, lastMove);
                    addMoveToHistory(move);
                    m_turn = (m_turn == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
                    break;
                }
            }
        }
    }
}

void Game::makeMove(Player& currentPlayer, Move& move, PositionType::MoveType mtype, Move* lastMove) {

    Piece::Color pcolor = currentPlayer.getColor();
    Piece* piece = m_board.getPiece(move.m_from);
    m_board.removePiece(move.m_from);

    if (mtype == PositionType::MoveType::KCASTLE) {
        if (pcolor == Piece::Color::WHITE) {
            Piece* rook = m_board.getPiece(Position(7, 7));
            m_board.removePiece(Position(7, 7));

            m_board.setPiece(piece, Position(7, 6));
            m_board.setPiece(rook, Position(7, 5));
            piece->setPos(Position(7, 6));
            rook->setPos(Position(7, 5));
            piece->setMoved(true);
            rook->setMoved(true);
        }
        else if (pcolor == Piece::Color::BLACK) {
            Piece* rook = m_board.getPiece(Position(0, 7));
            m_board.removePiece(Position(0, 7));

            m_board.setPiece(piece, Position(0, 6));
            m_board.setPiece(rook, Position(0, 5));
            piece->setPos(Position(0, 6));
            rook->setPos(Position(0, 5));
            piece->setMoved(true);
            rook->setMoved(true);
        }
    }
    else if (mtype == PositionType::MoveType::QCASTLE) {
        if (pcolor == Piece::Color::WHITE) {
            Piece* rook = m_board.getPiece(Position(7, 0));
            m_board.removePiece(Position(7, 0));

            m_board.setPiece(piece, Position(7, 2));
            m_board.setPiece(rook, Position(7, 3));
            piece->setPos(Position(7, 2));
            rook->setPos(Position(7, 3));
            piece->setMoved(true);
            rook->setMoved(true);
        }
        else if (pcolor == Piece::Color::BLACK) {
            Piece* rook = m_board.getPiece(Position(0, 0));
            m_board.removePiece(Position(0, 0));

            m_board.setPiece(piece, Position(0, 2));
            m_board.setPiece(rook, Position(0, 3));
            piece->setPos(Position(0, 2));
            rook->setPos(Position(0, 3));
            piece->setMoved(true);
            rook->setMoved(true);
        }
    }
    else if (mtype == PositionType::MoveType::ENPASS) {
        m_board.setPiece(piece, move.m_to);
        piece->setPos(move.m_to);
        int direction = (pcolor == Piece::Color::WHITE) ? 1 : -1;
        m_board.removePiece(Position(move.m_to.row + direction, move.m_to.col));
        piece->setMoved(true);
    }
    else if (mtype == PositionType::MoveType::PROM) {
        std::cout << "Implement logic for promoting a pawn -> also for checking for promotion in valid moves" << std::endl;
    }
    else {
        m_board.setPiece(piece, move.m_to);
        piece->setPos(move.m_to);
        if (!piece->hasMoved()) {
            piece->setMoved(true);
        }
    }

    if (piece->getType().type == Piece::PieceType::KING) {
        currentPlayer.setKingpos(move.m_to);
    }
    m_board.printBoard();
}
//// need to add logic to remove the piece captured during en pessant from the board
//if (lastMove &&
//    state[lastMove->m_to.row][lastMove->m_to.col]->getType().type == Piece::PieceType::PAWN &&
//    state[lastMove->m_to.row][lastMove->m_to.col]->getColor() != m_color &&
//    abs(lastMove->m_from.row - lastMove->m_to.row) == 2 &&
//    lastMove->m_to.row == m_pos.row) {
//    if ((m_pos.col + 1 == lastMove->m_to.col) || (m_pos.col - 1 == lastMove->m_to.col)) {
//        row = m_pos.row + direction;
//        col = lastMove->m_to.col;
//        positions.insert({ row, col });
//    }
//}

//return positions;






//void test_001() {
//    Board chessBoard(8, 8);
//
//    std::vector<std::vector<Piece*>> state = chessBoard.getState();
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            if (state[i][j]) {
//                std::cout << state[i][j]->getIdent() << std::endl;
//                auto positions = state[i][j]->validMoves(state, nullptr);
//
//                for (auto& pos : positions) {
//                    std::cout << state[i][j]->getIdent() << " " << pos.first << " " << pos.second << std::endl;
//
//                }
//            }
//        }
//    }
//}

void test_002() {

    Player player_1;
    Player player_2;

    Game chessGame(player_1, player_2);
    chessGame.playGame();

}

// could have makeMove return a boolean on success or false on not
//void Game::makeMove(Move& move) {
//    Piece* piece = m_board.getPiece(move.m_from);
//    if (piece) {
//        if (m_turn == piece->getColor()) {
//            // after each move need to check if put a king in check and or if checkmate was performed
//            // maybe create an attacked board -> binary 2d array stating if the location is attacked or not
//            // then check if the king square is attacked to see if in check
//            // can only make a move that isn't a king move if king isn't in check
//            // check if it is a valid move here as well
//            // check what type of move it is here looking for certain cases
//            // looking for en pessant, promotion, castling, or standard
//            // could maybe make a member function on all the pieces that returns the type of move
//            // call the member function moveType(Position& from, Position& to)
//            // if gonna make a validMoves() function for Piece -> could call it every time
//            // validMoves should create an internal set of current valid moves that could be checked against
//            // setPiece is called -> would just need to also use setPiece during initialize Board
//            // need to invalid to the move types
//            // then could set up a switch here with the default for standard moves
//            // then other cases for the other types of moves because some require moving two pieces
//            // need to handle promotions - but probably will have board handle this
//
//
//            m_board.removePiece(move.m_from);
//            m_board.setPiece(piece, move.m_to);
//            piece->setPos(move.m_to);
//
//            if (!piece->hasMoved()) {
//                piece->setMoved(true);
//            }
//            m_turn = m_turn == Piece::Color::WHITE ? Piece::Color::BLACK : Piece::Color::WHITE;
//        }
//        else {
//            std::cout << "That is not your piece or not your turn..." << std::endl;
//        }
//
//    }
//    else {
//        std::cout << "No piece on that square..." << std::endl;
//    }
//}

int main() {

    test_002();
    //test_001();
	return 0;
}
