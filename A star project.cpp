#include <iostream>
#include <list>
#include <algorithm>

// Class representing a 2D point
class point {
public:
    point(int a = 0, int b = 0) { x = a; y = b; }
    bool operator ==(const point& o) const { return o.x == x && o.y == y; }
    point operator +(const point& o) const { return point(o.x + x, o.y + y); }
    int x, y;
};

// Class representing a 2D map
class map {
public:
    map() {
        // Initialize a 2D array to represent the map
        char t[20][20] = {
            {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 1, 1, 0}, {0, 0, 1, 0, 0, 0, 1, 0},
            {1, 1, 1, 0, 0, 0, 1, 0}, {0, 0, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 1, 1, 0}, {0, 0, 1, 0, 0, 0, 1, 0},
            {0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 1, 1, 0},
        };
        w = h = 20;
        // Copy the values from the array to the map
        for (int r = 0; r < h; r++)
            for (int s = 0; s < w; s++)
                m[s][r] = t[r][s];
    }
    // Function to access the map value at a specific location
    int operator()(int x, int y) { return m[x][y]; }
    char m[20][20]; // 2D array representing the map
    int w, h; // Width and height of the map
};

// Class representing a node in the A* algorithm
class node {
public:
    bool operator == (const node& o) const { return pos == o.pos; }
    bool operator == (const point& o) const { return pos == o; }
    bool operator < (const node& o) const { return dist + cost < o.dist + o.cost; }
    point pos, parent; // Position and parent of the node
    int dist, cost; // Distance and cost of the node
};

// Class implementing the A* algorithm
class aStar {
public:
    aStar() {
        // Define possible neighbors (movements) for a given position
        neighbours[0] = point(-1, -1); neighbours[1] = point(1, -1);
        neighbours[2] = point(-1, 1); neighbours[3] = point(1, 1);
        neighbours[4] = point(0, -1); neighbours[5] = point(-1, 0);
        neighbours[6] = point(0, 1); neighbours[7] = point(1, 0);
    }

    // Calculate heuristic distance between two points
    int calcDist(point& p) const {
        // Need a better heuristic; currently using Euclidean distance
        int x = end.x - p.x, y = end.y - p.y;
        return (x * x + y * y);
    }

    // Check if a point is within the bounds of the map
    bool isValid(point& p) const {
        return (p.x >= 0 && p.y >= 0 && p.x < m.w && p.y < m.h);
    }

    // Check if a point already exists in the open or closed lists and update if needed
    bool existPoint(point& p, int cost) {
        auto i = std::find(closed.begin(), closed.end(), p);
        if (i != closed.end()) {
            if ((*i).cost + (*i).dist < cost) return true;
            else { closed.erase(i); return false; }
        }
        i = std::find(open.begin(), open.end(), p);
        if (i != open.end()) {
            if ((*i).cost + (*i).dist < cost) return true;
            else { open.erase(i); return false; }
        }
        return false;
    }

    // Generate new nodes and add them to the open list
    bool fillOpen(node& n) {
        int stepCost, nc, dist;
        point neighbour;

        for (int x = 0; x < 8; x++) {
            // One can make diagonals have different cost
            stepCost = x < 4 ? 1 : 1;
            neighbour = n.pos + neighbours[x];
            if (neighbour == end) return true;

            if (isValid(neighbour) && m(neighbour.x, neighbour.y) != 1) {
                nc = stepCost + n.cost;
                dist = calcDist(neighbour);
                if (!existPoint(neighbour, nc + dist)) {
                    node m;
                    m.cost = nc; m.dist = dist;
                    m.pos = neighbour;
                    m.parent = n.pos;
                    open.push_back(m);
                }
            }
        }
        return false;
    }

    // Perform A* search
    bool search(point& s, point& e, map& mp) {
        node n; end = e; start = s; m = mp;
        n.cost = 0; n.pos = s; n.parent = point(-1, -1); n.dist = calcDist(s);
        open.push_back(n);
        while (!open.empty()) {
            open.sort();
            node n = open.front();
            open.pop_front();
            closed.push_back(n);
            if (fillOpen(n)) return true;
        }
        return false;
    }

    // Reconstruct the path from the closed list
    int path(std::list<point>& path) const {
        path.push_front(end);
        int cost = 1 + closed.back().cost;
        path.push_front(closed.back().pos);
        point parent = closed.back().parent;

        for (auto i = closed.rbegin(); i != closed.rend(); i++) {
            if ((*i).pos == parent && !((*i).pos == start)) {
                path.push_front((*i).pos);
                parent = (*i).parent;
            }
        }
        path.push_front(start);
        return cost;
    }

    map m; point end, start;
    point neighbours[8];
    std::list<node> open;
    std::list<node> closed;
};

// Main function
int main(int argc, char* argv[]) {
    map m;
    point s, e;

    // Get user input for the starting point
    std::cout << "Enter starting point (x y): ";
    std::cin >> s.x >> s.y;

    // Get user input for the ending point
    std::cout << "Enter ending point (x y): ";
    std::cin >> e.x >> e.y;

    aStar as;

    if (as.search(s, e, m)) {
        std::list<point> path;
        int c = as.path(path);
        for (int y = 0; y < m.h; y++) {
            for (int x = 0; x < m.w; x++) {
                if (m(x, y) == 1)
                    std::cout << char(0xdb);
                else {
                    if (std::find(path.begin(), path.end(), point(x, y)) != path.end())
                        std::cout << "x";
                    else
                        std::cout << ".";
                }
            }
            std::cout << "\n";
        }

        std::cout << "\nPath cost " << c << ": ";
        for (auto i = path.begin(); i != path.end(); i++) {
            std::cout << "(" << (*i).x << ", " << (*i).y << ") ";
        }
    }
    std::cout << "\n\n";
    return 0;
}
