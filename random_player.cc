/*
 * "AI" implementation of a computer player that just picks a random move
 */

#include "bits.hh"
#include "random_player.hh"

namespace Othello {

RandomPlayer::RandomPlayer(Color color, uint64_t seed)
 : Player(color),
   rstate_(seed? seed : rand())
{
}

// PRNG from https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
uint64_t
RandomPlayer::lehmer64() const {
  /*
  rstate_ *= 0xda942042e4dd58b5;
  return rstate_ >> 64;
  */
    rstate_ += 0x60bee2bee120fc15;
  __uint128_t tmp;
  tmp = (__uint128_t) rstate_ * 0xa3b195354a39b70d;
  uint64_t m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
  uint64_t m2 = (tmp >> 64) ^ tmp;
  return m2;
}

// Picks a random bit index, and increments it (modulu N2) until it finds
// that this bit number is set in legal moves, then returns it.
bits_t
RandomPlayer::get_move(Board, bits_t moves) const
{
  assert(moves);

  uint32_t idx = lehmer64() & (N2 - 1); // Take last 6 bits
  assert(idx < N2);

  while (!test(moves, idx)) {
    idx = (idx + 1) & (N2 - 1);
  }

  return set(0ull, idx);
}

} // namespace
