
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <memory>


using namespace std;

const int BOARD_WIDTH = 80;
const int BOARD_HEIGHT = 25;

std::string red = "\033[31m";
std::string green = "\033[32m";
std::string yellow = "\033[33m";
std::string blue = "\033[34m";


class Shapes {
protected:
    int id;
    int x, y;
    string shape;
    string color;
    string fillMode;
public:
    Shapes(string s, int id, int x, int y, string color, string fillMode)
        : shape(s), id(id), x(x), y(y), color(color), fillMode(fillMode) {}    
    virtual string getInfo() const = 0;
    virtual string getShape() const { return shape; }
    int getID() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) {
        x = newX; 
    }
    void setY(int newY) {
        y = newY; 
    }
    string getColor() const { return color; }
    string getFillMode() const { return fillMode; }
    virtual void drawOnBoard(vector<vector<string>>& grid) const = 0;
    virtual string getLoad() const = 0;
    virtual ~Shapes() {}
    virtual bool containsPoint(int x, int y) const = 0;
    virtual void setColor(const std::string& newColor) {
        color = newColor;
    }
    static std::string getColorCode(const std::string& colorName) {
        std::unordered_map<std::string, std::string> colorCodes = {
            {"black", "0"},
            {"red", "1"},
            {"green", "2"},
            {"yellow", "3"},
            {"blue", "4"},
            {"magenta", "5"},
            {"cyan", "6"},
            {"white", "7"}
        };

        if (colorCodes.find(colorName) != colorCodes.end()) {
            return "\033[3" + colorCodes[colorName] + "m"; // Text color only
        }
        else {
            return ""; // Return empty string if the color is not found
        }
    }
};

class Triangle : public Shapes {
private:
    double  height;
public:
    Triangle(int id, int x, int y, double h, string color, string fillMode)
        : Shapes("Triangle", id, x, y, color, fillMode), height(h) {}
    bool containsPoint(int px, int py) const override {
        // Calculate the vertices of the triangle
        int x1 = x;        
        int y1 = y;        
        int x2 = x - (height / 2); 
        int y2 = y + height;       
        int x3 = x + (height / 2); 
        int y3 = y + height;       

        // Area of the triangle ABC
        double areaABC = abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);

        // Area of triangle PAB
        double areaPAB = abs((px * (y1 - y2) + x1 * (y2 - py) + x2 * (py - y1)) / 2.0);
        // Area of triangle PBC
        double areaPBC = abs((px * (y2 - y3) + x2 * (y3 - py) + x3 * (py - y2)) / 2.0);
        // Area of triangle PCA
        double areaPCA = abs((px * (y3 - y1) + x3 * (y1 - py) + x1 * (py - y3)) / 2.0);

        // Check if the sum of PAB, PBC, and PCA is equal to ABC
        return (areaPAB + areaPBC + areaPCA == areaABC);
    }
    string getShape() const override {
        return "triangle";
    }
    string getInfo() const override {
        return "Triangle: ID=" + to_string(id) +
            " Height=" + to_string(height) +
            " Color=" + color + " FillMode=" + fillMode + " at (" + to_string(x) + "," + to_string(y) + ")";
    }
    string getLoad() const override {
        return "Triangle: " + to_string(id) + " " + to_string(x) + " " + to_string(y) + " " + to_string(height) + " " + color + " " + fillMode;
    }
    void drawOnBoard(vector<vector<string>>& grid) const override {
        int startX = x;
        int startY = y;
        int heightInt = height;
        char symbolB = color.empty() ? '*' : color[0]; // Default symbol
        std::string colorCode = getColorCode(color); // Get the ANSI color code for the shape's color
        std::string resetCode = "\033[0m"; // ANSI code to reset color
        string symbol = colorCode + std::string(1, symbolB) + resetCode;
        // Apply the color code, draw the shape, and then reset color
        
        for (int i = 0; i < heightInt; ++i) {
            int leftMost = startX - i;
            int rightMost = startX + i;
            int posY = startY + i;

            if (posY < BOARD_HEIGHT) {
                if (fillMode == "fill") {
                    // Fill between leftMost and rightMost
                    for (int j = leftMost; j <= rightMost; ++j) {
                        if (j >= 0 && j < BOARD_WIDTH) {
                            grid[posY][j]=symbol; // Set the symbol
                        }
                    }
                }

                // Set leftmost border
                if (leftMost >= 0 && leftMost < BOARD_WIDTH) {
                    grid[posY][leftMost]=symbol; // Set the symbol for left border
                }

                // Set rightmost border
                if (rightMost >= 0 && rightMost < BOARD_WIDTH && leftMost != rightMost) {
                    grid[posY][rightMost]=symbol; // Set the symbol for right border
                }
            }
        }

        // Draw the base of the triangle
        for (int j = 0; j < 2 * heightInt - 1; ++j) {
            int baseX = x - heightInt + 1 + j;
            int baseY = y + heightInt - 1;
            if (baseX >= 0 && baseX < BOARD_WIDTH && baseY < BOARD_HEIGHT) {
                grid[baseY][baseX] = symbol; // Set the symbol for the base
            }
        }
        
    }
    void setDimensions( double h) {
        height = h;
    }
    void setColor(const std::string& newColor) override {
        color = newColor;
    }
};

