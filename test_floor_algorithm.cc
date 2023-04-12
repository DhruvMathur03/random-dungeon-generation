#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>
#include <string>

bool isPointInQuadrant(std::vector<std::pair<int, int>> &quadrant, int x, int y) {
    return (x >= quadrant[0].first && x <= quadrant[1].first &&
            y >= quadrant[0].second && y <= quadrant[1].second);
}

void drawLine(int x0, int y0, int x1, int y1, std::vector<std::string> &map) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < (map[0].size()) && y0 >= 0 && y0 < (map.size())) {
            if (map[y0][x0] == ' ') {
                map[y0][x0] = '#';
            } else if (map[y0][x0] == '|' || map[y0][x0] == '-') {
                map[y0][x0] = '+';
            }
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

std::vector<std::string> read_map_from_file(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::string> floor;
    std::string line;
    while (std::getline(file, line)) {
        floor.push_back(line);
    }
    return floor;
}

void dfs(const std::vector<std::string> &floor_map, int row, int col, std::vector<std::pair<int, int>> &corner_tiles, std::vector<std::vector<bool>> &visited) {
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

std::vector<std::vector<std::pair<int, int>>> detect_chamber_corner_tiles(const std::vector<std::string> &floor_map) {
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

void makePaths(std::vector<std::vector<std::pair<int, int>>> &minMaxCoords, std::vector<std::string> &floor) {
    // Initializing Quadrants
    std::vector<std::pair<int, int>> quadrantOne = {std::make_pair(2, 2), std::make_pair(10, 30)};
    std::vector<std::pair<int, int>> quadrantTwo = {std::make_pair(2, 52), std::make_pair(10, 76)};
    std::vector<std::pair<int, int>> quadrantThree = {std::make_pair(17, 2), std::make_pair(22, 30)};
    std::vector<std::pair<int, int>> quadrantFour = {std::make_pair(17, 52), std::make_pair(22, 76)};
    std::vector<std::pair<int, int>> quadrantMiddle = {std::make_pair(11, 31), std::make_pair(15, 49)};

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist_seed(0, 3);
    int seed = dist_seed(gen);
    for (int i = 0; i < minMaxCoords.size(); ++i) {

        if ((isPointInQuadrant(quadrantOne, minMaxCoords[i][0].first, minMaxCoords[i][0].second))) {
            // found 1st chamber
            // drawing 1 -> 2
            int min_x = minMaxCoords[i][0].first;
            int max_x = minMaxCoords[i][1].first;
            int min_y = minMaxCoords[i][0].second;
            int max_y = minMaxCoords[i][1].second;

            std::uniform_int_distribution<int> dist(min_x + 1, max_x - 1);

            int des_x = dist(gen);
            int des_y = 0;
            int new_des_y = 0;
            if (seed == 0 || seed == 1 || seed == 2) {
                while (true) {
                    for (int j = 0; j < 20; ++j) {
                        if (floor[des_x][max_y + 1 - j] == '|') {
                            des_y = max_y + 1 - j;
                            break;
                        }
                    }

                    for (int j = 1; j < 35; ++j) {
                        if (floor[des_x][des_y + j] == '|') {
                            new_des_y = des_y + j;
                            break;
                        }
                    }
                    if (new_des_y != 0) break;
                    des_x = dist(gen);
                }

                drawLine(des_y, des_x, new_des_y, des_x, floor);
            }

            // drawing 1 -> 4
            std::uniform_int_distribution<int> dist2(min_y + 1, max_y - 1);

            int des_y2 = dist2(gen);
            int des_x2 = 0;
            int new_des_x2 = 0;
            if (seed == 0 || seed == 1 || seed == 3) {
                while (true) {
                    for (int j = 0; j < 20; ++j) {
                        if (floor[max_x + 1 - j][des_y2] == '-') {
                            des_x2 = max_x + 1 - j;
                            break;
                        }
                    }
                    for (int j = 1; j < 20; ++j) {
                        if (floor[des_x2 + j][des_y2] == '-') {
                            new_des_x2 = des_x2 + j;
                            break;
                        }
                    }

                    if (new_des_x2 != 0) break;
                    des_y2 = dist2(gen);
                }

                drawLine(des_y2, des_x2, des_y2, new_des_x2, floor);
            }
        }

        // finding coordinates that belong in quadrant 4
        if ((isPointInQuadrant(quadrantFour, minMaxCoords[i][0].first, minMaxCoords[i][0].second))) {
            // found 5th chamber
            // drawing 5 -> 4
            int min_x = minMaxCoords[i][0].first;
            int max_x = minMaxCoords[i][1].first;
            int min_y = minMaxCoords[i][0].second;
            int max_y = minMaxCoords[i][1].second;

            std::uniform_int_distribution<int> dist(min_x + 1, max_x - 1);

            int des_x = dist(gen);
            int des_y = 0;
            int new_des_y = 0;

            while (true) {
                for (int j = 0; j < 20; ++j) {
                    if (floor[des_x][min_y - 1 + j] == '|') {
                        des_y = min_y - 1 + j;
                        break;
                    }
                }
                for (int j = 1; j < 35; ++j) {
                    if (floor[des_x][des_y - j] == '|') {
                        new_des_y = des_y - j;
                        break;
                    }
                }

                if (new_des_y != 0) break;
                des_x = dist(gen);
            }

            drawLine(des_y, des_x, new_des_y, des_x, floor);

            // drawing 5 -> 2
            std::uniform_int_distribution<int> dist2(min_y + 1, max_y - 1);
            int des_y2 = dist2(gen);
            int des_x2 = 0;
            int new_des_x2 = 0;

            if (seed == 0 || seed == 3) {
                while (true) {
                    for (int j = 0; j < 20; ++j) {
                        if (floor[min_x - 1 + j][des_y2] == '-') {
                            des_x2 = min_x - 1 + j;
                            break;
                        }
                    }
                    for (int j = 1; j < 20; ++j) {
                        if (floor[des_x2 - j][des_y2] == '-') {
                            new_des_x2 = des_x2 - j;
                            break;
                        }
                    }
                    if (new_des_x2 != 0) break;
                    des_y2 = dist2(gen);
                }

                drawLine(des_y2, des_x2, des_y2, new_des_x2, floor);
            }
        }
    }

    for (int i = 0; i < minMaxCoords.size(); ++i) {
        if (isPointInQuadrant(quadrantMiddle, minMaxCoords[i][0].first, minMaxCoords[i][0].second)) {
            // middle quadrant
            int min_x = minMaxCoords[i][0].first;
            int max_x = minMaxCoords[i][1].first;
            int min_y = minMaxCoords[i][0].second;
            int max_y = minMaxCoords[i][1].second;

            std::uniform_int_distribution<int> dist(min_x + 1, max_x - 1);
            std::uniform_int_distribution<int> dist2(min_y + 1, max_y - 1);

            int des_y = dist2(gen);
            int des_x = 0;
            int new_des_x = 0;

            if (seed == 0 || seed == 2) {
                while (true) {
                    for (int j = 0; j < 10; ++j) {
                        if (floor[min_x - 1 + j][des_y] == '-') {
                            des_x = min_x - 1 + j;
                            break;
                        }
                    }
                    if (des_x != 0) break;
                    des_y = dist2(gen);
                }
                for (int j = 0; j < min_x; ++j) {
                    if (floor[min_x - 1 - j][des_y] == '#') {
                        new_des_x = min_x - 1 - j;
                        break;
                    }
                }

                drawLine(des_y, des_x, des_y, new_des_x, floor);
            }

            int des_y2 = dist2(gen);
            int des_x2 = 0;
            int new_des_x2 = 0;

            while (true) {
                for (int j = 0; j < 10; ++j) {
                    if (floor[max_x + 1 - j][des_y2] == '-') {
                        des_x2 = max_x + 1 - j;
                        break;
                    }
                }
                if (des_x2 != 0) break;
                des_y2 = dist2(gen);
            }
            for (int j = 0; j < 24 - max_x; ++j) {
                if (floor[max_x + 1 + j][des_y2] == '#') {
                    new_des_x2 = max_x + 1 + j;
                    break;
                }
            }

            drawLine(des_y2, des_x2, des_y2, new_des_x2, floor);

            int des_x3 = dist(gen);
            int des_y3 = 0;
            int new_des_y3 = 0;

            if (seed == 0 || seed == 1 || seed == 3) {
                for (int j = 0; j < 10; ++j) {
                    if (floor[des_x3][min_y - 1 + j] == '|') {
                        des_y3 = min_y - 1 + j;
                        break;
                    }
                }
                for (int j = 0; j < min_y; ++j) {
                    if (floor[des_x3][min_y - 1 - j] == '#') {
                        new_des_y3 = min_y - 1 - j;
                        break;
                    }
                }

                drawLine(des_y3, des_x3, new_des_y3, des_x3, floor);
            }

            des_x3 = dist(gen);
            des_y3 = 0;
            new_des_y3 = 0;

            if (seed == 0) {
                for (int j = 0; j < 10; ++j) {
                    if (floor[des_x3][max_y + 1 - j] == '|') {
                        des_y3 = max_y + 1 - j;
                        break;
                    }
                }
                for (int j = 0; j < 76 - max_y; ++j) {
                    if (floor[des_x3][max_y + 1 + j] == '#') {
                        new_des_y3 = max_y + 1 + j;
                        break;
                    }
                }

                drawLine(des_y3, des_x3, new_des_y3, des_x3, floor);
            }

            break;
        }
    }

}


int main() {
    std::string filename = "generated_floor.txt";

    std::vector<std::string> floor = read_map_from_file(filename);
    std::vector<std::vector<std::pair<int, int>>> chamber_coordinates = detect_chamber_corner_tiles(floor);

    std::vector<std::vector<std::pair<int, int>>> minMaxCoords;

    for (const auto &chambers : chamber_coordinates) {
        std::vector<std::pair<int, int>> coords;

        // Initializing Min and Max Height and Width coordinates
        const auto max_w = max_element(chambers.begin(), chambers.end(), [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });
        const auto max_h = max_element(chambers.begin(), chambers.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
        const auto min_w = min_element(chambers.begin(), chambers.end(), [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });
        const auto min_h = min_element(chambers.begin(), chambers.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });

        coords.push_back(std::make_pair(min_h->first, min_w->second));
        coords.push_back(std::make_pair(max_h->first, max_w->second));

        minMaxCoords.push_back(coords);
    }

    makePaths(minMaxCoords, floor);

    for (const auto &row : floor) {
        std::cout << row << std::endl;
    }

}
