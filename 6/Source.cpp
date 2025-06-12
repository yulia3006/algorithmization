#include <SFML/Graphics.hpp>
#include <set>
#include <functional>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;
using namespace sf;

const int GRID_SIZE = 10; // ������ ���� NxN
const int CELL_SIZE = 25;  // ������ ����� ������ � ��������
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;
// ��� ����� ��� smfl
Color Orange(229, 135, 34, 255);
Color Purple(90, 34, 139, 255);

// ���� �����
enum CellType {
    EMPTY,
    OBSTACLE,
    START,
    END,
    PATH,
    VISITED,
    QUEUED
};

// ��������� ��� ������������� ������
struct Cell {
    int x, y;
    CellType type;
    int f, g, h; // ��� ��������� A*
    Cell* parent; // ����� ��� ��������

    Cell(int x, int y) : x(x), y(y), type(EMPTY), f(0), g(0), h(0), parent(nullptr) {}

    // ������������ ��������� == ��� ��������� �����
    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
    // ��� ������������ ������ ��������� ����
    void F() {
        f = g + h;
    }
};

// ������� ��� ���������� ��������� (������������� ����������)
int heuristic(const Cell& a, const Cell& b) {
    return (abs(a.x - b.x) + abs(a.y - b.y));
}

// ������� ��� ��������, ��������� �� ������ � �������� �����
bool isValid(int x, int y) {
    return (GRID_SIZE > x) and (x >= 0) and (GRID_SIZE > y) and (y >= 0);
}
// ��� ���������
//� ����� � � ����� - ���������� �� ���� ������� � ��������� ���������������� ����� ������
void draw_grid(RenderWindow& window, vector<vector<Cell>>& grid) {
    for (int x = 0; x < grid.size(); x++) {
        for (int y = 0; y < grid.size(); y++) {
            RectangleShape cell(Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            switch (grid[x][y].type) {
            case EMPTY: cell.setFillColor(Color::White); break;
            case OBSTACLE: cell.setFillColor(Color::Black); break;
            case START: cell.setFillColor(Orange); break;
            case END: cell.setFillColor(Color::Cyan); break;
            case PATH: cell.setFillColor(Purple); break;
            case VISITED: cell.setFillColor(Color::Red); break;
            case QUEUED: cell.setFillColor(Color::Green); break;
            }
            cell.setOutlineColor(Color::Black);
            cell.setOutlineThickness(1);
            window.draw(cell);

        }
    }
}

//������ �����
vector<vector<Cell>> make_grid() {
    //������� ������
    vector<vector<Cell>> grid;
    //���������� ������
    int startX = 0;
    int startY = 1;
    int endX = 9;
    int endY = 9;
    //����� � �������������
    vector<vector<int>> gridObstacle = {
        {1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 0, 1, 0}
    };
    for (int x = 0; x < GRID_SIZE; x++) {

        vector<Cell> row;

        for (int y = 0; y < GRID_SIZE; y++) {
            Cell cell(x, y);
            cell.h = abs(x - endX) + abs(y - endY);
            cell.F();


            if (gridObstacle[y][x]) cell.type = OBSTACLE;
            row.push_back(cell);
        }
        grid.push_back(row);
    }
    Cell* start = &grid[startX][startY];
    Cell* end = &grid[endX][endY];

    start->type = START;
    start->g = 0;
    end->type = END;
    pair<Cell*, Cell*> p(start, end);

    return grid;
}


vector<pair<int, int>> A_Star(vector<vector<Cell>>& grid, Cell* start, Cell* goal) {
    // ������ ������������ ������� ��� ��������� ������, ��� ���� ����� ������� � ���������� ����������� ��������� f ������ ��� �� �������
    priority_queue<Cell*, vector<Cell*>, function<bool(Cell*, Cell*)>> open_list(
        [](Cell* a, Cell* b) { return a->f > b->f; }
    );
    // ������� �������� ������ ��� ������������� �����
    set<pair<int, int>> closed_list;
    // ��������� �����
    start->g = 0;// ��������� �� ������ � ��������� �����
    start->h = heuristic(*start, *goal); //  ������������� ������ ���� �� ������ �� ������
    start->F(); // ����������� ������ ���������
    open_list.push(start);// � ��������� � �������� ������
    // ���� �������������� �������� ������ �� ����, ���������� ���������� ������ � ���� ��������
    while (!open_list.empty()) {
        // �� ���� ������� � min F, � �������� �� ������
        Cell* current = open_list.top();
        open_list.pop();
        // ���� �������� ����, "�������" ���� � ��������������� ���� ���� 
        if (current->x == goal->x && current->y == goal->y) {
            vector<pair<int, int>> path;
            while (current != nullptr) {
                path.emplace_back(current->x, current->y);
                // � ������ ��� ������ �� PATH (����� START � END)
                if (current->type != START && current->type != END) {
                    grid[current->x][current->y].type = PATH;
                }
                //��������� current ��������������� �� ������������ ������
                current = current->parent;;
            }
            // ������ �������� ���� ����� ��� �������������� ����
            reverse(path.begin(), path.end());
            return path;
        }
        // � ������� ���������� ������� ������������ � �������� ������, ��������, ��� ��� ����������
        closed_list.insert({ current->x, current->y });
        if (grid[current->x][current->y].type != START && grid[current->x][current->y].type != END) {
            grid[current->x][current->y].type = VISITED;
        }
        // ������� ��� �������� ��� ������� ������
        cout << " " << current->x
            << " " << current->y
            << " F " << current->f << " "
            << "g -  " << current->g << "  "
            << "h -  " << current->h << endl;
        // �������� ������� ������
        for (auto [dx, dy] : vector<pair<int, int>>{ {-1, 0}, {1, 0}, {0, -1}, {0, 1} }) {
            int nx = current->x + dx;
            int ny = current->y + dy;
            // ��������, ��� ����� ������ ��������� � �������
            if (!isValid(nx, ny)) continue;
            Cell* neighbor = &grid[nx][ny];
            // continue, ���� ����������� � ���� ������ ��� ����������
            if (neighbor->type == OBSTACLE || closed_list.count({ nx, ny })) {
                continue;
            }
            // ������ g ��� ������
            int next_g = current->g + 1;
            // �������� ������ ��������� � �������� ������, ����� ���������� � �������� g � � ������� ����������� ������
            // ���� ������� �������� g ������ ������, �� ������ �� ������
            // � ��� ���� ����� �������� g ������, �� ��������� ����������, ������ �������� �� ������� ������
            if (next_g < neighbor->g || neighbor->g == 0) {
                neighbor->parent = current;
                neighbor->g = next_g;
                neighbor->h = heuristic(*neighbor, *goal);
                neighbor->F();
                // ���������� ������ � �������� ������ ��� ���������
                open_list.push(neighbor);
                if (neighbor->type != END) neighbor->type = QUEUED;
            }
        }
    }
    return {}; // ���� ���� - ���� ���� �� �������
}
int main() {
    // �������� ������ ��� ������ ������
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return EXIT_FAILURE;
    }

    // ��������� ������ ��� ������ �������
    sf::Text statusText;
    statusText.setFont(font);


    vector<vector<Cell>> grid;
    grid = make_grid();
    Cell* start = &grid[0][7];
    Cell* goal = &grid[9][9];

    //�������� ���� � ������ ���������
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), L" ");
    bool pathFound = false;
    vector<pair<int, int>> path;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (event.key.scancode == Keyboard::Scan::Space) { //  A* 
                    auto path = A_Star(grid, start, goal);
                }
            }
        }
        window.clear();
        draw_grid(window, grid);
        window.display();
    }
    return 0;
}