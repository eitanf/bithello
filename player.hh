/*
 * Base class for any player, either human (with a UI), or computer (with an AI).
 */

#pragma once

#include "bits.hh"
#include "board.hh"

#include <memory>

namespace Othello {

// Which player is currently playing?
enum class Color { BLACK = 0, WHITE = 1 };

// Flip color to opponent's:
inline constexpr Color
opponent_of(Color color) { return Color(!bool(color)); }

// Abstract base class for all player types:
class Player {
 public:
  Player(Color color) : color_(color) {}
  virtual ~Player() = default;

  // Display the current state of the board from my player's perspective:
  virtual void display_board(Board) const = 0;

  // Ask for the player's choice of move from a bitmap of valid moves
  // Can return 0 to request undo (human player).
  virtual bits_t get_move(Board b, bits_t moves) const = 0;

  // Notify player of the opponent's latest move:
  virtual void notify_move(Board before, bits_t pos) const = 0;

  // Notify player that the game is over, present final board:
  virtual void game_over(Board b) const = 0;

  const Color color_;
};

using player_ptr_t = std::shared_ptr<Player>;

} // namespace
