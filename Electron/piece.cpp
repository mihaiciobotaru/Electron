const Color WIRE_COLOR = Color::White;
const Color BKGD_COLOR = Color::Black;
const Color GRID_COLOR = Color(50, 50, 50);

class Piece {
  private:
    int xMinForNodes, xMaxForNodes, yMinForNodes, yMaxForNodes;
    int xMinForEdges, xMaxForEdges, yMinForEdges, yMaxForEdges;
    vector<pair<string, vector<int>>> shapes;
    vector<pair<int, int>> ends;
    static map<string, Piece> GenericPieces;

  public:
    int x, y;
    string info;

    Piece() { }

    Piece(const string& type, int x, int y) {
        *this = GenericPieces[type];
        this->x = x;
        this->y = y;
        info = "info";
    }

    Piece(const string& file) {
        ifstream fin(file);
        int n; fin >> n;
        shapes.resize(n);
        for (auto& [type, args] : shapes) {
            fin >> type;
            if (type == "line")
                args.resize(4);
            else if (type == "circle")
                args.resize(3);
            else if (type == "triangle")
                args.resize(6);
            else if (type == "square")
                args.resize(3);
            for (int& arg : args)
                fin >> arg;
            char chr; fin.get(chr);
            if (chr == ' ') {
                args.emplace_back();
                string dummy; fin >> dummy;
            }
        }
        int m; fin >> m;
        ends.resize(m);
        for (auto& [x, y] : ends)
            fin >> x >> y;
        fin >> xMinForEdges >> xMaxForEdges;
        fin >> yMinForEdges >> yMaxForEdges;
        fin.close();
        xMinForNodes = xMinForEdges; xMaxForNodes = xMaxForEdges;
        yMinForNodes = yMinForEdges; yMaxForNodes = yMaxForEdges;
        for (auto& [x, y] : ends) {
            xMinForNodes = min(xMinForNodes, x); xMaxForNodes = max(xMaxForNodes, x);
            yMinForNodes = min(yMinForNodes, y); yMaxForNodes = max(yMaxForNodes, y);
        }
    }

    void saveInFile(ofstream& fout) {
        fout << x << ' ' << y << ' ' << (info == "" ? "info" : info) << '\n';
        fout << xMinForNodes << ' ' << xMaxForNodes << ' ' << yMinForNodes << ' ' << yMaxForNodes << '\n';
        fout << xMinForEdges << ' ' << xMaxForEdges << ' ' << yMinForEdges << ' ' << yMaxForEdges << '\n';
        fout << shapes.size() << '\n';
        for (auto& [type, args] : shapes) {
            fout << type << ' ' << args.size() << ' ';
            for (int arg : args)
                fout << arg << ' ';
            fout << '\n';
        }
        fout << ends.size() << '\n';
        for (auto [x, y] : ends)
            fout << x << ' ' << y << '\n';
    }

    void loadFromFile(ifstream& fin) {
        fin >> x >> y >> info;
        fin >> xMinForNodes >> xMaxForNodes >> yMinForNodes >> yMaxForNodes;
        fin >> xMinForEdges >> xMaxForEdges >> yMinForEdges >> yMaxForEdges;
        int shapesSize; fin >> shapesSize;
        shapes.resize(shapesSize);
        for (auto& [type, args] : shapes) {
            fin >> type;
            int argsSize; fin >> argsSize;
            args.resize(argsSize);
            for (int& arg : args)
                fin >> arg;
        }
        int endsSize; fin >> endsSize;
        ends.resize(endsSize);
        for (auto& [x, y] : ends)
            fin >> x >> y;
        info = "info";
    }

