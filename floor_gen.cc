#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <cmath>
#include <fstream>
#include <algorithm>

void dfs(const std::vector<std::vector<char>> &floor_map, int row, int col, std::vector<std::pair<int, int>> &corner_tiles, std::vector<std::vector<bool>> &visited) {
    if (row < 0 || col < 0 || row >= floor_map.size() || col >= floor_map[0].size() || floor_map[row][col] != '.' || visited[row][col]) {
        return;
    }

    visited[row][col] = true;

    int wall_count = 0;
    const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto &dir : directions) {
        int new_row = row + dir.first;
        int new_col = col + dir.second;
        if (new_row >= 0 && new_col >= 0 && new_row < floor_map.size() && new_col < floor_map[0].size()) {
            if (floor_map[new_row][new_col] == '|' || floor_map[new_row][new_col] == '-') {
                if (!((floor_map[row - 1][col] == '.' && floor_map[row + 1][col] == '.') || (floor_map[row][col - 1] == '.' && floor_map[row][col + 1] == '.'))) {
                    wall_count++;
                }
            }
        }
    }

    if (wall_count >= 2) {
        corner_tiles.push_back({row, col});
    }

    for (const auto &dir : directions) {
        dfs(floor_map, row + dir.first, col + dir.second, corner_tiles, visited);
    }
}

std::vector<std::vector<std::pair<int, int>>> detect_chamber_corner_tiles(const std::vector<std::vector<char>> &floor_map) {
    std::vector<std::vector<std::pair<int, int>>> chambers_corner_tiles;
    std::vector<std::vector<bool>> visited(floor_map.size(), std::vector<bool>(floor_map[0].size(), false));

    for (int i = 0; i < floor_map.size(); ++i) {
        for (int j = 0; j < floor_map[i].size(); ++j) {
            if (floor_map[i][j] == '.' && !visited[i][j]) {
                std::vector<std::pair<int, int>> corner_tiles;
                dfs(floor_map, i, j, corner_tiles, visited);
                chambers_corner_tiles.push_back(corner_tiles);
            }
        }
    }

    return chambers_corner_tiles;
}


const int WIDTH = 79;
const int HEIGHT = 25;

const char WALL = '|';
const char HORIZONTAL_WALL = '-';
const char FLOOR = '.';
const char PASSAGE = '#';
const char EMPTY = ' ';

enum quad { FIRST, SECOND, THIRD, FOURTH, MIDDLE };

