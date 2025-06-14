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

const int GRID_SIZE = 10; // Ðàçìåð ïîëÿ NxN
const int CELL_SIZE = 25;  // Ðàçìåð îäíîé ÿ÷åéêè â ïèêñåëÿõ
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;
// äîï öâåòà äëÿ smfl
Color Orange(229, 135, 34, 255);
Color Purple(90, 34, 139, 255);

// Òèïû ÿ÷ååê
enum CellType {
    EMPTY,
    OBSTACLE,
    START,
    END,
    PATH,
    VISITED,
    QUEUED
};

// Ñòðóêòóðà äëÿ ïðåäñòàâëåíèÿ ÿ÷åéêè
struct Cell {
    int x, y;
    CellType type;
    int f, g, h; // Äëÿ àëãîðèòìà A*
    Cell* parent; // àäðåñ äëÿ ðîäèòåëÿ

    Cell(int x, int y) : x(x), y(y), type(EMPTY), f(0), g(0), h(0), parent(nullptr) {}

    // Ïåðåçàãðóçêà îïåðàòîðà == äëÿ ñðàâíåíèÿ ÿ÷ååê
    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
    // äëÿ âûñ÷èòûâàíèÿ ïîëíîé ñòîèìîñòè ïóòè
    void F() {
        f = g + h;
    }
};

// Ôóíêöèÿ äëÿ âû÷èñëåíèÿ ýâðèñòèêè (ìàíõýòòåíñêîå ðàññòîÿíèå)
int heuristic(const Cell& a, const Cell& b) {
    return (abs(a.x - b.x) + abs(a.y - b.y));
}

// Ôóíêöèÿ äëÿ ïðîâåðêè, íàõîäèòñÿ ëè ÿ÷åéêà â ïðåäåëàõ ñåòêè
bool isValid(int x, int y) {
    return (GRID_SIZE > x) and (x >= 0) and (GRID_SIZE > y) and (y >= 0);
}
// äëÿ îòðèñîâêè
//â öåëîì è â îáùåì - ïðîõîäèìñÿ ïî âñåì êëåòêàì è çàïîëíÿåì ñîîòâåòñòâóþùèìè òèïàì öâåòîì
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

