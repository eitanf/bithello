/*
 * "AI" implementation of a computer player that just picks a random move
 */

#include "bits.hh"
#include "random_player.hh"

namespace Othello {


RandomPlayer::RandomPlayer(Color color, uint64_t seed)
 : Player(color),
   generator_(seed? seed : std::random_device{}())
{
}

// Picks a random bit index, and increments it (modulu N2) until it finds
// that this bit number is set in legal moves, then returns it.
bits_t
RandomPlayer::get_move(Board, bits_t moves) const
{
  assert(moves);

  uint32_t idx = generator_();
  assert(idx < N2);

  while (!test(moves, idx)) {
    idx = (idx + 1) & (N2 - 1);
  }

  return set(0ull, idx);
}

} // namespace