class Circle : public Shapes {
private:
    double radius;
public:
    Circle(int id, int x, int y, double r, string color, string fillMode) : Shapes("Circle", id, x, y, color, fillMode), radius(r) {}
    bool containsPoint(int pointX, int pointY) const override {
        double distance = sqrt(pow(pointX - this->x, 2) + pow(pointY - this->y, 2));
        return distance <= radius;
    }
    string getInfo() const override {
        return "Circle: ID=" + to_string(id) + " Radius=" + to_string(radius) +
            " Color=" + color + " FillMode=" + fillMode +
            " at (" + to_string(x) + "," + to_string(y) + ")";
    }
    string getLoad() const override {
        return "Circle: " + to_string(id) + " " + to_string(x) + " " + to_string(y) + " " + to_string(radius) + " " + color + " " + fillMode;
    }
    string getShape() const override {
        return "circle";
    }
    void drawOnBoard(vector<vector<string>>& grid) const {
        std::string colorCode = getColorCode(color);
        std::string resetCode = "\033[0m";

        char symbolChar = (color.empty() ? '*' : color[0]); // Rename this to avoid conflict
        std::string coloredSymbol = colorCode + std::string(1, symbolChar) + resetCode;

        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {
                int posX = x + i; // Adjust X coordinate
                int posY = y + j; // Adjust Y coordinate

                if (posX >= 0 && posX < grid[0].size() && posY >= 0 && posY < grid.size()) {
                    double dist = i * i + j * j;

                    // Fill the circle
                    if (fillMode == "fill") {
                        if (dist <= radius * radius) {
                           grid[posY][posX]= coloredSymbol;        // Store symbol
                        }
                    }
                    // Draw the frame of the circle
                    else if (fillMode == "frame") {
                        if (dist >= (radius - 0.5) * (radius - 0.5) && dist <= (radius + 0.5) * (radius + 0.5)) {
                           grid[posY][posX]= coloredSymbol;        // Store symbol
                        }
                    }
                }
            }
        }

    }

    void setDimensions( double r) {
        radius = r;
    }
    void setColor(const std::string& newColor) override {
        color = newColor;
    }
};
class Square : public Shapes {
private:
    double side;
public:
    Square(int id, int x, int y, double s, string color, string fillMode)
        : Shapes("Square", id, x, y, color, fillMode), side(s) {}
    bool containsPoint(int pointX, int pointY) const override {
        int halfSide = static_cast<int>(side / 2); // Calculate half side for checking
        return pointX >= (x - halfSide) && pointX <= (x + halfSide) &&
            pointY >= (y - halfSide) && pointY <= (y + halfSide);
    }
    string getInfo() const override {
        return "Square: ID=" + to_string(id) + " Side=" + to_string(side) +
            " Color=" + color + " FillMode=" + fillMode +
            " at (" + to_string(x) + "," + to_string(y) + ")";
    }

