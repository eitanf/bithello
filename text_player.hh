/*
 * Text-based UI for a human Othello player
 */

#pragma once

#include <string>

#include "player.hh"

namespace Othello {

constexpr char SYMBOLS[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

class TextPlayer : public Player {
 public:
  TextPlayer(Color color) : Player(color) {}
  virtual ~TextPlayer() = default;

  virtual void display_board(Board) const;
  virtual bits_t get_move(Board, bits_t) const;

  // Notify player of the opponent's latest move:
  virtual void notify_move(Board before, bits_t pos) const;

  // Notify player that the game is over, present final board:
  virtual void game_over(Board b) const;

 private:
  // Display all valid moves with the text UI
  std::string display_moves(Board b, bits_t moves,
      const char charset[] = SYMBOLS) const;
};

} // namespace
