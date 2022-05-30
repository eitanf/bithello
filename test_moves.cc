/*
 * Unit tests for legal move functionality
 * Keep in mind that the bit representation of positions is such the the LSB
 * stands for the top left corner, and the MSB for bottom right.
 */

#define CATCH_CONFIG_MAIN

#include "board.hh"
#include "catch.hh"
#include "moves.hh"
#include "player.hh"
#include "scan.hh"

using namespace Othello;
using enum Color;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "legal_moves finds all possible moves -- dense", "[moves]" ) {
  Board b({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "ooo.ooxo",
    ".o.oooox",
    ".x.ox.ox",
    "xo.xo.x."
    });

  SECTION( "L2R" ) {
    REQUIRE(legal_moves(L_START, L2R, b.dark_, b.light_) ==
     0b00100100'00000000'00000000'00000000'00000000'00000000'00000000'00000000);
    REQUIRE(legal_moves(L_START, L2R, b.light_, b.dark_) ==
     0b00000000'00100000'00000000'00000000'00000000'10010000'01000000'00000000);
  }

  SECTION( "R2L" ) {
    REQUIRE(legal_moves(R_START, R2L, b.dark_, b.light_) ==
     0b00000000'00100100'00000100'00001000'00000000'00010000'00000010'00000010);
    REQUIRE(legal_moves(R_START, R2L, b.light_, b.dark_) ==
     0b00000100'00000000'00000000'00000000'00001000'00000000'00000000'00000000);
  }

  SECTION( "T2B" ) {
    REQUIRE(legal_moves(T_START, T2B, b.dark_, b.light_) ==
     0b00000000'00100000'00000101'00000000'00000000'00010000'00000000'00000000);
    REQUIRE(legal_moves(T_START, T2B, b.light_, b.dark_) ==
     0b10000000'00000000'00000000'00000000'00001000'00000000'00000000'00000000);
  }

  SECTION( "B2T" ) {
    REQUIRE(legal_moves(B_START, B2T, b.dark_, b.light_) ==
     0b00000000'00000000'00000000'00001000'00000010'10000000'00000000'00000001);
    REQUIRE(legal_moves(B_START, B2T, b.light_, b.dark_) ==
     0b00000000'00000000'00000000'00000000'00000000'00010000'01000000'00000000);
  }

  SECTION( "BL2TR" ) {
    REQUIRE(legal_moves(BL_START, BL2TR, b.dark_, b.light_) == setpos(1, 6));
    REQUIRE(legal_moves(BL_START, BL2TR, b.light_, b.dark_) == setpos(2, 7));
  }

  SECTION( "BR2TL" ) {
    REQUIRE(legal_moves(BR_START, BR2TL, b.dark_, b.light_) ==
     0b00000000'00000000'00000000'00000000'00000010'00010000'00000000'00000010);
    REQUIRE(!legal_moves(BR_START, BR2TL, b.light_, b.dark_));
  }

  SECTION( "TR2BL" ) {
    REQUIRE(legal_moves(TR_START, TR2BL, b.dark_, b.light_) ==
     0b00100100'00000100'00000001'00000000'00000010'00000000'00000000'00000000);
    REQUIRE(legal_moves(TR_START, TR2BL, b.light_, b.dark_) ==
     0b00000000'00000000'00000000'00001000'00000000'00010000'00000000'00000000);
  }

  SECTION( "TL2BR" ) {
    REQUIRE(legal_moves(TL_START, TL2BR, b.dark_, b.light_) == setpos(5, 2));
    REQUIRE(legal_moves(TL_START, TL2BR, b.light_, b.dark_) ==
     0b00100000'00000000'00000000'00001000'00000000'00000000'00000000'00000000);
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "legal_moves finds all possible moves -- triangular", "[moves]" ) {
  Board b({
    "xxxxxxxx",
    "xoooooo.",
    "xooooo..",
    "xoooo...",
    "xooo....",
    "xoo.....",
    "xo......",
    "x.......",
    });

  SECTION( "L2R" ) {
    REQUIRE(!legal_moves(R_START, R2L, b.dark_, b.light_));
    REQUIRE(!legal_moves(R_START, R2L, b.light_, b.dark_));
    REQUIRE(!legal_moves(B_START, B2T, b.dark_, b.light_));
    REQUIRE(!legal_moves(B_START, B2T, b.light_, b.dark_));
    REQUIRE(!legal_moves(BL_START, BL2TR, b.dark_, b.light_));
    REQUIRE(!legal_moves(BL_START, BL2TR, b.light_, b.dark_));
    REQUIRE(!legal_moves(BR_START, BR2TL, b.dark_, b.light_));
    REQUIRE(!legal_moves(BR_START, BR2TL, b.light_, b.dark_));
    REQUIRE(!legal_moves(TR_START, TR2BL, b.dark_, b.light_));
    REQUIRE(!legal_moves(TR_START, TR2BL, b.light_, b.dark_));

    REQUIRE(legal_moves(L_START, L2R, b.dark_, b.light_) ==
     0b00000000'00000100'00001000'00010000'00100000'01000000'10000000'00000000);
    REQUIRE(!legal_moves(L_START, L2R, b.light_, b.dark_));

    REQUIRE(legal_moves(T_START, T2B, b.dark_, b.light_) ==
     0b00000010'00000100'00001000'00010000'00100000'01000000'00000000'00000000);
    REQUIRE(!legal_moves(T_START, T2B, b.light_, b.dark_));

    REQUIRE(legal_moves(TL_START, TL2BR, b.dark_, b.light_) ==
     0b00000100'00001100'00011000'00110000'01100000'11000000'00000000'00000000);
    REQUIRE(!legal_moves(TL_START, TL2BR, b.light_, b.dark_));
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "legal_moves stops at board borders", "[moves]" ) {
  SECTION( "horizontal and vertical" ) {
    Board b({
        "ooo..xxx",
        "oo....xx",
        "o......x",
        "........",
        "........",
        "x......o",
        "xx....oo",
        "xxx..ooo"
      });
    REQUIRE(all_legal_moves(b, DARK) == 0ull);
    REQUIRE(all_legal_moves(b, LIGHT) == 0ull);
  }

  SECTION ( "diagonal" ) {
    Board b({
        "xxxx.xxx",
        "x......x",
        "x......x",
        "........",
        ".......x",
        "o.......",
        "o......o",
        "oooo.ooo"
      });
    REQUIRE(all_legal_moves(b, DARK) == 0ull);
    REQUIRE(all_legal_moves(b, LIGHT) == 0ull);
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - R2L", "[moves]" ) {
  Board b_in({  ".x......" });  // b_in.dark_ == 2
  Board b_out({ "x......." });

    REQUIRE((inside(R2L) & b_in.dark_) != 0);
    REQUIRE((inside(R2L) & b_out.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - L2R", "[moves]" ) {
  Board b_in({  "......x." });
  Board b_out({ ".......x" });

    REQUIRE((inside(L2R) & b_in.dark_) != 0);
    REQUIRE((inside(L2R) & b_out.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - B2T", "[moves]" ) {
  Board b_in({ "", "x" });
  Board b_out({ "x" });

    REQUIRE((inside(B2T) & b_in.dark_) != 0);
    REQUIRE((inside(B2T) & b_out.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - T2B", "[moves]" ) {
  Board b_in({ "", "", "", "", "", "", "x", "" });
  Board b_out({ "", "", "", "", "", "", "", "x", });

    REQUIRE((inside(T2B) & b_in.dark_) != 0);
    REQUIRE((inside(T2B) & b_out.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - TL2BR", "[moves]" ) {
  Board b_in({ "" , "", "", "", "", "", "x", "" });
  Board b_out1({ "", "", "", "", "", "", ".......x", "" });
  Board b_out2({ "", "", "", "", "", "", "", ".......x", });
  Board b_out3({ "", "", "", "", "", "", "", "......x.", });

    REQUIRE((inside(TL2BR) & b_in.dark_) != 0);
    REQUIRE((inside(TL2BR) & b_out1.dark_) == 0);
    REQUIRE((inside(TL2BR) & b_out2.dark_) == 0);
    REQUIRE((inside(TL2BR) & b_out3.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - TR2BL", "[moves]" ) {
  Board b_in({ "" , "", "", "", "", "", ".x", "" });
  Board b_out1({ "", "", "", "", "", "", "x.......", "" });
  Board b_out2({ "", "", "", "", "", "", "", "x.......", });
  Board b_out3({ "", "", "", "", "", "", "", "......x.", });

    REQUIRE((inside(TR2BL) & b_in.dark_) != 0);
    REQUIRE((inside(TR2BL) & b_out1.dark_) == 0);
    REQUIRE((inside(TR2BL) & b_out2.dark_) == 0);
    REQUIRE((inside(TR2BL) & b_out3.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - BL2TR", "[moves]" ) {
  Board b_in({ "" , "", "", "", "", "", "x", "" });
  Board b_out1({ "", "", "", "", "", "", ".......x", "" });
  Board b_out2({ "", "", "", "", "", "", "", ".......x", });
  Board b_out3({ "...x", "", "", "", "", "", "", "........", });

    REQUIRE((inside(BL2TR) & b_in.dark_) != 0);
    REQUIRE((inside(BL2TR) & b_out1.dark_) == 0);
    REQUIRE((inside(BL2TR) & b_out2.dark_) == 0);
    REQUIRE((inside(BL2TR) & b_out3.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Inside board computes correctly - BR2TL", "[moves]" ) {
  Board b_in({ "" , "", "", "", "", "", ".x", "" });
  Board b_out1({ "", "", "", "", "", "", "x.......", "" });
  Board b_out2({ "x", "", "", "", "", "", "", "........", });
  Board b_out3({ "", "x", "", "", "", "", "", "........", });

    REQUIRE((inside(BR2TL) & b_in.dark_) != 0);
    REQUIRE((inside(BR2TL) & b_out1.dark_) == 0);
    REQUIRE((inside(BR2TL) & b_out2.dark_) == 0);
    REQUIRE((inside(BR2TL) & b_out3.dark_) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "flip_bits flips exactly the correct bits -- dense", "[moves]" ) {
  Board b({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "ooo.ooxo",
    ".o.oooox",
    ".x.ox.ox",
    "xo.xo.x."
    });

  SECTION( "Row 0" ) {
    REQUIRE(find_flipped(setpos(0, 0), b.dark_, b.light_, T2B) == setpos(1, 0));

    REQUIRE(find_flipped(setpos(0, 1), b.dark_, b.light_, L2R) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00001100);
  }

  SECTION ( "Row 1" ) {
    REQUIRE(find_flipped(setpos(1, 1), b.dark_, b.light_, L2R) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00011100'00000000);

    REQUIRE(find_flipped(setpos(1, 6), b.dark_, b.light_, TR2BL) == setpos(2, 5));
    REQUIRE(find_flipped(setpos(1, 6), b.light_, b.dark_, R2L) == setpos(1, 5));
    REQUIRE(find_flipped(setpos(1, 6), b.light_, b.dark_, T2B) ==
      0b00000000'00000000'00000000'01000000'01000000'01000000'00000000'00000000);
  }

  SECTION( "Row 2" ) {
    REQUIRE(find_flipped(setpos(2, 4), b.dark_, b.light_, L2R) == setpos(2, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.dark_, b.light_, TL2BR) == setpos(3, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.dark_, b.light_, B2T) == setpos(1, 4));
    REQUIRE(find_flipped(setpos(2, 4), b.light_, b.dark_, BL2TR) == setpos(1, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.light_, b.dark_, R2L) == setpos(2, 3));
    REQUIRE(find_flipped(setpos(2, 4), b.light_, b.dark_, T2B) == setpos(3, 4));

    REQUIRE(find_flipped(setpos(2, 7), b.dark_, b.light_, T2B) ==
      0b00000000'00000000'00000000'10000000'10000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(2, 7), b.light_, b.dark_, TR2BL) == setpos(3, 6));
  }

  SECTION( "Row 3" ) {
    REQUIRE(find_flipped(setpos(3, 1), b.dark_, b.light_, BL2TR) ==
       0b00000000'00000000'00000000'00000000'00000000'00000100'00001000'00000000);
    REQUIRE(find_flipped(setpos(3, 1), b.dark_, b.light_, T2B) ==
       0b00000000'00000000'00000010'00000010'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(3, 1), b.dark_, b.light_, TL2BR) ==
       0b00000000'00000000'00001000'00000100'00000000'00000000'00000000'00000000);

    REQUIRE(find_flipped(setpos(3, 3), b.light_, b.dark_, B2T) == setpos(2, 3));
    REQUIRE(find_flipped(setpos(3, 3), b.light_, b.dark_, L2R) == setpos(3, 4));
  }

  SECTION( "Row 4" ) {
    REQUIRE(find_flipped(setpos(4, 3), b.dark_, b.light_, L2R) ==
       0b00000000'00000000'00000000'00110000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(4, 3), b.dark_, b.light_, T2B) ==
       0b00000000'00001000'00001000'00000000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(4, 3), b.light_, b.dark_, BL2TR) == setpos(3, 4));
    REQUIRE(find_flipped(setpos(4, 3), b.light_, b.dark_, BR2TL) == setpos(3, 2));
  }

  SECTION( "Row 5" ) {
    REQUIRE(find_flipped(setpos(5, 0), b.dark_, b.light_, B2T) == setpos(4, 0));
    REQUIRE(find_flipped(setpos(5, 0), b.dark_, b.light_, BL2TR) == setpos(4, 1));

    REQUIRE(find_flipped(setpos(5, 2), b.dark_, b.light_, B2T) == setpos(4, 2));
    REQUIRE(find_flipped(setpos(5, 2), b.dark_, b.light_, BR2TL) == setpos(4, 1));
    REQUIRE(find_flipped(setpos(5, 2), b.dark_, b.light_, L2R) ==
       0b00000000'00000000'01111000'00000000'00000000'00000000'00000000'00000000);
  }

  SECTION( "Row 6" ) {
    REQUIRE(find_flipped(setpos(6, 2), b.dark_, b.light_, L2R) == setpos(6, 3));
    REQUIRE(find_flipped(setpos(6, 2), b.dark_, b.light_, BL2TR) ==
       0b00000000'00000000'00001000'00010000'00100000'00000000'00000000'00000000);

    REQUIRE(find_flipped(setpos(6, 5), b.dark_, b.light_, L2R) == setpos(6, 6));
    REQUIRE(find_flipped(setpos(6, 5), b.dark_, b.light_, B2T) ==
       0b00000000'00000000'00100000'00100000'00100000'00100000'00000000'00000000);
    REQUIRE(find_flipped(setpos(6, 5), b.light_, b.dark_, R2L) == setpos(6, 4));
  }

  SECTION( "Row 7" ) {
    REQUIRE(find_flipped(setpos(7, 2), b.dark_, b.light_, R2L) == setpos(7, 1));
    REQUIRE(find_flipped(setpos(7, 2), b.dark_, b.light_, BL2TR) ==
       0b00000000'00001000'00010000'00100000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(7, 2), b.light_, b.dark_, L2R) == setpos(7, 3));

    REQUIRE(find_flipped(setpos(7, 5), b.dark_, b.light_, R2L) == setpos(7, 4));
    REQUIRE(find_flipped(setpos(7, 5), b.dark_, b.light_, BL2TR) == setpos(6, 6));
    REQUIRE(find_flipped(setpos(7, 5), b.light_, b.dark_, BR2TL) == setpos(6, 4));
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "effect_moves changes all flipped bits", "[moves]" ) {
  Board b({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "ooo.ooxo",
    ".o.oooox",
    ".x.ox.ox",
    "xo.xo.x."
    });

  REQUIRE(effect_move(b, LIGHT, setpos(2, 4)) ==
    Board({
    "..ooxoox",
    "o.oooo.x",
    "xooooox.",
    "x.x.ooxo",
    "ooo.ooxo",
    ".o.oooox",
    ".x.ox.ox",
    "xo.xo.x."}));

  REQUIRE(effect_move(b, DARK, setpos(4, 3)) ==
    Board({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "oooxxxxo",
    ".o.xooox",
    ".x.xx.ox",
    "xo.xo.x."}));

  REQUIRE(effect_move(b, DARK, setpos(7, 2)) ==
    Board({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "ooo.oxxo",
    ".o.oxoox",
    ".x.xx.ox",
    "xxxxo.x."}));
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "find_flipped finds nothing when no legal moves", "[moves]" ) {
  SECTION( "horizontal and vertical" ) {
    Board b({
        "ooo..xxx",
        "oo....xx",
        "o......x",
        "........",
        "........",
        "x......o",
        "xx....oo",
        "xxx..ooo"
      });

    REQUIRE(all_flipped(b.dark_, b.light_, setpos(1, 2)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(1, 2)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(2, 6)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(2, 6)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(7, 3)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(7, 3)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(7, 4)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(7, 4)) == 0);
  }

  SECTION ( "diagonal" ) {
    Board b({
        "xxxx.xxx",
        "x......x",
        "x......x",
        "........",
        ".......x",
        "o.......",
        "o......o",
        "oooo.ooo"
      });

    REQUIRE(all_flipped(b.dark_, b.light_, setpos(1, 2)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(1, 2)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(2, 6)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(2, 6)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(5, 7)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(5, 7)) == 0);
    REQUIRE(all_flipped(b.dark_, b.light_, setpos(6, 2)) == 0);
    REQUIRE(all_flipped(b.light_, b.dark_, setpos(6, 2)) == 0);
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "find_flipped finds exactly the correct bits -- corner", "[moves]" ) {
  Board b({
    "...xxxxx",
    ".x.xxxxx",
    "..xxoxx.",
    "..oooxoo",
    "...ox.x.",
    "..o.x...",
    "....x...",
    "....x..."
    });

    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, L2R) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, R2L) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, T2B) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, B2T) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, BR2TL) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, BL2TR) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, TR2BL) == 0);
    REQUIRE(find_flipped(setpos(0, 0), b.light_, b.dark_, TL2BR) ==
      0b00000000'00000000'00000000'00000000'00000000'00000100'00000010'00000000);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Computes the correct tile difference", "[moves]" ) {
  const Board board({
      "oxxxxxxo",
      "oxxxxxxo",
      "xxxxxxxx",
      "xxxxxxxx",
      "oooooooo",
      "oooooooo",
      "ooooooxo",
      "xoooooox",
      });

  // A dummy player class for testing purposes that does nothing:
  struct DummyPlayer : public Player {
    DummyPlayer(Color color) : Player(color) {}
    ~DummyPlayer() = default;
    void display_board(Board) const {}
    bits_t get_move(Board, bits_t) const { return 0; }
    void notify_move(Board, bits_t) const {}
    void game_over(Board) const {}
  };

  auto pb = player_ptr_t(new DummyPlayer(DARK));
  auto pw = player_ptr_t(new DummyPlayer(LIGHT));

  REQUIRE(play_game(board, pb, pw) == -2);
  REQUIRE(play_game(board, pw, pb) == -2);
}
