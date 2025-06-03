#include "pathfinding.h"
#include "global.h"
#include <queue>
#include <climits>

struct node_t {
    int x, y, dist;
};

struct NodeComparator {
    bool operator()(const node_t &a, const node_t &b) const {
        return a.dist > b.dist;
    }
};

void djikstraForTunnel(int sx, int sy) {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            disTunneling[i][j] = INT_MAX;
    disTunneling[sy][sx] = 0;
    std::priority_queue<node_t, std::vector<node_t>, NodeComparator> pq;
    pq.push({sx, sy, 0});
    int dirs[8][2] = { {-1,0}, {1,0}, {0,-1}, {0,1}, {-1,-1}, {-1,1}, {1,-1}, {1,1} };
    while (!pq.empty()) {
        node_t u = pq.top();
        pq.pop();
        if (u.dist > disTunneling[u.y][u.x])
            continue;
        for (int i = 0; i < 8; i++) {
            int nx = u.x + dirs[i][0];
            int ny = u.y + dirs[i][1];
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
                continue;
            if (hardness[ny][nx] != 255) {
                int cost = 1;
                if (hardness[ny][nx] > 0 && hardness[ny][nx] < 255)
                    cost += hardness[ny][nx] / 85;
                int alt = u.dist + cost;
                if (alt < disTunneling[ny][nx]) {
                    disTunneling[ny][nx] = alt;
                    pq.push({nx, ny, alt});
                }
            }
        }
    }
}

void djikstraForNonTunnel(int sx, int sy) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            disNonTunneling[r][c] = INT_MAX;
    disNonTunneling[sy][sx] = 0;
    std::priority_queue<node_t, std::vector<node_t>, NodeComparator> pq;
    pq.push({sx, sy, 0});
    int dirs[8][2] = { {-1,0}, {1,0}, {0,-1}, {0,1}, {-1,-1}, {-1,1}, {1,-1}, {1,1} };
    while (!pq.empty()) {
        node_t u = pq.top();
        pq.pop();
        if (u.dist > disNonTunneling[u.y][u.x])
            continue;
        for (int i = 0; i < 8; i++) {
            int nx = u.x + dirs[i][0];
            int ny = u.y + dirs[i][1];
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
                continue;
            // non-tunneling monsters can only move where hardness == 0.
            if (hardness[ny][nx] == 0) {
                int alt = u.dist + 1;
                if (alt < disNonTunneling[ny][nx]) {
                    disNonTunneling[ny][nx] = alt;
                    pq.push({nx, ny, alt});
                }
            }
        }
    }
}
