#include <pybind11/pybind11.h>
#include <vector>
#include "ChessObjects.h"



namespace py = pybind11;

void playGame(Game& game) {
    game.playGame();
}

PYBIND11_MODULE(chess, m) {
    py::class_<Game>(m, "Game")
        .def(py::init<>())  // Default constructor for Game
        .def("play_game", &playGame);

    py::class_<Player>(m, "Player")
        .def("get_color", &Player::getColor);

    py::class_<Piece>(m, "Piece")
        .def("get_color", &Piece::getColor);

    py::class_<Move>(m, "Move")
        .def(py::init<const Position&, const Position&>())
        .def("get_from", &Move::getFrom)
        .def("get_to", &Move::getTo);
}

//
//
//cmake_minimum_required(VERSION 3.12)
//project(chess_game)
//
//set(CMAKE_CXX_STANDARD 14)
//
//# Path to pybind11(either from system package or from source)
//find_package(pybind11 REQUIRED)
//
//# Add your source files here
//add_library(chess MODULE bindings.cpp game.cpp)
//
//target_link_libraries(chess PRIVATE pybind11::module)
//
//
//mkdir build
//cd build
//cmake ..
//make

//import chess
//
//# Create a Game object
//game = chess.Game()
//
//# Play the game using C++ logic
//game.play_game()

//from flask import Flask, jsonify
//import chess
//
//app = Flask(__name__)
//
//@app.route('/start_game', methods = ['GET'])
//def start_game() :
//    game = chess.Game()
//    game.play_game()  # Call the C++ game logic
//    return jsonify({ "status": "Game Over" })
//
//    if __name__ == '__main__':
//app.run(debug = True)