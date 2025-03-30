#pragma once
#include <vector>
#include <string>
#include <utility>
#include <queue> 
#include <deque>
#include <functional>
#include <unordered_set>

struct pair_hash {
	template <typename T1, typename T2>
	std::size_t operator ()(const std::pair<T1, T2>& p) const {
		auto h1 = std::hash<T1>{}(p.first);  // Hash of the first element
		auto h2 = std::hash<T2>{}(p.second); // Hash of the second element

		// Combine the two hash values (this technique is commonly used)
		return h1 ^ (h2 << 1);  // XOR the hashes and shift the second hash
	}
};

struct Position {

	int row;
	int col;

	Position(int rowVal, int colVal);
	~Position() = default;

	bool operator==(const Position& other) const;

	friend std::ostream& operator<<(std::ostream& os, const Position& pos);

};

struct position_hash {
	size_t operator()(const Position& pos) const;
};

struct PositionType {

	enum class MoveType { STND, CAPT, QCASTLE, KCASTLE, PROM, ENPASS };
	std::pair<int, int> pair;
	MoveType mtype;

	PositionType(std::pair<int, int> pair, MoveType mtype);
	~PositionType() = default;

	bool operator==(const PositionType& other) const;

};
struct positionType_hash {
	size_t operator()(const PositionType& posType) const;
};

struct Move {
	Position m_from;   // Position from where the piece is moving
	Position m_to;     // Position where the piece is moving to

	Move(Position from, Position to);
	~Move() = default;
};


class Piece {

public:
	enum class Color { WHITE, BLACK };

	struct PieceType {
		enum Type { PIECE, PAWN, KING, QUEEN, ROOK, KNIGHT, BISHOP } type;
	};

	Piece(Color color, std::string ident);

	virtual ~Piece() = default;

	virtual std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove = nullptr) = 0;

	virtual PieceType getType() const = 0;

	Color getColor() const;

	std::string getIdent() const;

	bool hasMoved() const;
	
	void setMoved(bool moved);

	Position getPos() const;

	void setPos(const Position& pos);

	bool isDefended(const std::vector<std::vector<Piece*>>& state);

	virtual std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) = 0;


protected:
	Color m_color;
	Position m_pos;
	bool m_moved;
	std::string m_ident;
	bool m_defended;
};

class Pawn : public Piece {

public:
	// Constructor for Pawn
	Pawn(Color color, std::string ident = "P");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) override;
	PieceType getType() const override;
};

class King : public Piece {

public:
	// Constructor for King
	King(Color color, std::string ident = "K");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(
		const std::vector<std::vector<Piece*>>& board, const Position& pos) override {
		// Empty implementation for King
		return std::unordered_set<PositionType, positionType_hash>();
	}
	PieceType getType() const override;
};

class Queen : public Piece {

public:
	// Constructor for Queen
	Queen(Color color, std::string ident = "Q");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) override;
	PieceType getType() const override;
};

class Rook : public Piece {

public:
	// Constructor for Rook
	Rook(Color color, std::string ident = "R");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) override;
	PieceType getType() const override;
};

class Bishop : public Piece {

public:
	// Constructor for Pawn
	Bishop(Color color, std::string ident = "B");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) override;
	PieceType getType() const override;
};

class Knight : public Piece {

public:
	// Constructor for Knight
	Knight(Color color, std::string ident = "N");
	std::unordered_set<PositionType, positionType_hash> validMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	std::unordered_set<PositionType, positionType_hash> lineOfAttack(const std::vector<std::vector<Piece*>>& state, const Position& kingPos) override;
	PieceType getType() const override;
};

class Board {
public:
	Board(int rows, int cols);

	virtual ~Board();

	void createPiece(Piece::PieceType::Type ptype, Piece::Color pcolor, Position& pos);

	Piece* getPiece(const Position& pos);

	void setPiece(Piece* piece, const Position& pos);

	void removePiece(const Position& pos);

	void initializeBoard();

	void printBoard() const;

	std::vector<std::vector<Piece*>> getState();

private:
	int m_rows, m_cols;
	std::vector<std::vector<Piece*>> m_state;

};

class Player {
public:

	Player();
	~Player();
	Piece::Color getColor();
	void setColor(Piece::Color color);
	Position getKingpos();
	void setKingpos(Position& pos);
	std::vector<Piece*> attackingPieces(const std::vector<std::vector<Piece*>>& state);
	std::unordered_map<Position, std::unordered_set<PositionType, positionType_hash>, position_hash> legalMoves(const std::vector<std::vector<Piece*>>& state, Move* lastMove);
	std::vector<Piece*> capturedPieces;
	bool putsKingInCheck(const std::vector<std::vector<Piece*>>& state, const Move& move);


private:
	Piece::Color m_color;
	std::queue<Move*> moveQueue;
	Position m_kingPos;
};

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

