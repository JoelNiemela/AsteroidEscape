#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>
#include <climits>

struct Tile {
	std::array<std::string, 4> structure;

	Tile() {}
	Tile(std::array<std::string, 4> structure) : structure(structure) {}
} tiles[9];

std::vector<int> get_adj_tiles(int pos) {
	std::vector<int> adj;
	if (pos % 3 > 0) adj.push_back(pos-1);
	if (pos % 3 < 2) adj.push_back(pos+1);
	if (pos / 3 > 0) adj.push_back(pos-3);
	if (pos / 3 < 2) adj.push_back(pos+3);

	return adj;
}

std::vector<std::array<char, 9>> get_adj_states(std::array<char, 9> state) {
	std::vector<std::array<char, 9>> states;

	int pos = std::find(state.begin(), state.end(), '0') - state.begin();
	std::vector<int> adj_tiles = get_adj_tiles(pos);
	for (int adj : adj_tiles) {
		std::array<char, 9> adj_state = state;
		std::swap(adj_state[pos], adj_state[adj]);
		states.push_back(adj_state);
	}

	return states;
}

bool add_tile_to_board(std::array<std::array<char, 8>, 8> &board, const Tile &tile, int tile_x, int tile_y,
                       bool moving = false) {
	for (int i = 0; i < 16; i++) {
		int rel_x = i % 4;
		int rel_y = i / 4;

		int x = tile_x + rel_x;
		int y = tile_y + rel_y;

		if (x >= 8 || y >= 8) continue;

		if (board[y][x] == '*' || (moving && board[y][x] == '@')) {
			if (tile.structure[rel_y][rel_x] == '*' || (moving && tile.structure[rel_y][rel_x] == '@')) {
				return false;
			}
		}

		if (tile.structure[rel_y][rel_x] != '_') {
			board[y][x] = tile.structure[rel_y][rel_x];
		}
	}

	return true;
}

void print_board(const std::array<std::array<char, 8>, 8> &board) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			std::cout << board[y][x];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

bool get_board(std::array<std::array<char, 8>, 8> &board, std::array<char, 9> state) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = '_';
		}
	}

	for (int i = 0; i < 9; i++) {
		char tile_index = state[i];
		Tile tile = tiles[tile_index - '0'];

		int tile_x = (i % 3) * 2;
		int tile_y = (i / 3) * 2;

		if (!add_tile_to_board(board, tile, tile_x, tile_y)) {
			return false;
		}
	}

	return true;
}

std::pair<int, int> get_dir(int from_pos, int to_pos) {
	int x = from_pos % 3;
	int y = from_pos / 3;

	if (to_pos == from_pos+1 && x < 2) return std::make_pair(1, 0);
	if (to_pos == from_pos-1 && x > 0) return std::make_pair(-1, 0);
	if (to_pos == from_pos+3 && y < 2) return std::make_pair(0, 1);
	if (to_pos == from_pos-3 && y > 0) return std::make_pair(0, -1);

	return std::make_pair(0, 0);
}

bool valid_move(std::array<char, 9> from, std::array<char, 9> to) {
	int from_pos = std::find(to.begin(), to.end(), '0') - to.begin();
	int to_pos = std::find(from.begin(), from.end(), '0') - from.begin();

	std::array<char, 9> empty_state = from;
	empty_state[from_pos] = '0';

	auto [dx, dy] = get_dir(from_pos, to_pos);

	if (dx == 0 && dy == 0) return false;

	int x = (from_pos % 3) * 2;
	int y = (from_pos / 3) * 2;

	char tile_index = from[from_pos];
	Tile tile = tiles[tile_index - '0'];
	for (int i = 0; i <= 2; i++) {
		std::array<std::array<char, 8>, 8> board;
		if (!get_board(board, empty_state) || !add_tile_to_board(board, tile, x + dx*i, y + dy*i)) {
			return false;
		}
	}

	return true;
}

bool is_solve_state(std::array<char, 9> state) {
	if (state[7] != '8') return false;

	std::array<std::array<char, 8>, 8> board;
	get_board(board, state);

	std::vector<std::pair<int, int>> exit_path = {
		{6, 2},                 {6, 5},
		{7, 2}, {7, 3}, {7, 4}, {7, 5},
	};

	for (const auto [y, x] : exit_path) {
		if (board[y][x] != '_') return false;
	}

	return true;
}

struct StateDistance {
	std::array<char, 9> state;
	int dst;

	StateDistance(std::array<char, 9> state, int dst) : state(state), dst(dst) {}
};

int main() {
	tiles[0] = Tile({
		"____",
		"____",
		"____",
		"____",
	});

	tiles[8] = Tile({
		"____",
		"****",
		"_@@_",
		"____",
	});

	for (int i = 1; i <= 7; i++) {
		std::array<std::string, 4> structure;
		for (int j = 0; j < 4; j++) {
			std::cin >> structure[j];
		}
		tiles[i] = Tile(structure);
	}

	std::array<char, 9> state;
	for (int i = 0; i < 9; i++) {
		std::cin >> state[i];
	}

	StateDistance solve_state = {{'0', '0', '0', '0', '0', '0', '0', '0', '0'}, INT_MAX};

	std::map<std::array<char, 9>, int> dst;
	std::map<std::array<char, 9>, std::array<char, 9>> prev;

	std::queue<StateDistance> queue;
	queue.emplace(state, 0);
	dst[state] = 0;
	while (!queue.empty()) {
		StateDistance state = queue.front(); queue.pop();
		auto adj_states = get_adj_states(state.state);
		for (const auto &adj : adj_states) {
			auto it = dst.find(adj);
			if (it != dst.end() && it->second <= state.dst+1) continue;

			if (valid_move(state.state, adj)) {
				queue.emplace(adj, state.dst+1);
				dst[adj] = state.dst+1;
				prev[adj] = state.state;

				if (state.dst+1 < solve_state.dst && is_solve_state(adj)) {
					solve_state.state = adj;
					solve_state.dst = state.dst+1;
				}
			}
		}
	}

	std::vector<std::array<char, 9>> path;
	std::array<char, 9> curr_state = solve_state.state;
	while (curr_state != state) {
		path.push_back(curr_state);
		curr_state = prev[curr_state];
	}

	std::reverse(path.begin(), path.end());

	for (int i = 0; i < path.size(); i++) {
		std::array<char, 9> state = path[i];
		std::array<char, 9> prev_state = prev[state];
		int from_pos = std::find(state.begin(), state.end(), '0') - state.begin();
		int to_pos = std::find(prev_state.begin(), prev_state.end(), '0') - prev_state.begin();

		std::array<std::array<char, 10>, 3> disp;
		for (auto &line : disp) {
			std::fill_n(line.begin(), 10, ' ');
		}

		for (int i = 0; i < 9; i++) {
			int x = i % 3;
			int y = i / 3;

			disp[y][x] = state[i] == '0' ? '@' : '.';
			disp[y][x + 7] = prev_state[i] == '0' ? '@' : '.';
		}

		disp[1][4] = '-';
		disp[1][5] = '>';

		std::cout << i+1 << ':' << std::endl;
		for (const auto &line : disp) {
			for (char c : line) std::cout << c;
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << path.size()+1 << ':' << std::endl;
	std::cout << "...    ..." << std::endl;
	std::cout << "... -> ..." << std::endl;
	std::cout << ".@.    ..." << std::endl;
	std::cout << "        @ " << std::endl;
}