    string getLoad() const override {
        return "Square: " + to_string(id) + " " + to_string(x) + " " + to_string(y) +
            " " + to_string(side) + " " + color + " " + fillMode;
    }
    string getShape() const override {
        return "square";
    }
    void setColor(const std::string& newColor) override {
        color = newColor;
    }
    void setDimensions(double s) {
        side = s;
    }
    void drawOnBoard(vector<vector<string>>& grid) const override {
        int startX = x;
        int startY = y;
        int sideInt = side;

        std::string colorCode = getColorCode(color);
        std::string resetCode = "\033[0m";
        char symbolB = color.empty() ? '*' : color[0]; // Default symbol if color is empty
        string symbol = colorCode + std::string(1, symbolB) + resetCode;
        // Fill the square
        if (fillMode == "fill") {
            for (int i = 0; i < sideInt; ++i) {
                for (int j = 0; j < sideInt; ++j) {
                    int posX = startX + j;
                    int posY = startY + i;

                    if (posX >= 0 && posX < BOARD_WIDTH && posY >= 0 && posY < BOARD_HEIGHT) {
                        grid[posY][posX]=symbol; // Store the symbol
                    }
                }
            }
        }

        // Draw the borders of the square
        for (int i = 0; i < sideInt; ++i) {
            // Top border
            if (startY >= 0 && startY < BOARD_HEIGHT) {
                if (startX + i >= 0 && startX + i < BOARD_WIDTH) {
                    grid[startY][startX + i]=symbol; // Store the symbol for top border
                }

                // Bottom border
                if (startY + sideInt - 1 >= 0 && startY + sideInt - 1 < BOARD_HEIGHT &&
                    startX + i >= 0 && startX + i < BOARD_WIDTH) {
                    grid[startY + sideInt - 1][startX + i]=symbol; // Store the symbol for bottom border
                }
            }
        }

        for (int i = 0; i < sideInt; ++i) {
            // Left border
            if (startY + i >= 0 && startY + i < BOARD_HEIGHT && startX >= 0 && startX < BOARD_WIDTH) {
                grid[startY + i][startX]=symbol; // Store the symbol for left border
            }

            // Right border
            if (startY + i >= 0 && startY + i < BOARD_HEIGHT &&
                startX + sideInt - 1 >= 0 && startX + sideInt - 1 < BOARD_WIDTH) {
                grid[startY + i][startX + sideInt - 1]=symbol; // Store the symbol for right border
            }
        }

    }

};
class Rectangle : public Shapes {
private:
    double  width, height;
public:
    Rectangle(int id, int x, int y, double w, double h, string color, string fillMode) : Shapes("Rectangle", id, x, y, color, fillMode), width(w), height(h) {}
    bool containsPoint(int pointX, int pointY) const override {
        int halfWidth = static_cast<int>(width / 2);
        int halfHeight = static_cast<int>(height / 2);

        return pointX >= (x - halfWidth) && pointX <= (x + halfWidth) &&
            pointY >= (y - halfHeight) && pointY <= (y + halfHeight);
    }
    string getInfo() const override {
        return "Rectangle: top-left corner (" + std::to_string(x) + ", " + std::to_string(y) +
            "), width " + std::to_string(width) + ", height " + std::to_string(height) + " Color=" + color + " FillMode=" + fillMode;
    }
    string getLoad() const override {
        return "Rectangle: " + to_string(getID()) + " " + to_string(getX()) + " " + to_string(getY()) + " " + to_string(width) + " " + to_string(height) + " " + color + " " + fillMode;
    }
    string getShape() const override {
        return "rectangle";
    }
    void drawOnBoard(vector<vector<string>>& grid) const override {
        int startX = x;
        int startY = y;
        int heightInt = height;
        int widthInt = width;
        std::string colorCode = getColorCode(color);
        std::string resetCode = "\033[0m";
        char colorSymbol = color.empty() ? '*' : color[0]; // Default to '*' if color is empty
        string symbol = colorCode + std::string(1, colorSymbol) + resetCode;

        // Fill the rectangle
        if (fillMode == "fill") {
            for (int i = 0; i < heightInt; ++i) {
                for (int j = 0; j < widthInt; ++j) {
                    int posX = startX + j; // Calculate x position
                    int posY = startY + i; // Calculate y position

                    // Check bounds before filling
                    if (posX >= 0 && posX < grid[0].size() && posY < grid.size()) {
                        grid[posY][posX]=colorSymbol; // Fill the rectangle with the color symbol
                    }
                }
            }
        }

        // Optionally, you can still draw the outline if needed
        for (int i = 0; i < widthInt; ++i) {
            // Top border
            if (startY < grid.size()) {
                if (startX + i < grid[0].size()) {
                    grid[startY][startX + i]=colorSymbol; // Top side
                }

                // Bottom border
                if (startY + heightInt - 1 < grid.size() && startX + i < grid[0].size()) {
                    grid[startY + heightInt - 1][startX + i]=colorSymbol; // Bottom side
                }
            }
        }

        for (int i = 0; i < heightInt; ++i) {
            // Left border
            if (startY + i < grid.size() && startX < grid[0].size()) {
                grid[startY + i][startX]=colorSymbol; // Left side
            }

            // Right border
            if (startY + i < grid.size() && startX + widthInt - 1 < grid[0].size()) {
                grid[startY + i][startX + widthInt - 1]=colorSymbol; // Right side
            }
        }
    }

