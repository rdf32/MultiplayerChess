#pragma once
#include <vector>
#include <string>
#include <utility>
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

	Position(int rowVal, int colVal);
	~Position() = default;

	int row;
	int col;
};

struct Move {
	Position m_from;   // Position from where the piece is moving
	Position m_to;     // Position where the piece is moving to

	// Constructor definition
	Move(Position to, Position from);

	// Default destructor (optional)
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

	virtual std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove = nullptr) = 0;

	virtual PieceType getType() const = 0;

	Color getColor() const;

	std::string getIdent() const;

	bool hasMoved() const;
	
	void setMoved(bool moved);

	Position getPos() const;

	void setPos(const Position& pos);


protected:
	Color m_color;
	Position m_pos;
	bool m_moved;
	std::string m_ident;
};

class Pawn : public Piece {

public:
	// Constructor for Pawn
	Pawn(Color color, std::string ident = "P");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class King : public Piece {

public:
	// Constructor for King
	King(Color color, std::string ident = "K");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class Queen : public Piece {

public:
	// Constructor for Queen
	Queen(Color color, std::string ident = "Q");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class Rook : public Piece {

public:
	// Constructor for Rook
	Rook(Color color, std::string ident = "R");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class Bishop : public Piece {

public:
	// Constructor for Pawn
	Bishop(Color color, std::string ident = "B");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class Knight : public Piece {

public:
	// Constructor for Knight
	Knight(Color color, std::string ident = "N");
	std::unordered_set<std::pair<int, int>, pair_hash> validMoves(std::vector<std::vector<Piece*>>& state, Move* lastMove) override;
	PieceType getType() const override;
};

class Board {
public:
	Board(int rows, int cols);

	virtual ~Board();

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

class Game {
public:
	Game();

	virtual ~Game() = default;

	void makeMove(Position& from, Position& to);

	Board m_board;


private:
	Piece::Color m_turn;
	//std::vector<Move*> history;
};



//class Player {
//public:
//
//protected:
//
//private:
//
//};
//

