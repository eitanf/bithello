/*
 * "AI" implementation of a computer player that just picks a random move
 */

#include "bits.hh"
#include "random_player.hh"

#include <random>

namespace Othello {

// Picks a random bit index, and increments it (modulu N2) until it finds
// that this bit number is set in legal moves, then returns it.
bits_t
RandomPlayer::get_move(Board, bits_t moves) const
{
  static auto generator = seed_? std::default_random_engine(seed_)
                               : std::default_random_engine(std::random_device{}());
  static auto dist = std::uniform_int_distribution<idx_t>(0, N2 - 1);

  assert(moves);

  auto idx = dist(generator);
  assert(idx < N2);

  while (!test(moves, idx)) {
    idx = (idx + 1) % N2;
  }

  return set(0ull, idx);
}

} // namespace