    void setDimensions(double w, double h) {
        width = w;
        height = h;
    }
    void setColor(const std::string& newColor) override {
        color = newColor;
    }
};

class Line : public Shapes {
private:
    int x1, y1, x2, y2;
public:

    Line(int id, int x1, int y1, int x2, int y2, string color, string fillMode)
        : Shapes("Line", id, x1, y1, color, fillMode), x1(x1), y1(y1), x2(x2), y2(y2) {}
    bool containsPoint(int x, int y) const override{
        // Check if (x, y) is on the line segment from (x1, y1) to (x2, y2)
        return (y - y1) * (x2 - x1) == (y2 - y1) * (x - x1) && // Collinear check
            x >= min(x1, x2) && x <= max(x1, x2) &&
            y >= min(y1, y2) && y <= max(y1, y2);
    }
    void setDimensions(int x, int y, int z, int t) {
        x1 = x;
        y1 = y;
        x2 = z;
        y2 = t;
    }
    void drawOnBoard(vector<vector<string>>& grid) const override {
        std::string colorCode = getColorCode(color);
        std::string resetCode = "\033[0m";
        char symbolB = color.empty() ? '*' : color[0]; // Default to '*' if color is empty
        string symbol = colorCode + std::string(1, symbolB) + resetCode;
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        int x = x1;
        int y = y1;

        while (true) {
            if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
                grid[y][x] = symbol;
            }

            if (x == x2 && y == y2) break;

            int e2 = err;

            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }

            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }
    void setColor(const std::string& newColor) override {
        color = newColor;
    }

 
    string getInfo() const override {
        return "Line: from (" + std::to_string(x1) + ", " + std::to_string(y1) + ") to (" +
            std::to_string(x2) + ", " + std::to_string(y2) + ")" +
            " Color=" + color + " FillMode=" + fillMode;
    }
    string getLoad() const override {
        return "Line: " + std::to_string(id) + " " + std::to_string(x1) + " " +
            std::to_string(y1) + " " + std::to_string(x2) + " " + std::to_string(y2) + " " + color + " " + fillMode;
    }
};

class Board {
private:
    vector<vector<string>> grid; 
    map<int, std::unique_ptr<Shapes>> shapes;    
    int nextID;
    int lastSelectedId;
public:
    Board() : grid(BOARD_HEIGHT, std::vector<std::string>(BOARD_WIDTH, " ")), nextID(0), lastSelectedId(-1) {}

