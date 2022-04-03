/*
 * "AI" implementation of a computer player that just picks a random move
 */

#pragma once

#include "player.hh"

namespace Othello {

class RandomPlayer : public Player {
 public:
   // If seed is zero, some random value will be picked.
  RandomPlayer(Color color, uint64_t seed = 0) : Player(color), seed_(seed) {}
  virtual ~RandomPlayer() = default;

  virtual void display_board(Board) const {};

  // Fetch a random move out of the valid, legal moves:
  virtual bits_t get_move(Board b, bits_t moves) const;

  virtual void notify_move(Board, bits_t) const {};

  virtual void game_over(Board) const {};

 private:
  uint64_t seed_;
};

} // namespace
