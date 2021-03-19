const int WDTH = 1400;
const int HGHT = 800;

bool equals(VertexArray& x, VertexArray& y) {
    if (x.getVertexCount() != y.getVertexCount())
        return false;
    for (int i = 0; i < (int) x.getVertexCount(); i++) {
        if (x[i].position.x != y[i].position.x) return false;
        if (x[i].position.y != y[i].position.y) return false;
    }
    return true;
}

class Circuit {
  private:
    vector<Piece> nodes;
    vector<tuple<int, int, int, int>> edges;
    vector<VertexArray> wires;
    vector<vector<int>> order;

    set<pair<int, int>> usedGridNodesForNodes;
    set<pair<int, int>> usedGridNodesForEdges;
    set<pair<pair<int, int>, pair<int, int>>> usedGridEdges;

    float zoom;

    static deque<Circuit> UndoStack;
    static stack<Circuit> RedoStack;

    bool findPath(int edge) {
        static const int xAdd[] = {0, 20, 0, -20};
        static const int yAdd[] = {-20, 0, 20, 0};

        auto [node1, end1, node2, end2] = edges[edge];
        int x1, y1; tie(x1, y1) = nodes[node1].getEnd(end1);
        int x2, y2; tie(x2, y2) = nodes[node2].getEnd(end2);

        unordered_map<int, unordered_map<int, int>> dp;
        dp[x1][y1] = 1;
        queue<pair<int, int>> q;
        q.emplace(x1, y1);
        while (!q.empty()) {
            auto [xNow, yNow] = q.front();
            q.pop();
            for (int i = 0; i < 4; i++) {
                int xNgh = xNow + xAdd[order[edge][i]];
                int yNgh = yNow + yAdd[order[edge][i]];
                auto now = make_pair(xNow, yNow);
                auto ngh = make_pair(xNgh, yNgh);
                if (-WDTH <= xNgh && xNgh <= 2 * WDTH &&
                    -HGHT <= yNgh && yNgh <= 2 * HGHT &&
                    !dp[xNgh][yNgh] &&
                    !usedGridNodesForEdges.count(ngh) &&
                    !usedGridEdges.count(make_pair(min(now, ngh), max(now, ngh)))) {
                        dp[xNgh][yNgh] = dp[xNow][yNow] + 1;
                        q.emplace(xNgh, yNgh);
                    }
            }
            if (dp[x2][y2])
                break;
        }
        if (!dp[x2][y2])
            return false;

        wires[edge] = VertexArray(LineStrip);
        function<void(int, int)> back = [&](int xNow, int yNow) {
            if (xNow == x1 && yNow == y1) {
                wires[edge].append(Vector2f(xNow, yNow));
                return;
            }
            for (int i = 0; i < 4; i++) {
                int xNgh = xNow + xAdd[order[edge][i]];
                int yNgh = yNow + yAdd[order[edge][i]];
                auto now = make_pair(xNow, yNow);
                auto ngh = make_pair(xNgh, yNgh);
                if (dp[xNgh][yNgh] == dp[xNow][yNow] - 1 &&
                    !usedGridEdges.count(make_pair(min(now, ngh), max(now, ngh)))) {
                        back(xNgh, yNgh);
                        wires[edge].append(Vector2f(xNow, yNow));
                        return;
                    }
            }
        };
        back(x2, y2);
        return true;
    }