    void draw(RenderWindow& window, float zoom, int xOffset, int yOffset) {
        for (auto [type, args] : shapes)
            if (type == "line") {
                VertexArray line(Lines, 2);
                line[0] = Vector2f(xOffset + zoom * (x + args[0]), yOffset + zoom * (y + args[1])); line[0].color = WIRE_COLOR;
                line[1] = Vector2f(xOffset + zoom * (x + args[2]), yOffset + zoom * (y + args[3])); line[1].color = WIRE_COLOR;
                window.draw(line);
            }
            else if (type == "circle") {
                CircleShape circle(zoom * args[2]);
                circle.setOrigin(zoom * args[2], zoom * args[2]);
                circle.setPosition(xOffset + zoom * (x + args[0]), yOffset + zoom * (y + args[1]));
                circle.setFillColor(BKGD_COLOR);
                circle.setOutlineColor(WIRE_COLOR);
                circle.setOutlineThickness(1);
                window.draw(circle);
            }
            else if (type == "triangle") {
                ConvexShape triangle(3);
                triangle.setPoint(0, Vector2f(xOffset + zoom * (x + args[0]), yOffset + zoom * (y + args[1])));
                triangle.setPoint(1, Vector2f(xOffset + zoom * (x + args[2]), yOffset + zoom * (y + args[3])));
                triangle.setPoint(2, Vector2f(xOffset + zoom * (x + args[4]), yOffset + zoom * (y + args[5])));
                triangle.setFillColor(WIRE_COLOR);
                triangle.setOutlineColor(WIRE_COLOR);
                window.draw(triangle);
            }
            else if (type == "square") {
                ConvexShape square(4);
                square.setPoint(0, Vector2f(xOffset + zoom * (x + args[0] - args[2]), yOffset + zoom * (y + args[1] - args[2])));
                square.setPoint(1, Vector2f(xOffset + zoom * (x + args[0] - args[2]), yOffset + zoom * (y + args[1] + args[2])));
                square.setPoint(2, Vector2f(xOffset + zoom * (x + args[0] + args[2]), yOffset + zoom * (y + args[1] + args[2])));
                square.setPoint(3, Vector2f(xOffset + zoom * (x + args[0] + args[2]), yOffset + zoom * (y + args[1] - args[2])));
                square.setFillColor(WIRE_COLOR);
                square.setOutlineColor(WIRE_COLOR);
                window.draw(square);
            }
    }

    void rotate() {
        auto rotatePoint = [](int& x, int& y) {
            int xNext = +y;
            int yNext = -x;
            x = xNext;
            y = yNext;
        };
        for (auto& [type, args] : shapes)
            if (type == "line" && args.size() == 4) {
                rotatePoint(args[0], args[1]);
                rotatePoint(args[2], args[3]);
            }
            else if (type == "circle" && args.size() == 3)
                rotatePoint(args[0], args[1]);
            else if (type == "triangle" && args.size() == 6) {
                rotatePoint(args[0], args[1]);
                rotatePoint(args[2], args[3]);
                rotatePoint(args[4], args[5]);
            }
            else if (type == "square" && args.size() == 3)
                rotatePoint(args[0], args[1]);
        for (auto& [x, y] : ends)
            rotatePoint(x, y);

        rotatePoint(xMinForNodes, yMinForNodes);
        rotatePoint(xMaxForNodes, yMaxForNodes);
        int xMinNewForNodes = min(xMinForNodes, xMaxForNodes), xMaxNewForNodes = max(xMinForNodes, xMaxForNodes);
        int yMinNewForNodes = min(yMinForNodes, yMaxForNodes), yMaxNewForNodes = max(yMinForNodes, yMaxForNodes);
        xMinForNodes = xMinNewForNodes; xMaxForNodes = xMaxNewForNodes;
        yMinForNodes = yMinNewForNodes; yMaxForNodes = yMaxNewForNodes;

        rotatePoint(xMinForEdges, yMinForEdges);
        rotatePoint(xMaxForEdges, yMaxForEdges);
        int xMinNewForEdges = min(xMinForEdges, xMaxForEdges), xMaxNewForEdges = max(xMinForEdges, xMaxForEdges);
        int yMinNewForEdges = min(yMinForEdges, yMaxForEdges), yMaxNewForEdges = max(yMinForEdges, yMaxForEdges);
        xMinForEdges = xMinNewForEdges; xMaxForEdges = xMaxNewForEdges;
        yMinForEdges = yMinNewForEdges; yMaxForEdges = yMaxNewForEdges;
    }

    inline int getMinXForNodes() { return x + xMinForNodes; }
    inline int getMaxXForNodes() { return x + xMaxForNodes; }
    inline int getMinYForNodes() { return y + yMinForNodes; }
    inline int getMaxYForNodes() { return y + yMaxForNodes; }

    inline int getMinXForEdges() { return x + xMinForEdges; }
    inline int getMaxXForEdges() { return x + xMaxForEdges; }
    inline int getMinYForEdges() { return y + yMinForEdges; }
    inline int getMaxYForEdges() { return y + yMaxForEdges; }

    int endCount() {
        return ends.size();
    }

    pair<int, int> getEnd(int end) {
        return make_pair(ends[end].first + x, ends[end].second + y);
    }

    static void LoadPieceTypes() {
        auto dir = filesystem::directory_iterator(PREFIX + "Pieces");
        for (auto& file : dir) {
            string path = file.path().generic_string();
            string name = file.path().filename().generic_string();
            if (name.size() < 6 || name.substr(name.size() - 6, 6) != ".piece")
                continue;
            name = name.substr(0, name.size() - 6);
            GenericPieces.emplace(name, Piece(path));
        }
    }
};
map<string, Piece> Piece::GenericPieces;