    bool isOccupied(int x, int y, const string& type, double param1 = 0, double param2 = 0) {
        for (const auto& shapePair : shapes) {
            const Shapes* shape = shapePair.second.get();
            if (shape->getShape() == type) {
                double shapeX = shape->getX();
                double shapeY = shape->getY();

                if (shapeX == x && shapeY == y) {
                    if (type == "circle") {
                        const Circle* circle = dynamic_cast<const Circle*>(shape);
                        if (circle && circle->getShape() == type && circle->getX() == x && circle->getY() == y && circle->getInfo().find(to_string(param1)) != string::npos) {
                            return true;
                        }
                    }
                    else if (type == "square") {
                        const Square* square = dynamic_cast<const Square*>(shape);
                        if (square && square->getShape() == type && square->getX() == x && square->getY() == y && square->getInfo().find(to_string(param1)) != string::npos) {
                            return true;
                        }
                    }
                    else if (type == "triangle") {
                        const Triangle* triangle = dynamic_cast<const Triangle*>(shape);
                        if (triangle && triangle->getShape() == type && triangle->getX() == x && triangle->getY() == y && triangle->getInfo().find(to_string(param1)) != string::npos) {
                            return true;
                        }
                    }
                    else if (type == "rectangle") {
                        const Rectangle* rectangle = dynamic_cast<const Rectangle*>(shape);

                        if (rectangle && rectangle->getShape() == type && rectangle->getX() == x && rectangle->getY() == y &&
                            rectangle->getInfo().find(to_string(param1)) != string::npos && rectangle->getInfo().find(to_string(param2)) != string::npos) {
                            return true;
                        }
                    }
                    // Додавання перевірки для лінії
                    else if (type == "line") {
                        const Line* line = dynamic_cast<const Line*>(shape);
                        if (line && line->getShape() == type &&
                            line->getInfo().find(to_string(x)) != string::npos &&
                            line->getInfo().find(to_string(y)) != string::npos &&
                            line->getInfo().find(to_string(param1)) != string::npos &&
                            line->getInfo().find(to_string(param2)) != string::npos) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void print() {
        cout << "+";
        for (int i = 0; i < BOARD_WIDTH; ++i) {
            cout << "-";
        }
        cout << "+" << "\n";

        for (const auto& row : grid) {
            cout << "|";
            for (const string& c : row) {
                cout << c;
            }
            cout << "|" << "\n";
        }

        std::cout << "+";
        for (int i = 0; i < BOARD_WIDTH; ++i) {
            std::cout << "-";
        }
        std::cout << "+" << "\n";
    }
    void draw() {
        for (auto& row : grid) {
            fill(row.begin(), row.end(), " ");
        }

        for (const auto& shapePair : shapes) {
            shapePair.second->drawOnBoard(grid);
        }
    }

    void list() {
        if (shapes.empty()) {
            cout << "No shapes added yet.\n";
        }
        else {
            cout << "List of shapes on the board:\n";
            for (const auto& shapePair : shapes) {
                cout << shapePair.second->getInfo() << endl;
            }
        }
    }

    bool isInBounds(int x, int y) const {
        if (x < 0 || y < 0) {
            cout << "err1" << endl;
            return false;
        }
        if (x > BOARD_WIDTH || y > BOARD_HEIGHT) {
            cout << "err" << endl;
            return false;
        }
        return true;
    }

    void addCircle(int x, int y, double r, string& color, string& fillMode) {
        if (!isOccupied(x, y, "circle", r)) {
            if (isInBounds(x, y) || isInBounds(x - r, y) || isInBounds(x + r, y) || isInBounds(x, y - r) || isInBounds(x, y + r)) {
                shapes[nextID] = std::make_unique<Circle>(nextID++, x, y, r, color, fillMode);
            }
            else {
                cout << "Error: Circle cannot be placed outside the board.\n";
            }
        }
        else {
            cout << "Error: Circle already has been placed.\n";
        }

    }

    void addSquare(int x, int y, double s, string color, string fillMode) {
        if (!isOccupied(x, y, "square", s)) {
            if (isInBounds(x, y) || isInBounds(x + s - 1, y) || isInBounds(x, y + s - 1)) {
                shapes[nextID] = std::make_unique<Square>(nextID++, x, y, s, color, fillMode);
            }
            else {
                cout << "Error: Square cannot be placed outside the board.\n";
            }
        }
        else {
            cout << "Error: Square already has been placed.\n";
        }

    }

    void addTriangle(int x, int y, double h, string color, string fillMode) {
        double b = 2 * h;
        if (!isOccupied(x, y, "triangle", h)) {
            int startX = static_cast<int>(x);
            int startY = static_cast<int>(y);
            if (isInBounds(x, y) || isInBounds(x - b / 2, y + h - 1) || isInBounds(x + b / 2, y + h - 1) || isInBounds(x, y + h)) {
                shapes[nextID] = std::make_unique<Triangle>(nextID++, x, y, h, color, fillMode);
            }
            else {
                cout << "Error: Triangle cannot be placed outside the board.\n";
            }
        }
        else {
            cout << "Error: Triangle already has been placed.\n";
        }

    }

    void addLine(int x1, int y1, int x2, int y2, string color, string fillMode) {
        // Перевіряємо, чи лінія може бути розміщена на цих координатах
        if (!isOccupied(x1, y1, "line", x2, y2)) {
            // Перевіряємо, чи координати початку і кінця лінії в межах дошки
            if (isInBounds(x1, y1) || isInBounds(x2, y2)) {
                // Якщо все добре, додаємо лінію на дошку
                shapes[nextID] = std::make_unique<Line>(nextID++, x1, y1, x2, y2, color, fillMode);
            }
            else {
                cout << "Error: Line cannot be placed outside the board.\n";
            }
        }
        else {
            cout << "Error: Line already exists at these coordinates.\n";
        }
    }

    void addRectangle(int x, int y, double width, double height, string color, string fillMode) {
        // Перевіряємо, чи місце для прямокутника вільне
        if (!isOccupied(x, y, "rectangle", width, height)) {
            // Перевіряємо, чи прямокутник не виходить за межі дошки
            if (isInBounds(x, y) || isInBounds(x + width - 1, y + height - 1)) {
                // Якщо всі умови виконані, додаємо новий прямокутник
                shapes[nextID] = std::make_unique<Rectangle>(nextID++, x, y, width, height, color, fillMode);
            }
            else {
                cout << "Error: Rectangle cannot be placed outside the board.\n";
            }
        }
        else {
            cout << "Error: Rectangle already exists at this location.\n";
        }
    }


    void undo() {
        if (!shapes.empty()) {
            auto lastElement = --shapes.end(); // Отримуємо ітератор на останній елемент
            shapes.erase(lastElement);
        }
        else {
            cout << "No shapes to undo.\n";
        }
    }

    void clear() {
        shapes.clear();
        cout << "Board cleared.\n";
    }
    void save(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file for writing.\n";
            return;
        }
        file << shapes.size() << endl;
        for (const auto& shapePair : shapes) {
            file << shapePair.second->getLoad() << endl; // Використовуємо shapePair.second для доступу до Shape
        }
        file.close();
        cout << "Blackboard saved to " << filename << ".\n";
    }

    void load(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file for reading.\n";
            return;
        }

        clear();
        int shapeCount;
        file >> shapeCount;
        for (int i = 0; i < shapeCount; ++i) {
            string shapeType;
            file >> shapeType;
            if (shapeType == "Circle:") {
                int id, x, y;
                double r;
                string color, fillMode;
                file >> id >> x >> y >> r >> color >> fillMode;
                addCircle(x, y, r, color, fillMode);
            }
            else if (shapeType == "Square:") {
                int id, x, y;
                double  s;
                string color, fillMode;
                file >> id >> x >> y >> s >> color >> fillMode ;
                addSquare(x, y, s, color, fillMode);
            }
            else if (shapeType == "Triangle:") {
                int id, x, y;
                double  h;
                string color, fillMode;
                file >> id >> x >> y >> h >> color >> fillMode;
                addTriangle(x, y, h, color, fillMode);
            }
            else if (shapeType == "Line:") {
                int id;
                int x1, y1, x2, y2;
                string color, fillMode;
                file >> id >> x1 >> y1 >> x2 >> y2 >> color >> fillMode;
                addLine(x1, y1, x2, y2, color, fillMode);
            }
            else if (shapeType == "Rectangle:") {
                int id, x, y;
                double  w, h;
                string color, fillMode;
                file >> id >> x >> y >> w >> h >> color >> fillMode;
                addRectangle(x, y, w, h, color, fillMode);
            }
        }
        file.close();
        cout << "Blackboard loaded from " << filename << ".\n";
    }
    
    void select(int id) {
        bool found = false;
        for (const auto& shapePair : shapes) {
            cout << shapePair.first << endl;
            if (shapePair.first==id) { // Assume containsPoint checks if a shape contains the point
                cout << shapePair.second->getInfo() << endl;
                lastSelectedId = shapePair.first;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Shape with ID " << id << " was not found.\n";
        }
    }

    void select(int x, int y) {
        bool found = false;
        for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) { 
            if (it->second->containsPoint(x, y)) {
                cout << it->second->getInfo() << endl; 
                lastSelectedId = it->first; 
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "No shape found at point (" << x << ", " << y << ").\n";
        }
    }

    int getLastSelectedId() const {
        return lastSelectedId; // Method to get the last selected shape ID
    }
    void remove() {
        if (lastSelectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }
        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            shapes.erase(it); 
            cout << "Shape with ID " << lastSelectedId << " removed.\n";
        }
        else {
            cout << "Shape with ID " << lastSelectedId << " not found.\n";
        }
    }
    void paint(const std::string& color) {
        if (lastSelectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }

        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            it->second->setColor(color); // Assuming a method to set color exists in your shape class
            std::cout << lastSelectedId << " " << it->second->getShape() << " " << color << std::endl; // Output new color info
        }
        else {
            std::cout << "Shape with ID " << lastSelectedId << " not found.\n";
        }
    }
    void move(int newX, int newY) {
        if (lastSelectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }

        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            Shapes* shape = it->second.get();

            if (newX < 0 || newX >= BOARD_WIDTH || newY < 0 || newY >= BOARD_HEIGHT) {
                std::cout << "Error: shape will go out of the board.\n";
                return;
            }

            shape->setX(newX); 
            shape->setY(newY); 

            lastSelectedId = lastSelectedId; 

            std::cout << lastSelectedId << " " << shape->getShape() << " moved.\n"; 
        }
        else {
            std::cout << "Shape with ID " << lastSelectedId << " not found.\n";
        }
    }
    void edit(int param1, int param2) {
        if (lastSelectedId == -1) {
            cout << "Error: No shape selected." << endl;
            return;
        }
        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            Shapes* shape = it->second.get(); // Get the selected shape
            if (auto* rectangle = dynamic_cast<Rectangle*>(shape)) {
                rectangle->setDimensions(param1, param2);
                cout << "Size of rectangle changed." << endl;
            }
        }
        else {
            cout << "Error: Shape with ID " << lastSelectedId << " not found." << endl;
        }
    }
    void edit(int param1) {
        // Check if a shape is selected
        if (lastSelectedId == -1) {
            cout << "Error: No shape selected." << endl;
            return;
        }

        // Find the selected shape by ID
        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            Shapes* shape = it->second.get(); // Get the selected shape

            if (auto* circle = dynamic_cast<Circle*>(shape)) {
                circle->setDimensions(param1);
                cout << "Radius of circle changed." << endl;
            }
            else if (auto* triangle = dynamic_cast<Triangle*>(shape)) {
                triangle->setDimensions(param1);
                
                cout << "Size of triangle changed." << endl;
            }
            else if (auto* square = dynamic_cast<Square*>(shape)) {
                square->setDimensions(param1);
                cout << "Size of square changed." << endl;
            }       
        }
        else {
            cout << "Error: Shape with ID " << lastSelectedId << " not found." << endl;
        }
    }
    void edit(int param1, int param2, int param3, int param4) {
        if (lastSelectedId == -1) {
            cout << "Error: No shape selected." << endl;
            return;
        }
        auto it = shapes.find(lastSelectedId);
        if (it != shapes.end()) {
            Shapes* shape = it->second.get(); // Get the selected shape
            if (auto* line = dynamic_cast<Line*>(shape)) {
                line->setDimensions(param1, param2, param3, param4);
                cout << "Size of rectangle changed." << endl;
            }
        }
        else {
            cout << "Error: Shape with ID " << lastSelectedId << " not found." << endl;
        }
    }
};