    bool updateEdge(int edge) {
        for (int i = 1; i < (int) wires[edge].getVertexCount(); i++) {
            auto one = make_pair(wires[edge][i - 1].position.x, wires[edge][i - 1].position.y);
            auto two = make_pair(wires[edge][i - 0].position.x, wires[edge][i - 0].position.y);
            usedGridEdges.erase(make_pair(min(one, two), max(one, two)));
        }
        auto backup = wires[edge];
        if (findPath(edge)) {
            for (int i = 1; i < (int) wires[edge].getVertexCount(); i++) {
                auto one = make_pair(wires[edge][i - 1].position.x, wires[edge][i - 1].position.y);
                auto two = make_pair(wires[edge][i - 0].position.x, wires[edge][i - 0].position.y);
                usedGridEdges.emplace(min(one, two), max(one, two));
            }
            return true;
        }
        wires[edge] = backup;
        for (int i = 1; i < (int) wires[edge].getVertexCount(); i++) {
            auto one = make_pair(wires[edge][i - 1].position.x, wires[edge][i - 1].position.y);
            auto two = make_pair(wires[edge][i - 0].position.x, wires[edge][i - 0].position.y);
            usedGridEdges.emplace(min(one, two), max(one, two));
        }
        return false;
    }

    bool updateEdges(int node) {
        auto backup = wires;
        for (int i = 0; i < (int) edges.size(); i++)
            if ((get<0>(edges[i]) == node || get<2>(edges[i]) == node) && !updateEdge(i)) {
                wires = backup;
                return false;
            }
        for (int i = 0; i < (int) edges.size(); i++) {
            bool intersects = false;
            for (int j = 0; j < (int) wires[i].getVertexCount(); j++)
                if (nodes[node].getMinXForEdges() <= wires[i][j].position.x && wires[i][j].position.x <= nodes[node].getMaxXForEdges() &&
                    nodes[node].getMinYForEdges() <= wires[i][j].position.y && wires[i][j].position.y <= nodes[node].getMaxYForEdges()) {
                        intersects = true;
                        break;
                    }
            if (intersects && !updateEdge(i)) {
                wires = backup;
                return false;
            }
        }
        return true;
    }

    void savePoint() {
        UndoStack.push_back(*this);
        if (UndoStack.size() > 1000)
            UndoStack.pop_front();
        while (!RedoStack.empty())
            RedoStack.pop();
    }

    bool checkFreeEnd(int node, int end) {
        for (auto [node1, end1, node2, end2] : edges)
            if ((node1 == node && end1 == end) ||
                (node2 == node && end2 == end))
                    return false;
        return true;
    }

  public:
    float xOffset;
    float yOffset;

    int activeNode;
    pair<int, int> activeEnd;

    Circuit() :
        zoom(1), xOffset(0), yOffset(0),
        activeNode(-1), activeEnd(-1, -1) {
            UndoStack.clear();
            UndoStack.push_back(*this);
        }

    void saveInFile(const string& file) {
        ofstream fout(file);
        fout << nodes.size() << '\n';
        for (auto& node : nodes)
            node.saveInFile(fout);
        fout << edges.size() << '\n';
        for (auto [node1, end1, node2, end2] : edges)
            fout << node1 << ' ' << end1 << ' ' << node2 << ' ' << end2 << '\n';
        for (auto& wire : wires) {
            fout << wire.getVertexCount() << ' ';
            for (int i = 0; i < (int) wire.getVertexCount(); i++)
                fout << wire[i].position.x << ' ' << wire[i].position.y << ' ';
            fout << '\n';
        }
        for (auto& ord : order) {
            for (int i = 0; i < 4; i++)
                fout << ord[i] << ' ';
            fout << '\n';
        }
        fout << usedGridNodesForNodes.size() << '\n';
        for (auto [x, y] : usedGridNodesForNodes)
            fout << x << ' ' << y << '\n';
        fout << usedGridNodesForEdges.size() << '\n';
        for (auto [x, y] : usedGridNodesForEdges)
            fout << x << ' ' << y << '\n';
        fout << usedGridEdges.size() << '\n';
        for (auto [one, two] : usedGridEdges)
            fout << one.first << ' ' << one.second << ' ' << two.first << ' ' << two.second << '\n';
        fout.close();
    }