void drawLine(int x0, int y0, int x1, int y1, std::vector<std::vector<char>>& map) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < static_cast<int>(map[0].size()) && y0 >= 0 && y0 < static_cast<int>(map.size())) {
            map[y0][x0] = '.';
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Point Selection in Region For Chambers

float distance(std::pair<int, int> a, std::pair<int, int> b) {
    return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
}

std::pair<int, int> generateRandomPoint(int x_min, int x_max, int y_min, int y_max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> x_distribution(x_min, x_max);
    std::uniform_int_distribution<> y_distribution(y_min, y_max);

    int x = x_distribution(gen);
    int y = y_distribution(gen);

    return std::make_pair(x, y);
}

std::vector<std::pair<int, int>> generateRandomPoints(int area_x_min, int area_x_max, int area_y_min, int area_y_max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> point_count_distribution(6, 10);

    int point_count = point_count_distribution(gen);

    std::vector<std::pair<int, int>> points;
    points.reserve(point_count);

    for (int i = 0; i < point_count; ++i) {
        std::pair<int, int> pt = generateRandomPoint(area_x_min, area_x_max, area_y_min, area_y_max);
        if (i != 0) {
            for (int j = 0; j < i; ++j) {
                float dist = distance(points[j], pt);
                while (true) {
                    if (dist > 2.5) {
                        points.push_back(pt);
                        break;
                    }
                    pt = generateRandomPoint(area_x_min, area_x_max, area_y_min, area_y_max);
                    dist = distance(points[j], pt);
                }
            }
        } else {
            points.push_back(pt);
        }
    }

    return points;
}


// Generating Chambers from Given Vector of Points

bool isCorner(std::vector<std::vector<char>> &floor, int x, int y) {
    if (floor[x][y] == FLOOR || floor[x][y] == WALL) return false;
    if ((floor[x][y] != HORIZONTAL_WALL) && (floor[x][y-1] == HORIZONTAL_WALL || floor[x][y+1] == HORIZONTAL_WALL)) return true;
    if ((floor[x][y-1] == WALL || floor[x][y-1] == HORIZONTAL_WALL) && floor[x][y+1] == FLOOR) {
        if (floor[x][y-1] == WALL) floor[x][y-1] = EMPTY;
        return true;
    }
    if ((floor[x][y+1] == WALL || floor[x][y+1] == HORIZONTAL_WALL) && floor[x][y-1] == FLOOR) {
        if (floor[x][y+1] == WALL) floor[x][y+1] = EMPTY;
        return true;
    }
    return false;
}

bool isPointInChamber(std::vector<std::pair<int, int>> &chamber, int x, int y) {
    size_t num_points = chamber.size();
    bool inside = false;

    for (size_t i = 0, j = num_points - 1; i < num_points; j = i++) {
        int xi = chamber[i].first, yi = chamber[i].second;
        int xj = chamber[j].first, yj = chamber[j].second;

        bool intersect = ((yi > y) != (yj > y)) &&
                         (x < (xj - xi) * (y - yi) / (yj - yi) + xi);

        if (intersect) {
            inside = !inside;
        }
    }

    return inside;
}

int getFloorNeighbours(std::vector<std::vector<char>> &floor, int x, int y) {
    int count = 0;
    if (floor[x-1][y-1] == FLOOR) ++count;
    if (floor[x-1][y+1] == FLOOR) ++count;
    if (floor[x+1][y-1] == FLOOR) ++count;
    if (floor[x+1][y] == FLOOR) ++count;
    if (floor[x-1][y] == FLOOR) ++count;
    if (floor[x][y+1] == FLOOR) ++count;
    if (floor[x][y-1] == FLOOR) ++count;
    if (floor[x+1][y+1] == FLOOR) ++count;
    return count;
}

bool floorInRange(std::vector<std::vector<char>> &floor, int x, int y) {
    if (x > 0 && x < HEIGHT - 1 && y > 0 && y < WIDTH - 1) {
        if (getFloorNeighbours(floor, x, y) > 4) return true;
    }

    return false;
}

void generateFloor(std::vector<std::vector<char>> &floor, std::vector<std::pair<int, int>> &points) {
    for (const auto &point : points) {
        floor[point.second][point.first] = FLOOR;
    }

    std::vector<std::pair<int, int>> newPoints;

    for (int i = 0; i < points.size(); ++i) {
        std::vector<std::pair<int, int>> vec;
        if (i+1 != points.size()) {
            for (int j = i+1; j < points.size(); ++j) {
                drawLine(points[i].first, points[i].second, points[j].first, points[j].second, floor);
            }
        } else {
            drawLine(points[i].first, points[i].second, points[0].first, points[0].second, floor);
        }
    }

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (floor[i][j] == EMPTY) {
                if (floorInRange(floor, i, j)) floor[i][j] = FLOOR;
            }
        }
    }

    for (int i = 1; i < HEIGHT; ++i) {
        for (int j = 1; j < WIDTH; ++j) {
            if (floor[i][j] == FLOOR) {
                if (floor[i][j - 1] == EMPTY) floor[i][j - 1] = WALL;
                if (floor[i][j + 1] == EMPTY) floor[i][j + 1] = WALL;
                if (floor[i - 1][j] == EMPTY) floor[i - 1][j] = HORIZONTAL_WALL;
                if (floor[i + 1][j] == EMPTY) floor[i + 1][j] = HORIZONTAL_WALL;
            }
        }
    }

    for (int i = 1; i < HEIGHT; ++i) {
        for (int j = 1; j < WIDTH; ++j) {
            if (isCorner(floor, i, j)) {
                floor[i][j] = WALL;
            }
        }
    }
}

int main() {
    std::vector<std::vector<char>> floor(HEIGHT, std::vector<char>(WIDTH, EMPTY));
    std::vector<std::pair<int, int>> points = generateRandomPoints(2, 30, 2, 9);
    std::vector<std::pair<int, int>> points2 = generateRandomPoints(52, 76, 17, 22);
    std::vector<std::pair<int, int>> points3 = generateRandomPoints(2, 30, 17, 22);
    std::vector<std::pair<int, int>> points4 = generateRandomPoints(52, 76, 2, 9);
    std::vector<std::pair<int, int>> points5 = generateRandomPoints(31, 49, 11, 15);

    generateFloor(floor, points);
    generateFloor(floor, points2);
    generateFloor(floor, points3);
    generateFloor(floor, points4);
    generateFloor(floor, points5);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (y == 0 || y == HEIGHT - 1) {
                floor[y][x] = HORIZONTAL_WALL;
            }
            if (x == 0 || x == WIDTH - 1) {
                floor[y][x] = WALL;
            }
        }
    }

    std::ofstream output_file("generated_floor.txt");

    for (const auto &row : floor) {
        for (const auto &col : row) {
                output_file << col;
        }
        output_file << std::endl;
    }
    return 0;
}
