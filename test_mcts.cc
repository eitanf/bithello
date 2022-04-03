/*
 * Unit tests for MCTS functionality
 * Keep in mind that the bit representation of positions is such the the LSB
 * stands for the top left corner, and the MSB for bottom right.
 */

#define CATCH_CONFIG_MAIN

#include "mcts_node.hh"
#include "mcts_player.hh"
#include "moves.hh"
#include "stop.hh"

#include "catch.hh"

using namespace Othello;

auto b_odds = [](auto node) { return node.win_odds(Color::BLACK); };
auto w_odds = [](auto node) { return node.win_odds(Color::WHITE); };

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Size of MTCS node is a power of two", "[MCTS]" ) {
  REQUIRE(bits_set(sizeof(MCTSNode)) == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Odds at initial node are zero", "[MCTS]" ) {
  auto node = MCTSNode(Board(0, 0), Color::BLACK);
  REQUIRE(b_odds(node) == 0.);
  REQUIRE(w_odds(node) == 0.);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Odds after win are nonzero", "[MCTS]" ) {
  auto node = MCTSNode(Board(0, 0), Color::BLACK);
  node.count_wins(1, 0);
  REQUIRE(w_odds(node) == 0.);
  REQUIRE(b_odds(node) > 0.);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Odds at start are equal", "[MCTS]" ) {
  const Board board({ "", "", "", "...ox", "...xo" });
  auto node = MCTSNode(board, Color::BLACK);
  REQUIRE(b_odds(node) == Approx(w_odds(node)).epsilon(0.00001));
}

////////////////////////////////////////////////////////////////////////////////
// For this starting board: both players win if they pick the top-right corner first
TEST_CASE( "Picks an always-winning move over an always-losing move", "[MCTS]" ) {
  const Board board({
      "oxxxxxx.",
      "oxxxxxxo",
      "xxxxxxxx",
      "xxxxxxxx",
      "oooxoooo",
      "ooxooooo",
      "ooooooxo",
      "xoooooo.",
      });
  StopByMoves stopper;

  MCTSPlayer pb(Color::BLACK, stopper);
  auto moves = all_legal_moves(board, Color::BLACK);
  REQUIRE(pb.get_move(board, moves) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000000);

  MCTSPlayer pw(Color::WHITE, stopper);
  moves = all_legal_moves(board, Color::WHITE);
  REQUIRE(pb.get_move(board, moves) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000000);
}