    void loadFromFile(const string& file) {
        ifstream fin(file);
        int nodesSize; fin >> nodesSize;
        nodes.resize(nodesSize);
        for (auto& node : nodes)
            node.loadFromFile(fin);
        int edgesSize; fin >> edgesSize;
        edges.resize(edgesSize);
        for (auto& [node1, end1, node2, end2] : edges)
            fin >> node1 >> end1 >> node2 >> end2;
        wires.resize(edgesSize);
        for (auto& wire : wires) {
            int wireSize; fin >> wireSize;
            wire = VertexArray(LineStrip);
            for (int i = 0; i < wireSize; i++) {
                int x, y; fin >> x >> y;
                wire.append(Vector2f(x, y));
            }
        }
        order.resize(edgesSize);
        for (auto& ord : order) {
            ord.resize(4);
            for (int& val : ord)
                fin >> val;
        }
        int usedGridNodesForNodesSize; fin >> usedGridNodesForNodesSize;
        usedGridNodesForNodes.clear();
        for (int i = 0; i < usedGridNodesForNodesSize; i++) {
            int x, y; fin >> x >> y;
            usedGridNodesForNodes.emplace(x, y);
        }
        int usedGridNodesForEdgesSize; fin >> usedGridNodesForEdgesSize;
        usedGridNodesForEdges.clear();
        for (int i = 0; i < usedGridNodesForEdgesSize; i++) {
            int x, y; fin >> x >> y;
            usedGridNodesForEdges.emplace(x, y);
        }
        int usedGridEdgesSize; fin >> usedGridEdgesSize;
        usedGridEdges.clear();
        for (int i = 0; i < usedGridEdgesSize; i++) {
            int x1, y1, x2, y2; fin >> x1 >> y1 >> x2 >> y2;
            usedGridEdges.emplace(make_pair(x1, y1), make_pair(x2, y2));
        }
        fin.close();
        UndoStack.clear();
        UndoStack.push_back(*this);
        while (!RedoStack.empty())
            RedoStack.pop();
    }

