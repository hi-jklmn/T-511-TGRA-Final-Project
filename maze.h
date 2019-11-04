#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <stack>
#include <cstdlib>
#include <algorithm>

using namespace std;

class Maze {
public:
    enum TileType {
        FLOOR,
        WALL,
        STATUE
    };

    vector<vector<TileType> > tiles;
    glm::vec3 start_location;
    glm::vec3 end_location;

    static Maze Default(int width, int height) {
        Maze m;
        m.tiles = gen_maze(width, height);
        return m;
    }

    //private:
    static vector<vector<TileType> > gen_maze(int w, int h) {
        struct P {
            int x, y;
        };

        // Goddamn MSVC doesn't understand runtime sized stack-allocated arrays
        // bool marked[w][h] <-- apparently only available on GCC >:(
        // This irritates me a lot because this makes the code considerably less performant though it doesn't matter much here
        vector<vector<bool>> marked(w);

        for (int i = 0; i < w; i++) {
            marked[i].resize(h);
            for (int j = 0; j < h; j++) {
                marked[i][j] = false;
            }
        }

        P mdim;
        mdim.x = 2 * w + 1;
        mdim.y = 2 * h + 1;

        vector<vector<TileType>> maze(mdim.x);

        for (int i = 0; i < mdim.x; i++) {
            maze[i].resize(mdim.y);
            for (int j = 0; j < mdim.y; j++) {
                maze[i][j] = (rand() % 10) < 9 ? TileType::WALL : TileType::STATUE;
            }
        }

        P start;
        start.x = w / 2;
        start.y = h / 2;

        stack<P> s;
        s.push(start);

        marked[start.x][start.y] = true;

        while (!s.empty()) {
            P curr = s.top();
            s.pop();

            maze[2 * curr.x + 1][2 * curr.y + 1] = TileType::FLOOR;

            vector<P> nbors;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (j == 0 && i == 0) continue;
                    if (j != 0 && i != 0) continue;

                    P n;
                    n.x = curr.x + i;
                    n.y = curr.y + j;

                    if (n.x < 0 || n.x >= w) continue;
                    if (n.y < 0 || n.y >= h) continue;
                    if (marked[n.x][n.y]) continue;

                    marked[n.x][n.y] = true;

                    maze[2 * curr.x + 1 + i][2 * curr.y + 1 + j] = TileType::FLOOR;
                    nbors.push_back(n);
                }
            }

            random_shuffle(nbors.begin(), nbors.end());

            for (P n : nbors) {
                s.push(n);
            }
        }

        vector<vector<TileType>> out;

        for (int i = 0; i < mdim.x; i++) {
            vector<TileType> temp;
            for (int j = 0; j < mdim.y; j++) {
                temp.push_back(maze[i][j]);
            }
            out.push_back(temp);
        }

        return out;
    }
};

#endif
