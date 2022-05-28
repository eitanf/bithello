/*
 * Unit tests for scanning functionality
 */

#define CATCH_CONFIG_MAIN

#include "scan.hh"
#include "catch.hh"

using namespace Othello;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "horizontal and vertical starts set the right number of bits", "[scan]" ) {
  REQUIRE(bits_set(L_START) == N);
  REQUIRE(bits_set(R_START) == N);
  REQUIRE(bits_set(T_START) == N);
  REQUIRE(bits_set(B_START) == N);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "L_START covers the leftmost bits exactly" "[scan]" ) {
  bits_t bits = 0;
  for (idx_t i = 0; i < N; ++i) {
    bits = set(bits, pos2bit(i, 0));
  }
  REQUIRE(L_START == bits);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "R_START covers the rightmost bits exactly" "[scan]" ) {
  bits_t bits = 0;
  for (idx_t i = 0; i < N; ++i) {
    bits = set(bits, pos2bit(i, N-1));
  }
  REQUIRE(R_START == bits);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "T_START covers the top-row bits exactly" "[scan]" ) {
  bits_t bits = 0;
  for (idx_t i = 0; i < N; ++i) {
    bits = set(bits, pos2bit(0, i));
  }
  REQUIRE(T_START == bits);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "B_START covers the bottom-row bits exactly" "[scan]" ) {
  bits_t bits = 0;
  for (idx_t i = 0; i < N; ++i) {
    bits = set(bits, pos2bit(N-1, i));
  }
  REQUIRE(B_START == bits);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "L_START ends at the rightmost bits exactly" "[scan]" ) {
  auto mask = L_START;
  const auto shift = L2R;

  for (idx_t i = 0; i < N - 1; ++i) {
    mask = shift(mask);
  }
  REQUIRE(mask == R_START);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "R_START ends at the leftmost bits exactly" "[scan]" ) {
  auto mask = R_START;
  const auto shift = R2L;

  for (idx_t i = 0; i < N - 1; ++i) {
    mask = shift(mask);
  }
  REQUIRE(mask == L_START);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "T_START ends at the bottom bits exactly" "[scan]" ) {
  auto mask = T_START;
  const auto shift = T2B;

  for (idx_t i = 0; i < N - 1; ++i) {
    mask = shift(mask);
  }
  REQUIRE(mask == B_START);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "B_START ends at the top bits exactly" "[scan]" ) {
  auto mask = B_START;
  const auto shift = B2T;

  for (idx_t i = 0; i < N - 1; ++i) {
    mask = shift(mask);
  }
  REQUIRE(mask == T_START);
}

////////////////////////////////////////////////////////////////////////////////
// diagonals
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "diagonal_start sets the right number of bits", "[scan]" ) {
  REQUIRE(BL_START.bits_set() == NDIAG);
  REQUIRE(BR_START.bits_set() == NDIAG);
  REQUIRE(TL_START.bits_set() == NDIAG);
  REQUIRE(TR_START.bits_set() == NDIAG);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "BL diagonal ends at the right spots" "[scan]" ) {
  auto mask = BL_START;
  const auto shift = BL2TR;

  REQUIRE(mask & setpos(2, 0));

  // Testing for correct end positions for all 11 diagonals after 7 shifts:
  mask = shift(mask);
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 2));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 3));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 4));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 5));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 6));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 7));
  REQUIRE(mask & setpos(1, 7));
  REQUIRE(mask & setpos(2, 7));
  REQUIRE(mask & setpos(3, 7));
  REQUIRE(mask & setpos(4, 7));
  REQUIRE(mask & setpos(5, 7));
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "BR diagonal ends at the right spots" "[scan]" ) {
  auto mask = BR_START;
  const auto shift = BR2TL;

  REQUIRE(mask & setpos(2, 7));

  // Testing for correct end positions for all 11 diagonals after 7 shifts:
  mask = shift(mask);
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 5));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 4));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 3));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 2));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 1));
  mask = shift(mask);
  REQUIRE(mask & setpos(0, 0));
  REQUIRE(mask & setpos(1, 0));
  REQUIRE(mask & setpos(2, 0));
  REQUIRE(mask & setpos(3, 0));
  REQUIRE(mask & setpos(4, 0));
  REQUIRE(mask & setpos(5, 0));
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "TL diagonal ends at the right spots" "[scan]" ) {
  auto mask = TL_START;
  const auto shift = TL2BR;

  REQUIRE(mask & setpos(5, 0));

  // Testing for correct end positions for all 11 diagonals after 7 shifts:
  mask = shift(mask);
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 2));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 3));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 4));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 5));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 6));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 7));
  REQUIRE(mask & setpos(6, 7));
  REQUIRE(mask & setpos(5, 7));
  REQUIRE(mask & setpos(4, 7));
  REQUIRE(mask & setpos(3, 7));
  REQUIRE(mask & setpos(2, 7));
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "TR diagonal ends at the right spots" "[scan]" ) {
  auto mask = TR_START;
  const auto shift = TR2BL;

  REQUIRE(mask & setpos(5, 7));

  // Testing for correct end positions for all 11 diagonals after 7 shifts:
  mask = shift(mask);
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 5));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 4));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 3));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 2));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 1));
  mask = shift(mask);
  REQUIRE(mask & setpos(7, 0));
  REQUIRE(mask & setpos(6, 0));
  REQUIRE(mask & setpos(5, 0));
  REQUIRE(mask & setpos(4, 0));
  REQUIRE(mask & setpos(3, 0));
  REQUIRE(mask & setpos(2, 0));
}
