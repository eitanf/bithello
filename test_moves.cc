/*
 * Unit tests for legal move functionality
 * Keep in mind that the bit representation of positions is such the the LSB
 * stands for the top left corner, and the MSB for bottom right.
 */

#define CATCH_CONFIG_MAIN

#include "scan.hh"
#include "board.hh"
#include "moves.hh"
#include "catch.hh"

using namespace Othello;

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
    REQUIRE(legal_moves(L_START, L2R, b.black_, b.white_) ==
     0b00100100'00000000'00000000'00000000'00000000'00000000'00000000'00000000);
    REQUIRE(legal_moves(L_START, L2R, b.white_, b.black_) ==
     0b00000000'00100000'00000000'00000000'00000000'10010000'01000000'00000000);
  }

  SECTION( "R2L" ) {
    REQUIRE(legal_moves(R_START, R2L, b.black_, b.white_) ==
     0b00000000'00100100'00000100'00001000'00000000'00010000'00000010'00000010);
    REQUIRE(legal_moves(R_START, R2L, b.white_, b.black_) ==
     0b00000100'00000000'00000000'00000000'00001000'00000000'00000000'00000000);
  }

  SECTION( "T2B" ) {
    REQUIRE(legal_moves(T_START, T2B, b.black_, b.white_) ==
     0b00000000'00100000'00000101'00000000'00000000'00010000'00000000'00000000);
    REQUIRE(legal_moves(T_START, T2B, b.white_, b.black_) ==
     0b10000000'00000000'00000000'00000000'00001000'00000000'00000000'00000000);
  }

  SECTION( "B2T" ) {
    REQUIRE(legal_moves(B_START, B2T, b.black_, b.white_) ==
     0b00000000'00000000'00000000'00001000'00000010'10000000'00000000'00000001);
    REQUIRE(legal_moves(B_START, B2T, b.white_, b.black_) ==
     0b00000000'00000000'00000000'00000000'00000000'00010000'01000000'00000000);
  }

  SECTION( "BL2TR" ) {
    REQUIRE(legal_moves(BL_START, BL2TR, b.black_, b.white_) == setpos(1, 6));
    REQUIRE(legal_moves(BL_START, BL2TR, b.white_, b.black_) == setpos(2, 7));
  }

  SECTION( "BR2TL" ) {
    REQUIRE(legal_moves(BR_START, BR2TL, b.black_, b.white_) ==
     0b00000000'00000000'00000000'00000000'00000010'00010000'00000000'00000010);
    REQUIRE(!legal_moves(BR_START, BR2TL, b.white_, b.black_));
  }

  SECTION( "TR2BL" ) {
    REQUIRE(legal_moves(TR_START, TR2BL, b.black_, b.white_) ==
     0b00100100'00000100'00000001'00000000'00000010'00000000'00000000'00000000);
    REQUIRE(legal_moves(TR_START, TR2BL, b.white_, b.black_) ==
     0b00000000'00000000'00000000'00001000'00000000'00010000'00000000'00000000);
  }

  SECTION( "TL2BR" ) {
    REQUIRE(legal_moves(TL_START, TL2BR, b.black_, b.white_) == setpos(5, 2));
    REQUIRE(legal_moves(TL_START, TL2BR, b.white_, b.black_) ==
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
    REQUIRE(!legal_moves(R_START, R2L, b.black_, b.white_));
    REQUIRE(!legal_moves(R_START, R2L, b.white_, b.black_));
    REQUIRE(!legal_moves(B_START, B2T, b.black_, b.white_));
    REQUIRE(!legal_moves(B_START, B2T, b.white_, b.black_));
    REQUIRE(!legal_moves(BL_START, BL2TR, b.black_, b.white_));
    REQUIRE(!legal_moves(BL_START, BL2TR, b.white_, b.black_));
    REQUIRE(!legal_moves(BR_START, BR2TL, b.black_, b.white_));
    REQUIRE(!legal_moves(BR_START, BR2TL, b.white_, b.black_));
    REQUIRE(!legal_moves(TR_START, TR2BL, b.black_, b.white_));
    REQUIRE(!legal_moves(TR_START, TR2BL, b.white_, b.black_));

    REQUIRE(legal_moves(L_START, L2R, b.black_, b.white_) ==
     0b00000000'00000100'00001000'00010000'00100000'01000000'10000000'00000000);
    REQUIRE(!legal_moves(L_START, L2R, b.white_, b.black_));

    REQUIRE(legal_moves(T_START, T2B, b.black_, b.white_) ==
     0b00000010'00000100'00001000'00010000'00100000'01000000'00000000'00000000);
    REQUIRE(!legal_moves(T_START, T2B, b.white_, b.black_));

    REQUIRE(legal_moves(TL_START, TL2BR, b.black_, b.white_) ==
     0b00000100'00001100'00011000'00110000'01100000'11000000'00000000'00000000);
    REQUIRE(!legal_moves(TL_START, TL2BR, b.white_, b.black_));
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
    REQUIRE(all_legal_moves(b, Color::BLACK) == 0ull);
    REQUIRE(all_legal_moves(b, Color::WHITE) == 0ull);
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
    REQUIRE(all_legal_moves(b, Color::BLACK) == 0ull);
    REQUIRE(all_legal_moves(b, Color::WHITE) == 0ull);
  }
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
    REQUIRE(find_flipped(setpos(0, 0), b.black_, b.white_, T2B) == setpos(1, 0));

    REQUIRE(find_flipped(setpos(0, 1), b.black_, b.white_, L2R) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00001100);
  }

  SECTION ( "Row 1" ) {
    REQUIRE(find_flipped(setpos(1, 1), b.black_, b.white_, L2R) ==
      0b00000000'00000000'00000000'00000000'00000000'00000000'00011100'00000000);

    REQUIRE(find_flipped(setpos(1, 6), b.black_, b.white_, TR2BL) == setpos(2, 5));
    REQUIRE(find_flipped(setpos(1, 6), b.white_, b.black_, R2L) == setpos(1, 5));
    REQUIRE(find_flipped(setpos(1, 6), b.white_, b.black_, T2B) ==
      0b00000000'00000000'00000000'01000000'01000000'01000000'00000000'00000000);
  }

  SECTION( "Row 2" ) {
    REQUIRE(find_flipped(setpos(2, 4), b.black_, b.white_, L2R) == setpos(2, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.black_, b.white_, TL2BR) == setpos(3, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.black_, b.white_, B2T) == setpos(1, 4));
    REQUIRE(find_flipped(setpos(2, 4), b.white_, b.black_, BL2TR) == setpos(1, 5));
    REQUIRE(find_flipped(setpos(2, 4), b.white_, b.black_, R2L) == setpos(2, 3));
    REQUIRE(find_flipped(setpos(2, 4), b.white_, b.black_, T2B) == setpos(3, 4));

    REQUIRE(find_flipped(setpos(2, 7), b.black_, b.white_, T2B) ==
      0b00000000'00000000'00000000'10000000'10000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(2, 7), b.white_, b.black_, TR2BL) == setpos(3, 6));
  }

  SECTION( "Row 3" ) {
    REQUIRE(find_flipped(setpos(3, 1), b.black_, b.white_, BL2TR) ==
       0b00000000'00000000'00000000'00000000'00000000'00000100'00001000'00000000);
    REQUIRE(find_flipped(setpos(3, 1), b.black_, b.white_, T2B) ==
       0b00000000'00000000'00000010'00000010'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(3, 1), b.black_, b.white_, TL2BR) ==
       0b00000000'00000000'00001000'00000100'00000000'00000000'00000000'00000000);

    REQUIRE(find_flipped(setpos(3, 3), b.white_, b.black_, B2T) == setpos(2, 3));
    REQUIRE(find_flipped(setpos(3, 3), b.white_, b.black_, L2R) == setpos(3, 4));
  }

  SECTION( "Row 4" ) {
    REQUIRE(find_flipped(setpos(4, 3), b.black_, b.white_, L2R) ==
       0b00000000'00000000'00000000'00110000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(4, 3), b.black_, b.white_, T2B) ==
       0b00000000'00001000'00001000'00000000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(4, 3), b.white_, b.black_, BL2TR) == setpos(3, 4));
    REQUIRE(find_flipped(setpos(4, 3), b.white_, b.black_, BR2TL) == setpos(3, 2));
  }

  SECTION( "Row 5" ) {
    REQUIRE(find_flipped(setpos(5, 0), b.black_, b.white_, B2T) == setpos(4, 0));
    REQUIRE(find_flipped(setpos(5, 0), b.black_, b.white_, BL2TR) == setpos(4, 1));

    REQUIRE(find_flipped(setpos(5, 2), b.black_, b.white_, B2T) == setpos(4, 2));
    REQUIRE(find_flipped(setpos(5, 2), b.black_, b.white_, BR2TL) == setpos(4, 1));
    REQUIRE(find_flipped(setpos(5, 2), b.black_, b.white_, L2R) ==
       0b00000000'00000000'01111000'00000000'00000000'00000000'00000000'00000000);
  }

  SECTION( "Row 6" ) {
    REQUIRE(find_flipped(setpos(6, 2), b.black_, b.white_, L2R) == setpos(6, 3));
    REQUIRE(find_flipped(setpos(6, 2), b.black_, b.white_, BL2TR) ==
       0b00000000'00000000'00001000'00010000'00100000'00000000'00000000'00000000);

    REQUIRE(find_flipped(setpos(6, 5), b.black_, b.white_, L2R) == setpos(6, 6));
    REQUIRE(find_flipped(setpos(6, 5), b.black_, b.white_, B2T) ==
       0b00000000'00000000'00100000'00100000'00100000'00100000'00000000'00000000);
    REQUIRE(find_flipped(setpos(6, 5), b.white_, b.black_, R2L) == setpos(6, 4));
  }

  SECTION( "Row 7" ) {
    REQUIRE(find_flipped(setpos(7, 2), b.black_, b.white_, R2L) == setpos(7, 1));
    REQUIRE(find_flipped(setpos(7, 2), b.black_, b.white_, BL2TR) ==
       0b00000000'00001000'00010000'00100000'00000000'00000000'00000000'00000000);
    REQUIRE(find_flipped(setpos(7, 2), b.white_, b.black_, L2R) == setpos(7, 3));

    REQUIRE(find_flipped(setpos(7, 5), b.black_, b.white_, R2L) == setpos(7, 4));
    REQUIRE(find_flipped(setpos(7, 5), b.black_, b.white_, BL2TR) == setpos(6, 6));
    REQUIRE(find_flipped(setpos(7, 5), b.white_, b.black_, BR2TL) == setpos(6, 4));
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

  REQUIRE(effect_move(b, Color::WHITE, setpos(2, 4)) ==
    Board({
    "..ooxoox",
    "o.oooo.x",
    "xooooox.",
    "x.x.ooxo",
    "ooo.ooxo",
    ".o.oooox",
    ".x.ox.ox",
    "xo.xo.x."}));

  REQUIRE(effect_move(b, Color::BLACK, setpos(4, 3)) ==
    Board({
    "..ooxoox",
    "o.ooox.x",
    "xoox.ox.",
    "x.x.xoxo",
    "oooxxxxo",
    ".o.xooox",
    ".x.xx.ox",
    "xo.xo.x."}));

  REQUIRE(effect_move(b, Color::BLACK, setpos(7, 2)) ==
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
TEST_CASE( "effect_moves changes nothing when no legal moves", "[moves]" ) {
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

    REQUIRE(effect_move(b, Color::BLACK, setpos(1, 3)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(1, 3)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(2, 6)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(2, 6)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(7, 3)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(7, 3)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(7, 4)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(7, 4)) == b);
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

    REQUIRE(effect_move(b, Color::BLACK, setpos(1, 3)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(1, 3)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(2, 6)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(2, 6)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(6, 2)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(6, 2)) == b);
    REQUIRE(effect_move(b, Color::BLACK, setpos(5, 7)) == b);
    REQUIRE(effect_move(b, Color::WHITE, setpos(5, 7)) == b);
  }
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

  auto pb = player_ptr_t(new DummyPlayer(Color::BLACK));
  auto pw = player_ptr_t(new DummyPlayer(Color::WHITE));

  REQUIRE(play_game(board, pb, pw) == -2);
  REQUIRE(play_game(board, pw, pb) == -2);
}