//ñîçäà¸ì ñåòêó
vector<vector<Cell>> make_grid() {
    //ñîçäàåì êëåòêó
    vector<vector<Cell>> grid;
    //êîîðäèíàòû íà÷àëà
    int startX = 0;
    int startY = 1;
    int endX = 9;
    int endY = 9;
    //êàðòà ñ ïðåïÿòñòâèÿìè
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
    // ñîçäà¸ì ïðèîðèòåòíóþ î÷åðåäü äëÿ îòêðûòîãî ñïèñêà, äëÿ òîãî ÷òîáû ýëåìåíò ñ íàèìåíüøèì ïîêàçàòåëåì ñòîèìîñòè f âñåãäà áûë íà âåðøèíå
    priority_queue<Cell*, vector<Cell*>, function<bool(Cell*, Cell*)>> open_list(
        [](Cell* a, Cell* b) { return a->f > b->f; }
    );
    // ñîçäàåì çàêðûòûé ñïèñîê äëÿ ðàññìîòðåííûõ òî÷åê
    set<pair<int, int>> closed_list;
    // íà÷àëüíàÿ òî÷êà
    start->g = 0;// ñòîèìîñòü äî ôèíèøà ñ íà÷àëüíîé òî÷êè
    start->h = heuristic(*start, *goal); //  ýâðèñòè÷åñêàÿ îöåíêà ïóòè äî ôèíèøà îò íà÷àëà
    start->F(); // âûñ÷èòûâàåì ïîëíóþ ñòîèìîñòü
    open_list.push(start);// è äîáàâëÿåì â îòêðûòûé ñïèñîê
    // ïîêà ñîîòâåòñòâåííî îòêðûòûé ñïèñîê íå ïóñò, ïðîèñõîäèò îáíîâëåíèå äàííûõ î âñåõ âåðøèíàõ
    while (!open_list.empty()) {
        // íà âõîä âåðøèíà ñ min F, è óäàëåòñÿ èç ñïèñêà
        Cell* current = open_list.top();
        open_list.pop();
        // åñëè äîñòèãëè öåëè, "ñîçäàåì" ïóòü è âîññòàíàâëèâàåì ýòîò ïóòü 
        if (current->x == goal->x && current->y == goal->y) {
            vector<pair<int, int>> path;
            while (current != nullptr) {
                path.emplace_back(current->x, current->y);
                // è ìåíÿåì òèï ÿ÷åéêè íà PATH (êðîìå START è END)
                if (current->type != START && current->type != END) {
                    grid[current->x][current->y].type = PATH;
                }
                //óêàçàòåëü current ïåðåíàçíà÷àåòñÿ íà ðîäèòåëüñêèé îáúåêò
                current = current->parent;;
            }
            // äåëàåì ðåâ¸ðñèíã ïóòè òàêæå äëÿ âîññòàíîâëåíèÿ ïóòè
            reverse(path.begin(), path.end());
            return path;
        }
        // è òåêóùàÿ ïîñåù¸ííàÿ âåðøèíà íàïðàâëÿåòñÿ â çàêðûòûé ñïèñîê, ïîìå÷àåì, ÷òî óæå ïîñåù¸ííàÿ
        closed_list.insert({ current->x, current->y });
        if (grid[current->x][current->y].type != START && grid[current->x][current->y].type != END) {
            grid[current->x][current->y].type = VISITED;
        }
        // âûâîäèì âñå çíà÷åíèÿ äëÿ òåêóùåé êëåòêè
        cout << " " << current->x
            << " " << current->y
            << " F " << current->f << " "
            << "g -  " << current->g << "  "
            << "h -  " << current->h << endl;
        // ïðîâåðêà ñîñåäåé êëåòêè
        for (auto [dx, dy] : vector<pair<int, int>>{ {-1, 0}, {1, 0}, {0, -1}, {0, 1} }) {
            int nx = current->x + dx;
            int ny = current->y + dy;
            // ïðîâåðêà, ÷òî ñîñåä âîîáùå íàõîäèòñÿ â ìàòðèöå
            if (!isValid(nx, ny)) continue;
            Cell* neighbor = &grid[nx][ny];
            // continue, åñëè ïðåïÿòñòâèå è åñëè êëåòêà óæå ïîñåùåííàÿ
            if (neighbor->type == OBSTACLE || closed_list.count({ nx, ny })) {
                continue;
            }
            // ðàñ÷åò g äëÿ êëåòêè
            int next_g = current->g + 1;
            // ñîñåäíÿÿ êëåòêà íàõîäèòñÿ â îòêðûòîì ñïèñêå, òîãäà ñðàâíèâàåì å¸ çíà÷åíèå g è ó òåêóùåé ïðîâåðÿåìîé êëåòêè
            // åñëè ïðåæíåå çíà÷åíèå g ìåíüøå íîâîãî, òî íè÷åãî íå äåëàåì
            // à âîò åñëè íîâîå çíà÷åíèå g ìåíüøå, òî îáíîâëÿåì ïåðåìåííóþ, ìåíÿåì ðîäèòåëÿ íà òåêóùóþ êëåòêó
            if (next_g < neighbor->g || neighbor->g == 0) {
                neighbor->parent = current;
                neighbor->g = next_g;
                neighbor->h = heuristic(*neighbor, *goal);
                neighbor->F();
                // äîáàâëåíèå êëåòêè â îòêðûòûé ñïèñîê äëÿ ïîñåùåíèÿ
                open_list.push(neighbor);
                if (neighbor->type != END) neighbor->type = QUEUED;
            }
        }
    }
    return {}; // èíîé ïóòü - åñëè ïóòè íå íàøëîñü
}
int main() {
    // çàãðóçêà øðèôòà äëÿ âûâîäà òåêñòà
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return EXIT_FAILURE;
    }

    // íàñòðîéêà òåêñòà äëÿ âûâîäà ñòàòóñà
    sf::Text statusText;
    statusText.setFont(font);


    vector<vector<Cell>> grid;
    grid = make_grid();
    Cell* start = &grid[0][1];
    Cell* goal = &grid[9][9];

    //ñîçäàíèå îêíà è ðàáîòà àëãîðèòìà
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
