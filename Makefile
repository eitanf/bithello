CXX=g++-11 # clang++-13 also works
CXXFLAGS=-Wall -Wextra -pedantic -Werror -std=c++2a
OPTFLAGS=-O0 -g -march=native
OPTFLAGS=-O3 -DNDEBUG -march=native -funroll-loops -DBENCHMARK  #-g -pg
LDFLAGS=$(CXXFLAGS) -pthread -flto # -pg
OBJ=$(SRC:.cc=.o)

all:  bithello

bithello: bithello.o board.o text_player.o random_player.o mcts_player.o mcts_node.o moves.o stop.o
	$(CXX) $(LDFLAGS)  -o $@ $^

test_bits: test_bits.o
	$(CXX) $(LDFLAGS)  -o $@ $^

test_bits.o: test_bits.cc bits.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

test_scan: test_scan.o
	$(CXX) $(LDFLAGS)  -o $@ $^

test_scan.o: test_scan.cc moves.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

test_moves: test_moves.o board.o moves.o
	$(CXX) $(LDFLAGS)  -o $@ $^

test_moves.o: test_moves.cc moves.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

test_mcts: test_mcts.o mcts_node.o mcts_player.o board.o moves.o random_player.o
	$(CXX) $(LDFLAGS)  -o $@ $^

test_mcts.o: test_mcts.cc mcts_node.hh stop.hh player.hh moves.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

bithello.o: bithello.cc stop.hh mcts_node.hh player.hh moves.hh scan.hh bits.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

clean:
	rm -rf *.o bithello *.dSYM test_bits test_scan test_moves

test:	test_bits test_scan test_moves test_mcts
	./test_bits
	./test_scan
	./test_moves
	./test_mcts