    bool addNode(const string& type, int x, int y) {
        Piece piece(type, x, y);
        for (int x = piece.getMinXForNodes(); x <= piece.getMaxXForNodes(); x += 20)
            for (int y = piece.getMinYForNodes(); y <= piece.getMaxYForNodes(); y += 20)
                if (usedGridNodesForNodes.count(make_pair(x, y)))
                    return false;
        nodes.push_back(piece);
        for (int x = piece.getMinXForNodes(); x <= piece.getMaxXForNodes(); x += 20)
            for (int y = piece.getMinYForNodes(); y <= piece.getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.emplace(x, y);
        for (int x = piece.getMinXForEdges(); x <= piece.getMaxXForEdges(); x += 20)
            for (int y = piece.getMinYForEdges(); y <= piece.getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.emplace(x, y);
        if (!updateEdges(nodes.size() - 1)) {
            *this = UndoStack.back();
            return false;
        }
        savePoint();
        return true;
    }

    bool rotateNode(int node) {
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.erase(make_pair(x, y));
        for (int x = nodes[node].getMinXForEdges(); x <= nodes[node].getMaxXForEdges(); x += 20)
            for (int y = nodes[node].getMinYForEdges(); y <= nodes[node].getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.erase(make_pair(x, y));
        nodes[node].rotate();
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                if (usedGridNodesForNodes.count(make_pair(x, y))) {
                    *this = UndoStack.back();
                    return false;
                }
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.emplace(x, y);
        for (int x = nodes[node].getMinXForEdges(); x <= nodes[node].getMaxXForEdges(); x += 20)
            for (int y = nodes[node].getMinYForEdges(); y <= nodes[node].getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.emplace(x, y);
        if (!updateEdges(node)) {
            *this = UndoStack.back();
            return false;
        }
        savePoint();
        return true;
    }

    bool moveNode(int node, int xAdd, int yAdd) {
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.erase(make_pair(x, y));
        for (int x = nodes[node].getMinXForEdges(); x <= nodes[node].getMaxXForEdges(); x += 20)
            for (int y = nodes[node].getMinYForEdges(); y <= nodes[node].getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.erase(make_pair(x, y));
        nodes[node].x += xAdd;
        nodes[node].y += yAdd;
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                if (usedGridNodesForNodes.count(make_pair(x, y))) {
                    *this = UndoStack.back();
                    return false;
                }
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.emplace(x, y);
        for (int x = nodes[node].getMinXForEdges(); x <= nodes[node].getMaxXForEdges(); x += 20)
            for (int y = nodes[node].getMinYForEdges(); y <= nodes[node].getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.emplace(x, y);
        if (!updateEdges(node)) {
            *this = UndoStack.back();
            return false;
        }
        savePoint();
        return true;
    }

    void eraseNode(int node) {
        for (int x = nodes[node].getMinXForNodes(); x <= nodes[node].getMaxXForNodes(); x += 20)
            for (int y = nodes[node].getMinYForNodes(); y <= nodes[node].getMaxYForNodes(); y += 20)
                usedGridNodesForNodes.erase(make_pair(x, y));
        for (int x = nodes[node].getMinXForEdges(); x <= nodes[node].getMaxXForEdges(); x += 20)
            for (int y = nodes[node].getMinYForEdges(); y <= nodes[node].getMaxYForEdges(); y += 20)
                usedGridNodesForEdges.erase(make_pair(x, y));
        nodes.erase(nodes.begin() + node);
        for (int i = (int) edges.size() - 1; i >= 0; i--)
            if (get<0>(edges[i]) == node || get<2>(edges[i]) == node) {
                edges.erase(edges.begin() + i);
                wires.erase(wires.begin() + i);
                order.erase(order.begin() + i);
            }
        for (auto& [node1, end1, node2, end2] : edges) {
            if (node1 > node) node1--;
            if (node2 > node) node2--;
        }
        savePoint();
    }

    bool addEdge(int node1, int end1, int node2, int end2) {
        edges.emplace_back(node1, end1, node2, end2);
        wires.emplace_back();
        order.push_back({0, 1, 2, 3});
        if (!updateEdge(edges.size() - 1)) {
            edges.pop_back();
            wires.pop_back();
            order.pop_back();
            return false;
        }
        savePoint();
        return true;
    }

    void changeEdge(int edge) {
        auto init = wires[edge];
        for (int i = 0; i < 24; i++) {
            next_permutation(order[edge].begin(), order[edge].end());
            updateEdge(edge);
            if (!equals(wires[edge], init))
                break;
        }
        savePoint();
    }

    void eraseEdge(int edge) {
        edges.erase(edges.begin() + edge);
        wires.erase(wires.begin() + edge);
        order.erase(order.begin() + edge);
        savePoint();
    }

    void draw(RenderWindow& window, int xMouse, int yMouse) {
        for (float x = -WDTH; x <= 2 * WDTH; x += 20) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f(xOffset + zoom * x, yOffset + zoom * -HGHT);
            line[1].position = Vector2f(xOffset + zoom * x, yOffset + zoom * 2 * HGHT);
            line[0].color = GRID_COLOR;
            line[1].color = GRID_COLOR;
            window.draw(line);
        }
        for (float y = -HGHT; y <= 2 * HGHT; y += 20) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f(xOffset + zoom * -WDTH, yOffset + zoom * y);
            line[1].position = Vector2f(xOffset + zoom * 2 * WDTH, yOffset + zoom * y);
            line[0].color = GRID_COLOR;
            line[1].color = GRID_COLOR;
            window.draw(line);
        }

        for (auto wire : wires) {
            for (int i = 0; i < (int) wire.getVertexCount(); i++) {
                wire[i].position.x = xOffset + zoom * wire[i].position.x; wire[i].color = WIRE_COLOR;
                wire[i].position.y = yOffset + zoom * wire[i].position.y; wire[i].color = WIRE_COLOR;
            }
            window.draw(wire);
        }
        for (int i = 0; i < (int) nodes.size(); i++)
            nodes[i].draw(window, zoom, xOffset, yOffset);
        if (activeEnd.first != -1) {
            VertexArray line(Lines, 2);
            line[0] = Vector2f(
                xOffset + zoom * nodes[activeEnd.first].getEnd(activeEnd.second).first,
                yOffset + zoom * nodes[activeEnd.first].getEnd(activeEnd.second).second
            );
            line[1] = Vector2f(xMouse, yMouse);
            line[0].color = line[1].color = WIRE_COLOR;
            window.draw(line);
        }

        map<pair<int, int>, int> frq;
        for (int i = 0; i < (int) wires.size(); i++) {
            frq[make_pair(wires[i][0].position.x, wires[i][0].position.y)]++;
            frq[make_pair(wires[i][(int) wires[i].getVertexCount() - 1].position.x, wires[i][(int) wires[i].getVertexCount() - 1].position.y)]++;
            for (int j = 1; j < (int) wires[i].getVertexCount() - 1; j++)
                if ((wires[i][j - 1].position.x == wires[i][j].position.x && wires[i][j].position.x == wires[i][j + 1].position.x) ||
                    (wires[i][j - 1].position.y == wires[i][j].position.y && wires[i][j].position.y == wires[i][j + 1].position.y))
                        frq[make_pair(wires[i][j].position.x, wires[i][j].position.y)]++;
        }
        for (auto [point, cnt] : frq)
            if (cnt == 2) {
                auto [x, y] = point;
                VertexArray lineH(Lines, 2);
                lineH[0] = Vector2f(xOffset + zoom * (x - 5), yOffset + zoom * y); lineH[0].color = GRID_COLOR;
                lineH[1] = Vector2f(xOffset + zoom * (x + 5), yOffset + zoom * y); lineH[1].color = GRID_COLOR;
                window.draw(lineH);

                VertexArray lineV(Lines, 2);
                lineV[0] = Vector2f(xOffset + zoom * x, yOffset + zoom * (y - 5)); lineV[0].color = WIRE_COLOR;
                lineV[1] = Vector2f(xOffset + zoom * x, yOffset + zoom * (y + 5)); lineV[1].color = WIRE_COLOR;
                window.draw(lineV);

                VertexArray inter(LineStrip);
                inter.append(Vector2f(xOffset + zoom * (x - 5), yOffset + zoom * (y - 0))); inter[0].color = WIRE_COLOR;
                inter.append(Vector2f(xOffset + zoom * (x - 5), yOffset + zoom * (y - 2))); inter[1].color = WIRE_COLOR;
                inter.append(Vector2f(xOffset + zoom * (x - 2), yOffset + zoom * (y - 5))); inter[2].color = WIRE_COLOR;
                inter.append(Vector2f(xOffset + zoom * (x + 2), yOffset + zoom * (y - 5))); inter[3].color = WIRE_COLOR;
                inter.append(Vector2f(xOffset + zoom * (x + 5), yOffset + zoom * (y - 2))); inter[4].color = WIRE_COLOR;
                inter.append(Vector2f(xOffset + zoom * (x + 5), yOffset + zoom * (y - 0))); inter[5].color = WIRE_COLOR;
                window.draw(inter);
            }
    }

    int canvasToCircuitX(int x) { return (x - xOffset) / zoom; }
    int canvasToCircuitY(int y) { return (y - yOffset) / zoom; }

    int hoveredNode(int x, int y) {
        x = canvasToCircuitX(x);
        y = canvasToCircuitY(y);
        for (int i = 0; i < (int) nodes.size(); i++)
            if (nodes[i].getMinXForEdges() - 5 <= x && x <= nodes[i].getMaxXForEdges() + 5 &&
                nodes[i].getMinYForEdges() - 5 <= y && y <= nodes[i].getMaxYForEdges() + 5)
                    return i;
        return -1;
    }

    pair<int, int> hoveredEnd(int x, int y) {
        x = canvasToCircuitX(x);
        y = canvasToCircuitY(y);
        for (int i = 0; i < (int) nodes.size(); i++)
            for (int j = 0; j < nodes[i].endCount(); j++)
                if (!(i == activeEnd.first && j == activeEnd.second)) {
                    auto [xEnd, yEnd] = nodes[i].getEnd(j);
                    if (xEnd - 10 <= x && x <= xEnd + 10 &&
                        yEnd - 10 <= y && y <= yEnd + 10)
                            return checkFreeEnd(i, j) ? make_pair(i, j) : make_pair(-1, -1);
                }
        return make_pair(-1, -1);
    }

    int hoveredEdge(int x, int y) {
        x = canvasToCircuitX(x);
        y = canvasToCircuitY(y);
        for (int i = 0; i < (int) nodes.size(); i++)
            for (int j = 0; j < nodes[i].endCount(); j++)
                if (!(i == activeEnd.first && j == activeEnd.second)) {
                    auto [xEnd, yEnd] = nodes[i].getEnd(j);
                    if (xEnd - 10 <= x && x <= xEnd + 10 &&
                        yEnd - 10 <= y && y <= yEnd + 10) {
                            if (checkFreeEnd(i, j))
                                return -1;
                            for (int k = 0; k < (int) edges.size(); k++)
                                if ((i == get<0>(edges[k]) && j == get<1>(edges[k])) ||
                                    (i == get<2>(edges[k]) && j == get<3>(edges[k])))
                                    return k;
                        }
                }
        return -1;
    }

    void undo() {
        if (UndoStack.size() > 1) {
            float backupZoom = zoom;
            int backupXOffset = xOffset;
            int backupYOffset = yOffset;
            RedoStack.push(UndoStack.back());
            UndoStack.pop_back();
            *this = UndoStack.back();
            activeNode = -1;
            activeEnd = make_pair(-1, -1);
            zoom = backupZoom;
            xOffset = backupXOffset;
            yOffset = backupYOffset;
        }
    }

    void redo() {
        if (!RedoStack.empty()) {
            float backupZoom = zoom;
            int backupXOffset = xOffset;
            int backupYOffset = yOffset;
            *this = RedoStack.top();
            RedoStack.pop();
            UndoStack.push_back(*this);
            activeNode = -1;
            activeEnd = make_pair(-1, -1);
            zoom = backupZoom;
            xOffset = backupXOffset;
            yOffset = backupYOffset;
        }
    }

    float getZoom() {
        return zoom;
    }

    void incZoom(float lvl) {
        float newXOffset = WDTH / 2 - (WDTH / 2 - xOffset) / zoom * (zoom + lvl);
        float newYOffset = HGHT / 2 - (HGHT / 2 - yOffset) / zoom * (zoom + lvl);
        if (newXOffset + 2 * WDTH * (zoom + lvl) < WDTH) return;
        if (newXOffset - 1 * WDTH * (zoom + lvl) >    0) return;
        if (newYOffset + 2 * HGHT * (zoom + lvl) < HGHT) return;
        if (newYOffset - 1 * HGHT * (zoom + lvl) >    0) return;
        xOffset = newXOffset;
        yOffset = newYOffset;
        zoom += lvl;
    }

    int nodeCount() { return nodes.size(); }
    int edgeCount() { return edges.size(); }

    int getNodeX(int node) { return nodes[node].x; }
    int getNodeY(int node) { return nodes[node].y; }

    string getNodeInfo(int node) {
        return nodes[node].info;
    }

    void setNodeInfo(int node, const string& info) {
        if (info != nodes[node].info) {
            savePoint();
            nodes[node].info = info;
        }
    }
};
deque<Circuit> Circuit::UndoStack;
stack<Circuit> Circuit::RedoStack;
