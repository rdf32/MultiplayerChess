#include <random>
#include <iostream>
#include <unordered_map>
#include "ChessObjects.h"

Player::Player(): m_color(Piece::Color::WHITE), m_kingPos(Position(0, 0)) {}

Player::~Player() {
    // Cleanup code (delete dynamically allocated pieces, etc.)
    for (Piece* piece : capturedPieces) {
        delete piece;
    }
}

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

                    for (const auto& pos : pieceMoves) {
                        Move move(from_pos, Position(pos.pair.first, pos.pair.second));
                        if (!putsKingInCheck(state, move)) {
                            legalPieceMoves[from_pos].insert(pos);
                        }
                    }
                }
            }
        }
    }
    else if (piecesAttacking.size() == 1) {

        std::unordered_set<std::pair<int, int>, pair_hash> attackedSquares;
        //std::unordered_set<std::pair<int, int>, pair_hash> attackingPiecePositions;

        for (const auto& piece : piecesAttacking) {
            //attackingPiecePositions.insert({ piece->getPos().row, piece->getPos().col });
            std::unordered_set<PositionType, positionType_hash> attPieceMoves = piece->lineOfAttack(state, kingPos);
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
                if (piece->getColor() == m_color) {
                    Position from_pos = piece->getPos();
                    std::unordered_set<PositionType, positionType_hash> pieceMoves = piece->validMoves(state, lastMove);
                    for (const auto& pos : pieceMoves) {
                        if (piece->getType().type == Piece::PieceType::KING) {
                            legalPieceMoves[from_pos].insert(pos);
                        }
                        else {
                            if (attackedSquares.find(pos.pair) != attackedSquares.end()) {
                                Move move(from_pos, Position(pos.pair.first, pos.pair.second));
                                if (!putsKingInCheck(state, move)) {
                                    legalPieceMoves[from_pos].insert(pos);
                                }
                                // if move puts king in check then skip
                            }
                            // or if piece can capture position of attacking piece, but only if that piece is not pinned
                            // checking if the piece is pinned maybe should be in the validMoves?????
                            // how to check if a piece is pinned???
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

bool Player::putsKingInCheck(const std::vector<std::vector<Piece*>>& state, const Move& move) {
    
    // make a copy for running the simulation
    std::vector<std::vector<Piece*>> modifiedState = state;

    Piece* piece = modifiedState[move.m_from.row][move.m_from.col];

    // simulate move
    modifiedState[move.m_to.row][move.m_to.col] = piece;
    modifiedState[move.m_from.row][move.m_from.col] = nullptr;

    std::vector<Piece*> piecesAttacking = attackingPieces(modifiedState);

    bool isKingInCheck;
    if (piecesAttacking.size() > 0) {
        isKingInCheck = true;
    }
    else {
        isKingInCheck = false;
    }

    // undo simulation
    modifiedState[move.m_to.row][move.m_to.col] = nullptr;
    modifiedState[move.m_from.row][move.m_from.col] = piece;

    return isKingInCheck;
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

        if (legalMoves.empty()) {
            std::cout << "Checkmate! " << (m_turn == Piece::Color::WHITE ? "Black " : "White ") << "wins!" << std::endl;
            break;
        }

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
    // could make this a switch of switches instead of if elses I am thinking
    Piece::Color pcolor = currentPlayer.getColor();
    Piece* piece = m_board.getPiece(move.m_from);
    m_board.removePiece(move.m_from);

    if (mtype == PositionType::MoveType::STND) {
        m_board.setPiece(piece, move.m_to);
        piece->setPos(move.m_to);
        if (!piece->hasMoved()) {
            piece->setMoved(true);
        }
        if (piece->getType().type == Piece::PieceType::KING) {
            currentPlayer.setKingpos(move.m_to);
        }
    }

    if (mtype == PositionType::MoveType::KCASTLE) {
        if (pcolor == Piece::Color::WHITE) {
            Piece* rook = m_board.getPiece(Position(7, 7));

            m_board.removePiece(Position(7, 7));
            m_board.setPiece(piece, Position(7, 6));
            m_board.setPiece(rook, Position(7, 5));

            piece->setPos(Position(7, 6));
            piece->setMoved(true);

            rook->setPos(Position(7, 5));
            rook->setMoved(true);

            currentPlayer.setKingpos(move.m_to);
        }
        else if (pcolor == Piece::Color::BLACK) {
            Piece* rook = m_board.getPiece(Position(0, 7));

            m_board.removePiece(Position(0, 7));
            m_board.setPiece(piece, Position(0, 6));
            m_board.setPiece(rook, Position(0, 5));

            piece->setPos(Position(0, 6));
            piece->setMoved(true);

            rook->setPos(Position(0, 5));
            rook->setMoved(true);

            currentPlayer.setKingpos(move.m_to);
        }
    }
    else if (mtype == PositionType::MoveType::QCASTLE) {
        if (pcolor == Piece::Color::WHITE) {
            Piece* rook = m_board.getPiece(Position(7, 0));

            m_board.removePiece(Position(7, 0));
            m_board.setPiece(piece, Position(7, 2));
            m_board.setPiece(rook, Position(7, 3));

            piece->setPos(Position(7, 2));
            piece->setMoved(true);

            rook->setPos(Position(7, 3));
            rook->setMoved(true);

            currentPlayer.setKingpos(move.m_to);
        }
        else if (pcolor == Piece::Color::BLACK) {
            Piece* rook = m_board.getPiece(Position(0, 0));

            m_board.removePiece(Position(0, 0));
            m_board.setPiece(piece, Position(0, 2));
            m_board.setPiece(rook, Position(0, 3));

            piece->setPos(Position(0, 2));
            piece->setMoved(true);

            rook->setPos(Position(0, 3));
            rook->setMoved(true);

            currentPlayer.setKingpos(move.m_to);
        }
    }
    else if (mtype == PositionType::MoveType::ENPASS) {
        m_board.setPiece(piece, move.m_to);
        piece->setPos(move.m_to);
        int direction = (pcolor == Piece::Color::WHITE) ? 1 : -1;
        Piece* capturedPiece = m_board.getPiece(Position(move.m_to.row + direction, move.m_to.col));
        currentPlayer.capturedPieces.push_back(capturedPiece);
        m_board.removePiece(Position(move.m_to.row + direction, move.m_to.col));
        piece->setMoved(true);

    }
    else if (mtype == PositionType::MoveType::PROM) {
        int promSelection;
        std::cout << "Select which piece to promote to { 0: Queen, 1: Knight, 2: Bishop, 3: Rook }" << std::endl;
        std::cin >> promSelection;
    
        switch (promSelection) {

        case 0:
            m_board.createPiece(Piece::PieceType::QUEEN, pcolor, move.m_to);
            delete piece;
            break;

        case 1:
            m_board.createPiece(Piece::PieceType::KNIGHT, pcolor, move.m_to);
            delete piece;
            break;

        case 2:
            m_board.createPiece(Piece::PieceType::BISHOP, pcolor, move.m_to);
            delete piece;
            break;

        case 3:
            m_board.createPiece(Piece::PieceType::ROOK, pcolor, move.m_to);
            delete piece;
            break;

        default:
            std::cerr << "Invalid piece type!" << std::endl;
            break;
        }
    } 
    else if (mtype == PositionType::MoveType::CAPT){
        Piece* capturedPiece = m_board.getPiece(move.m_to);
        currentPlayer.capturedPieces.push_back(capturedPiece);
        m_board.setPiece(piece, move.m_to);
        piece->setPos(move.m_to);
        if (!piece->hasMoved()) {
            piece->setMoved(true);
        }
        if (piece->getType().type == Piece::PieceType::KING) {
            currentPlayer.setKingpos(move.m_to);
        }
    }

    m_board.printBoard();
}


void test_002() {

    Player player_1;
    Player player_2;

    Game chessGame(player_1, player_2);
    chessGame.playGame();

}


int main() {

    test_002();
    //test_001();
	return 0;
}
