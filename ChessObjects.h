#pragma once
#include <vector>
#include <string>

struct Position {

	Position(int rowVal, int colVal);

	~Position() = default;

	int row;
	int col;
};

class Piece {

public:
	enum class Color { WHITE, BLACK };

	Piece(Color color, std::string ident);

	virtual ~Piece() = default;

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

class Board {
public:
	Board(int rows, int cols);

	virtual ~Board();

	Piece* getPiece(const Position& pos);

	void setPiece(Piece* piece, const Position& pos);

	void removePiece(const Position& pos);

	void initializeBoard();

	void printBoard() const;

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

class Player {
public:

protected:

private:

};

class Move {
public:
	enum class MoveType { STD, CASTLE, ENPES };

protected:
	Piece* piece;
	Position from;
	Position to;
	MoveType moveType;

};

class Pawn : public Piece {

public:
	// Constructor for Pawn
	Pawn(Color color, std::string ident = "P");

};

class Bishop : public Piece {

public:
	// Constructor for Pawn
	Bishop(Color color, std::string ident = "B");

};

class Knight : public Piece {

public:
	// Constructor for Knight
	Knight(Color color, std::string ident = "N");

};

class Rook : public Piece {

public:
	// Constructor for Rook
	Rook(Color color, std::string ident = "R");

};

class King : public Piece {

public:
	// Constructor for King
	King(Color color, std::string ident = "K");

};

class Queen : public Piece {

public:
	// Constructor for Queen
	Queen(Color color, std::string ident = "Q");

};