class CommandLine {
private:
    Board board;
    int lastSelectedId;  
    bool shapeSelected;
public:
    CommandLine() : lastSelectedId(-1), shapeSelected(false) {}

    void run() {
        string command;
        while (true) {
            
            cout << "> ";
            cin >> command;

            if (command == "draw") {
                board.draw(); 
                board.print(); 
            }
            else if (command == "list") {
                board.list();
            }
            else if (command == "shapes") {
                cout << "> Triangle coordinates base height" << endl;
                cout << "> Circle coordinates radius" << endl;
                cout << "> Square coordinates side" << endl;
                cout << "> Rectangle coordinates width height" << endl;
                cout << "> Line x1 y1 x2 y2" << endl;
            }
            else if (command == "add") {
                string shapeType, color, fillMode;
                cin >> fillMode >> color >> shapeType;

                if (shapeType == "circle") {
                    double r;
                    int x, y;
                    cin >> x >> y >> r;
                    board.addCircle(x, y, r, color, fillMode);
                }
                else if (shapeType == "square") {
                    double s;
                    int x, y;
                    cin >> x >> y >> s;
                    board.addSquare(x, y, s, color, fillMode);
                }
                else if (shapeType == "line") {
                    int x1, y1, x2, y2;
                    cin >> x1 >> y1 >> x2 >> y2;
                    board.addLine(x1, y1, x2, y2, color, fillMode);
                }
                else if (shapeType == "rectangle") {
                    double w, h;
                    int x, y;
                    cin >> x >> y >> w >> h;
                    board.addRectangle(x, y, w, h, color, fillMode);
                }
                else if (shapeType == "triangle") {
                    double h;
                    int x, y;
                    cin >> x >> y >> h;
                    board.addTriangle(x, y, h, color, fillMode);
                }
            }
            else if (command == "undo") {
                board.undo();
            }
            else if (command == "clear") {
                board.clear();
            }
            else if (command == "save") {
                string filepath;
                cin >> filepath;
                board.save(filepath);
            }
            else if (command == "load") {
                string filepath;
                cin >> filepath;
                board.load(filepath);
            }
            else if (command == "exit") {
          
                break;
            }
            else if (command == "select") {
                string selection;
                getline(cin, selection); 

                stringstream ss(selection);
                vector<string> params;
                string param;
                while (ss >> param) {
                    params.push_back(param);
                }

                if (params.size() == 1) { 
                    if (isdigit(params[0][0]) ) {
                        int id = stoi(params[0]); 
                        board.select(id); 
                        lastSelectedId = id;
                        shapeSelected = true; 
                    }
                    else {
                        cout << "Invalid ID.\n"; 
                    }
                }
                else if (params.size() == 2) { 
                    int x, y;
                    try {
                        x = stoi(params[0]); 
                        y = stoi(params[1]); 
                        board.select(x, y); 
                        if (shapeSelected) {
                            lastSelectedId = board.getLastSelectedId(); 
                        }
                    }
                    catch (const std::invalid_argument&) {
                        cout << "Invalid coordinates.\n";
                    }
                    catch (const std::out_of_range&) {
                        cout << "Coordinates out of range.\n"; 
                    }
                }
                else {
                    cout << "Invalid selection parameters.\n"; 
                }
            }
            else if (command == "edit") {
                string selection;
                getline(cin, selection); 

                stringstream ss(selection);
                vector<string> params;
                string param;
                while (ss >> param) {
                    params.push_back(param);
                }
                if (params.size() == 1) { 
                    double param1 = stod(params[0]);
                    board.edit(param1);
                }
                else if (params.size() == 2) { 
                    double param1 = stod(params[0]); 
                    double param2 = stod(params[1]);
                    board.edit(param1, param2);
                }
                else if (params.size() == 4) { 
                    double param1 = stod(params[0]); 
                    double param2 = stod(params[1]); 
                    double param3 = stod(params[2]); 
                    double param4 = stod(params[3]); 
                    board.edit(param1, param2, param3, param4);
                }
            }
            else if (command == "remove") {
                if (board.getLastSelectedId() == -1) {
                    cout << "No shape selected to remove.\n";
                }
                else {
                    board.remove();
                    lastSelectedId = -1;
                    shapeSelected = false;
                }
                
            }
            else if (command == "move") {
                int newX, newY;
                cin >> newX >> newY; 

                if (board.getLastSelectedId() == -1) {
                    cout << "No shape selected.\n"; 
                }
                else {
                    board.move(newX, newY); 
                }
            }
            else if (command == "paint") {
                string newColor;
                cin >> newColor;
                board.paint(newColor); 
            }
            else {
                cout << "Unknown command.\n";
            }
        }
    }
};

int main() {
   
    CommandLine cmd;
    cmd.run();
    return 0;
}