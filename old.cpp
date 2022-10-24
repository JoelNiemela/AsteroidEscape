#include <string>
#include <array>
#include <iostream>
#include <set>
#include <vector>

struct Tile {
	bool is_void = false;
	std::array<std::string, 4> structure;
	int index;
	bool ship = false;

	Tile() {}

	Tile(std::array<std::string, 4> structure, int index) : structure(structure), index(index) {}
};

bool check(char base, char overlay, bool final_move = true) {
	if (base == '_') return true;
	if (overlay == '_') return true;
	if (final_move && base == '~') return true;
	if (final_move && overlay == '~') return true;

	return false;
}

char overlay(char base, char overlay, bool final_move = true) {
	if (base == '_') return overlay;
	if (overlay == '_') return base;
	if (final_move && base == '~') return overlay;
	if (final_move && overlay == '~') return base;

	std::cout << "ERROR" << std::endl;
	return '#';
}

std::array<std::string, 10> board;
std::array<Tile, 9> tiles;
std::set<std::string> visited;
std::vector<int> traceback;

void clear_board() {
	for (int i = 0; i < 10; i++) {
		board[i] = std::string(8, '_');
	}
}

void draw_board(int ignore = -1) {
	std::cout << "draw_board(ignore = " << ignore << ")" << std::endl;
	clear_board();
	for (int i = 0; i < 9; i++) {
		if (i == ignore) continue;
		Tile tile = tiles[i];
		int offset_x = (i % 3) * 2;
		int offset_y = (i / 3) * 2;
		for (int rel_x = 0; rel_x < 4; rel_x++) {
			int x = offset_x + rel_x;
			for (int rel_y = 0; rel_y < 4; rel_y++) {
				int y = offset_y + rel_y;
				if (!check(board[y][x], tile.structure[rel_y][rel_x])) {
					std::cout << "Error: Invalid state! [" << x << ", " << y << "]{" << board[y][x] << ", " << tile.structure[rel_y][rel_x] << "}" << std::endl;
				}
				board[y][x] = overlay(board[y][x], tile.structure[rel_y][rel_x]);
			}
		}
	}
}

std::string state_string() {
	std::string str = "";
	for (int i = 0; i < 9; i++) {
		str += '0' + tiles[i].index;
	}

	return str;
}

bool check_move(int tile_i, int dest_i, int dx, int dy) {
	std::cout << "check_move(" << tile_i << " -> " << dest_i << ", dx = " << dx << ", dy = " << dy << ")" << std::endl;
	if (dest_i != 10 && !tiles[dest_i].is_void) return false;

	Tile tile = tiles[tile_i];
	if (tile.is_void) return false;

	std::string state = state_string();
	if (dest_i == 10) {
		state = "SOLVED";
	} else {
		std::swap(state[tile_i], state[dest_i]);
	}
	if (visited.find(state) != visited.end()) {
		return false;
	}

	for (int i = 0; i <= 2; i++) {
		draw_board(tile_i);
		int offset_x = (tile_i % 3) * 2 + (dx * i);
		int offset_y = (tile_i / 3) * 2 + (dy * i);
		for (int rel_x = 0; rel_x < 4; rel_x++) {
			int x = offset_x + rel_x;
			for (int rel_y = 0; rel_y < 4; rel_y++) {
				int y = offset_y + rel_y;
				if (!check(board[y][x], tile.structure[rel_y][rel_x], i != 1)) {
					return false;
				}
			}
		}
	}

	return true;
}

void move(int tile_i, int dest_i) {
	std::cout << "move(" << tile_i << " -> " << dest_i << ")" << std::endl;
	if (!tiles[dest_i].is_void || tiles[tile_i].is_void) {
		std::cout << "Error: Invalid move!" << std::endl;
	}

	std::swap(tiles[tile_i], tiles[dest_i]);
	visited.insert(state_string());

	draw_board();

	for (auto line : board) {
		std::cout << line << std::endl;
	}
}

int find_void() {
	for (int i = 0; i < 9; i++) {
		if (tiles[i].is_void) return i;
	}

	std::cout << "Error: No void tile" << std::endl;
	return -1;
}

int find_ship() {
	for (int i = 0; i < 9; i++) {
		if (tiles[i].ship) return i;
	}

	std::cout << "Error: No ship tile" << std::endl;
	return -1;
}

bool try_moves() {
	if (find_ship() == 7 && check_move(7, 10, 0, 1)) {
		std::cout << "SOLVED" << std::endl;
		draw_board();
		for (auto line : board) {
			std::cout << line << std::endl;
		}
		return true;
	}

	int tile_i = find_void();

	int y = tile_i / 3;
	int x = tile_i % 3;
	std::cout << "try_moves(x = " << x << ", y = " << y << ")" << std::endl;
	if (x > 0 && check_move(tile_i-1, tile_i, 1, 0)) {
		move(tile_i-1, tile_i);
		traceback.push_back(tile_i);
		if (try_moves()) {
			return true;
		}
		std::cout << "BACKTRACKING" << std::endl;
		move(tile_i, tile_i-1);
		traceback.pop_back();
	}
	if (x < 2 && check_move(tile_i+1, tile_i, -1, 0)) {
		move(tile_i+1, tile_i);
		traceback.push_back(tile_i);
		if (try_moves()) {
			return true;
		}
		std::cout << "BACKTRACKING" << std::endl;
		move(tile_i, tile_i+1);
		traceback.pop_back();
	}
	if (y > 0 && check_move(tile_i-3, tile_i, 0, 1)) {
		move(tile_i-3, tile_i);
		traceback.push_back(tile_i);
		if (try_moves()) {
			return true;
		}
		std::cout << "BACKTRACKING" << std::endl;
		move(tile_i, tile_i-3);
		traceback.pop_back();
	}
	if (y < 2 && check_move(tile_i+3, tile_i, 0, -1)) {
		move(tile_i+3, tile_i);
		traceback.push_back(tile_i);
		if (try_moves()) {
			return true;
		}
		std::cout << "BACKTRACKING" << std::endl;
		move(tile_i, tile_i+3);
		traceback.pop_back();
	}

	return false;
}

int main() {
	for (int i = 0; i < 9; i++) {
		std::array<std::string, 4> structure;
		bool ship = false;
		for (int i = 0; i < 4; i++) {
			std::cin >> structure[i];
			if (structure[i] == "ship") {
				i--;
				ship = true;
			}
		}
		tiles[i] = Tile(structure, i);
		if (structure[0] == "____" && structure[1] == "____" && structure[2] == "____" && structure[3] == "____") {
			tiles[i].is_void = true;
		}
		if (ship) {
			tiles[i].ship = true;
		}

		std::cout << "Tile: " << tiles[i].is_void << std::endl;
		for (auto line : tiles[i].structure) {
			std::cout << line << std::endl;
		}
	}

	draw_board();

	try_moves();

	draw_board();

	std::cout << visited.size() << std::endl;

	for (auto e : traceback) {
		std::cout << e << " -> ";
	}
	std::cout << std::endl;
}
