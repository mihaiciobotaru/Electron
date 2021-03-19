struct FileManager {
    int numberOfPages;
    int currentPage;
    vector<string> fileNames;
    vector<string> filePaths;
    string selectedFilePath;
    string selectedFileName;

    void readInfo() {
        fileNames.clear();
        filePaths.clear();
        auto dir = filesystem::directory_iterator(PREFIX + "Saves");
        for (auto& file : dir) {
            string path = file.path().generic_string();
            string name = file.path().filename().generic_string();
            if (name.size() < 8 || name.substr(name.size() - 8, 8) != ".circuit")
                continue;
            name = name.substr(0, name.size() - 8);
            filePaths.push_back(path);
            fileNames.push_back(name);
        }
        numberOfPages = fileNames.size() / 6;
        if (fileNames.size() % 6 == 0)
            numberOfPages--;
        if (fileNames.empty())
            numberOfPages = 0;
        sort(fileNames.begin(), fileNames.end());
        sort(filePaths.begin(), filePaths.end());
    }

    bool newFile(string filePath, string fileName) {
        int tempNum;
        string tempPath, tempName;
        auto itA = filePaths.begin();
        auto itB = fileNames.begin();
        while (itA != filePaths.end() || itB != fileNames.end()) {
            if (tempPath == filePath || tempName == fileName)
                return false;
            ++itA;
            ++itB;
        }
        filePaths.push_back(filePath);
        fileNames.push_back(fileName);

        numberOfPages = filePaths.size() / 6;
        if (filePaths.size() % 6 == 0)
            numberOfPages--;

        ofstream fout(filePath);
        fout << "0\n";
        fout << "0\n";
        fout << "0\n";
        fout << "0\n";
        fout << "0\n";
        fout.close();
        return true;
    }

    void removeFile(string filePath, string fileName) {
        int tempNum;
        string tempPath, tempName;
        for (auto it = filePaths.begin(); it != filePaths.end() + 1 && *(it - 1) != filePath; ++it)
            if (*it == filePath)
                filePaths.erase(it);
        for (auto it = fileNames.begin(); it != fileNames.end() + 1 && *(it - 1) != fileName; ++it)
            if (*it == fileName)
                fileNames.erase(it);

        numberOfPages = filePaths.size() / 6;
        if (filePaths.size() % 6 == 0)
            numberOfPages--;
        if (fileNames.size() == 0)
            numberOfPages = 0;

        char temp[100];
        for (int i = 0; i < filePath.length(); i++)
            temp[i] = filePath[i];
        temp[filePath.length()] = '\0';

        remove(temp);
        if (currentPage > numberOfPages)
            currentPage--;
    }

    void displayPageNum() {
        Position pos = {static_cast<float>(settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth * 0.45), static_cast<float>(settingsMenuSecondary.startPos.y + settingsMenuSecondary.buttonHeight * 0.25)};
        char temp[50], temp2[50];
        sprintf(temp, "%d", numberOfPages + 1);
        sprintf(temp2, "%d", currentPage + 1);
        printText(pos, currentLanguage[24] + temp2 + "/" + temp, 24, 1, currentLanguageOffset[24]);
    }

    Position pageButtonTemplate(bool isPrev, bool isActive, int color) {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setFillColor(Color(color));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight, settingsMenuSecondary.buttonHeight));

        CircleShape triangle(settingsMenuSecondary.buttonHeight * 0.4, 3);
        triangle.setOutlineThickness(-1.5f);
        triangle.setOutlineColor(Color::Black);
        triangle.setFillColor(Color(0xeeeeeeff));

        if (isPrev) {
            rectangle.setPosition(settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y);
            triangle.setPosition(settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonHeight * 0.17, settingsMenuSecondary.startPos.y + settingsMenuSecondary.buttonHeight * 0.9);
            triangle.rotate(-90);
        }
        else {
            rectangle.setPosition(settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth - settingsMenuSecondary.buttonHeight, settingsMenuSecondary.startPos.y);
            triangle.setPosition(settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth - settingsMenuSecondary.buttonHeight + settingsMenuSecondary.buttonHeight * 0.81, settingsMenuSecondary.startPos.y + settingsMenuSecondary.buttonHeight * 0.11);
            triangle.rotate(90);
        }

        if (!isActive) {
            rectangle.setFillColor(Color(0x33333388));
            triangle.setFillColor(Color(0x888888ff));
        }

        window->draw(rectangle);
        window->draw(triangle);
        return {rectangle.getPosition().x, rectangle.getPosition().y};
    }

    void displayPageBox() {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y);
        rectangle.setFillColor(Color(menuColor));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonWidth, settingsMenuSecondary.buttonHeight));
        window->draw(rectangle);
    }

    void displayControlsBox() {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + 7 * (settingsMenuSecondary.buttonHeight - 2));
        rectangle.setFillColor(Color(menuColor));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonWidth, settingsMenuSecondary.buttonHeight));
        window->draw(rectangle);
    }
} fileManager;

struct TextType {
    Position startPos;
    bool isActive;
    string text;
    int width;
    int height;

    void init() {
        isActive = false;
        text = "";
        startPos = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + 7 * (settingsMenuSecondary.buttonHeight - 2)};
        height = settingsMenuSecondary.buttonHeight;
        width = settingsMenuSecondary.buttonWidth - 3 * height;
        if (loadButton.clicked)
            width += height;
    }

    void displayText() {
        if (!isActive) {
            Font font;
            Text textSFML;
            font.loadFromFile(PREFIX + "Resources/font.ttf");
            textSFML.setFont(font);
            textSFML.setCharacterSize(24);
            textSFML.setFillColor(Color(0x999999ff));
            textSFML.setOutlineColor(Color::Black);
            textSFML.setOutlineThickness(-0.2f);
            textSFML.setString(currentLanguage[25]);
            textSFML.setPosition(startPos.x + 10, startPos.y + 0.25 * height);
            window->draw(textSFML);
        }
        else
            printText({startPos.x + 10, static_cast<float>(startPos.y + 0.25 * height)}, text, 24, -0.2f, currentLanguageOffset[25]);
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + width
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + height;
    }
} textType;

struct SelectButton {
    Position startPos;
    int color;

    void init() {
        startPos = {settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth - 1 * (settingsMenuSecondary.buttonHeight), settingsMenuSecondary.startPos.y + 7 * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        CircleShape innerCircle;
        CircleShape outterCircle;

        innerCircle.setOrigin(settingsMenuSecondary.buttonHeight * 0.29, settingsMenuSecondary.buttonHeight * 0.29);
        outterCircle.setOrigin(settingsMenuSecondary.buttonHeight * 0.39, settingsMenuSecondary.buttonHeight * 0.39);

        innerCircle.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        outterCircle.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);

        innerCircle.setFillColor(Color(color));
        outterCircle.setFillColor(Color::Blue);

        innerCircle.setOutlineThickness(-2.f);
        outterCircle.setOutlineThickness(-2.f);

        innerCircle.setOutlineColor(Color::Black);
        outterCircle.setOutlineColor(Color::Black);

        innerCircle.setRadius(settingsMenuSecondary.buttonHeight * 0.29);
        outterCircle.setRadius(settingsMenuSecondary.buttonHeight * 0.39);

        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(startPos.x, startPos.y);
        rectangle.setFillColor(Color(color));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight, settingsMenuSecondary.buttonHeight));

        window->draw(rectangle);
        window->draw(outterCircle);
        window->draw(innerCircle);
    }

    void displayHelp(Position mousePos) {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(mousePos.x, mousePos.y);
        rectangle.setFillColor(Color(menuColor));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 1.05, settingsMenuSecondary.buttonHeight * 0.45));
        window->draw(rectangle);
        printText({static_cast<float>(mousePos.x + settingsMenuSecondary.buttonHeight * 0.25), static_cast<float>(mousePos.y + settingsMenuSecondary.buttonHeight * 0.05)}, currentLanguage[28], 17, -0.5f, currentLanguageOffset[28]);
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + settingsMenuSecondary.buttonHeight
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + settingsMenuSecondary.buttonHeight;
    }
} selectButton;

struct EraseButton {
    Position startPos;
    int color;

    void init() {
        startPos = {settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth - 2 * (settingsMenuSecondary.buttonHeight) + 2, settingsMenuSecondary.startPos.y + 7 * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        RectangleShape rectangle1;
        rectangle1.setOrigin(settingsMenuSecondary.buttonHeight * 0.1, settingsMenuSecondary.buttonHeight * 0.42);
        rectangle1.setOutlineColor(Color::Black);
        rectangle1.setOutlineThickness(-2.f);
        rectangle1.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        rectangle1.setFillColor(Color::Red);
        rectangle1.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 0.2, settingsMenuSecondary.buttonHeight * 0.84));
        rectangle1.rotate(-45);

        RectangleShape rectangle2;
        rectangle2.setOrigin(settingsMenuSecondary.buttonHeight * 0.1, settingsMenuSecondary.buttonHeight * 0.42);
        rectangle2.setOutlineColor(Color::Black);
        rectangle2.setOutlineThickness(-2.f);
        rectangle2.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        rectangle2.setFillColor(Color::Red);
        rectangle2.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 0.2, settingsMenuSecondary.buttonHeight * 0.84));
        rectangle2.rotate(45);

        RectangleShape rectangle3;
        rectangle3.setOrigin(settingsMenuSecondary.buttonHeight * 0.07, settingsMenuSecondary.buttonHeight * 0.39);
        rectangle3.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        rectangle3.setFillColor(Color::Red);
        rectangle3.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 0.14, settingsMenuSecondary.buttonHeight * 0.78));
        rectangle3.rotate(-45);

        RectangleShape rectangle4;
        rectangle4.setOrigin(settingsMenuSecondary.buttonHeight * 0.07, settingsMenuSecondary.buttonHeight * 0.39);
        rectangle4.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        rectangle4.setFillColor(Color::Red);
        rectangle4.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 0.14, settingsMenuSecondary.buttonHeight * 0.78));
        rectangle4.rotate(45);

        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(startPos.x, startPos.y);
        rectangle.setFillColor(Color(color));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight, settingsMenuSecondary.buttonHeight));

        window->draw(rectangle);
        window->draw(rectangle1);
        window->draw(rectangle2);
        window->draw(rectangle3);
        window->draw(rectangle4);
    }

    void displayHelp(Position mousePos) {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(mousePos.x, mousePos.y);
        rectangle.setFillColor(Color(menuColor));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 1.05, settingsMenuSecondary.buttonHeight * 0.45));
        window->draw(rectangle);
        printText({static_cast<float>(mousePos.x + settingsMenuSecondary.buttonHeight * 0.25), static_cast<float>(mousePos.y + settingsMenuSecondary.buttonHeight * 0.05)}, currentLanguage[27], 17, -0.5f, currentLanguageOffset[27]);
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + settingsMenuSecondary.buttonHeight
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + settingsMenuSecondary.buttonHeight;
    }
} eraseButton;

struct NewFileButton {
    Position startPos;
    int color;
    bool visibility;
    bool isHovered;

    void init() {
        startPos = {settingsMenuSecondary.startPos.x + settingsMenuSecondary.buttonWidth - 3 * (settingsMenuSecondary.buttonHeight) + 4, settingsMenuSecondary.startPos.y + 7 * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        CircleShape circle;
        circle.setOrigin(settingsMenuSecondary.buttonHeight * 0.39, settingsMenuSecondary.buttonHeight * 0.39);
        circle.setPosition(startPos.x + settingsMenuSecondary.buttonHeight / 2, startPos.y + settingsMenuSecondary.buttonHeight / 2);
        circle.setFillColor(Color::Yellow);
        circle.setOutlineThickness(-2.f);
        circle.setOutlineColor(Color::Black);
        circle.setRadius(settingsMenuSecondary.buttonHeight * 0.39);
        circle.setPointCount(7);

        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(startPos.x, startPos.y);
        rectangle.setFillColor(Color(color));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight, settingsMenuSecondary.buttonHeight));

        window->draw(rectangle);
        window->draw(circle);
    }

    void displayHelp(Position mousePos) {
        RectangleShape rectangle;
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(mousePos.x, mousePos.y);
        rectangle.setFillColor(Color(menuColor));
        rectangle.setSize(Vector2f(settingsMenuSecondary.buttonHeight * 1.05, settingsMenuSecondary.buttonHeight * 0.45));
        window->draw(rectangle);
        printText({static_cast<float>(mousePos.x + settingsMenuSecondary.buttonHeight * 0.25), static_cast<float>(mousePos.y + settingsMenuSecondary.buttonHeight * 0.05)}, currentLanguage[26], 17, -0.5f, currentLanguageOffset[26]);
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + settingsMenuSecondary.buttonHeight
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + settingsMenuSecondary.buttonHeight;
    }
} newFileButton;

void hoverControlsFM(Position mousePosition) {
    selectButton .color = (selectButton .isWithin(mousePosition) ? hoveredColor : menuColor);
    eraseButton  .color = (eraseButton  .isWithin(mousePosition) ? hoveredColor : menuColor);
    newFileButton.color = (newFileButton.isWithin(mousePosition) ? hoveredColor : menuColor);
}

void initControlsFM() {
    selectButton.init();
    eraseButton.init();
    newFileButton.init();
    textType.init();
}

Position lastMousePos;

void displayControlsFM() {
    textType.displayText();
    selectButton.display();
    if (selectButton.isWithin(lastMousePos))
        selectButton.displayHelp(lastMousePos);
    eraseButton.display();
    if (eraseButton.isWithin(lastMousePos))
        eraseButton.displayHelp(lastMousePos);
    if (newFileButton.visibility) {
        newFileButton.display();
        if (newFileButton.isWithin(lastMousePos))
            newFileButton.displayHelp(lastMousePos);
    }
}

struct ButtonFile1 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 1;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile1;

struct ButtonFile2 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 2;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile2;

struct ButtonFile3 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 3;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile3;

struct ButtonFile4 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 4;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile4;

struct ButtonFile5 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 5;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile5;

struct ButtonFile6 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool visibility;

    string name;
    string path;

    void init() {
        visibility = false;
        buttonNumber = 6;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, name, color, -settingsMenuSecondary.buttonWidth * 0.40);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonFile6;

void initButtonFiles() {
    buttonFile1.init();
    buttonFile2.init();
    buttonFile3.init();
    buttonFile4.init();
    buttonFile5.init();
    buttonFile6.init();
    initControlsFM();
}

void displayButtonFiles() {
    if (buttonFile1.visibility) {
        if (fileManager.selectedFileName == buttonFile1.name)
            buttonFile1.color = clickedColor;
        buttonFile1.display();
    }
    if (buttonFile2.visibility) {
        if (fileManager.selectedFileName == buttonFile2.name)
            buttonFile2.color = clickedColor;
        buttonFile2.display();
    }
    if (buttonFile3.visibility) {
        if (fileManager.selectedFileName == buttonFile3.name)
            buttonFile3.color = clickedColor;
        buttonFile3.display();
    }
    if (buttonFile4.visibility) {
        if (fileManager.selectedFileName == buttonFile4.name)
            buttonFile4.color = clickedColor;
        buttonFile4.display();
    }
    if (buttonFile5.visibility) {
        if (fileManager.selectedFileName == buttonFile5.name)
            buttonFile5.color = clickedColor;
        buttonFile5.display();
    }
    if (buttonFile6.visibility) {
        if (fileManager.selectedFileName == buttonFile6.name)
            buttonFile6.color = clickedColor;
        buttonFile6.display();
    }
}

void hoverButtonFiles(Position mousePosition) {
    buttonFile1.color = (buttonFile1.isWithin(mousePosition) && buttonFile1.visibility ? hoveredColor : menuColor);
    buttonFile2.color = (buttonFile2.isWithin(mousePosition) && buttonFile2.visibility ? hoveredColor : menuColor);
    buttonFile3.color = (buttonFile3.isWithin(mousePosition) && buttonFile3.visibility ? hoveredColor : menuColor);
    buttonFile4.color = (buttonFile4.isWithin(mousePosition) && buttonFile4.visibility ? hoveredColor : menuColor);
    buttonFile5.color = (buttonFile5.isWithin(mousePosition) && buttonFile5.visibility ? hoveredColor : menuColor);
    buttonFile6.color = (buttonFile6.isWithin(mousePosition) && buttonFile6.visibility ? hoveredColor : menuColor);
}

void updatePage() {
    fileManager.readInfo();
    initButtonFiles();
    for (auto itA = fileManager.filePaths.begin(), itB = fileManager.fileNames.begin(); itA != fileManager.filePaths.end() || itB != fileManager.fileNames.end(); ++itA, ++itB) {
        bool visibility = (itA - fileManager.filePaths.begin()) + fileManager.currentPage * 6 <= fileManager.filePaths.size() - 1;
        switch (itA - fileManager.filePaths.begin()) {
          case 0:
            if (visibility) {
                buttonFile1.path = *(itA + fileManager.currentPage * 6);
                buttonFile1.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile1.visibility = visibility;
            break;
          case 1:
            if (visibility) {
                buttonFile2.path = *(itA + fileManager.currentPage * 6);
                buttonFile2.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile2.visibility = visibility;
            break;
          case 2:
            if (visibility) {
                buttonFile3.path = *(itA + fileManager.currentPage * 6);
                buttonFile3.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile3.visibility = visibility;
            break;
          case 3:
            if (visibility) {
                buttonFile4.path = *(itA + fileManager.currentPage * 6);
                buttonFile4.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile4.visibility = visibility;
            break;
          case 4:
            if (visibility) {
                buttonFile5.path = *(itA + fileManager.currentPage * 6);
                buttonFile5.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile5.visibility = visibility;
            break;
          case 5:
            if (visibility) {
                buttonFile6.path = *(itA + fileManager.currentPage * 6);
                buttonFile6.name = *(itB + fileManager.currentPage * 6);
            }
            buttonFile6.visibility = visibility;
            break;
        }
    }
}

struct ButtonPreviousPage {
    Position startPosButton;
    unsigned int color;
    bool isActive;

    void display() {
        startPosButton = fileManager.pageButtonTemplate(1, isActive, color);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonHeight
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonPrevPage;

struct ButtonNextPage {
    Position startPosButton;
    unsigned int color;
    bool isActive;

    void display() {
        startPosButton = fileManager.pageButtonTemplate(0, isActive, color);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonHeight
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} buttonNextPage;

void displayFileManager() {
    fileManager.displayPageBox();
    fileManager.displayPageNum();
    buttonPrevPage.display();
    buttonNextPage.display();
    displayButtonFiles();
    fileManager.displayControlsBox();
    displayControlsFM();
}

void hoverFileManager(Position mousePosition) {
    buttonPrevPage.color = (buttonPrevPage.isWithin(mousePosition) && buttonPrevPage.isActive ? hoveredColor : menuColor);
    buttonNextPage.color = (buttonNextPage.isWithin(mousePosition) && buttonNextPage.isActive ? hoveredColor : menuColor);
    hoverButtonFiles(mousePosition);
    hoverControlsFM(mousePosition);
}

void updateFileManager() {
    buttonPrevPage.isActive = (fileManager.currentPage > 0);
    buttonNextPage.isActive = (fileManager.currentPage < fileManager.numberOfPages);
}

void displaySettingsMenu() {
    settingsMenu.display();
    if (saveButton.clicked)
        saveButton.color = clickedColor;
    saveButton.display();
    if (loadButton.clicked)
        loadButton.color = clickedColor;
    loadButton.display();
    exitButton.display();
    closeButton.display();
    if (languageButton.clicked)
        languageButton.color = clickedColor;
    languageButton.display();
    if (themeButton.clicked)
        themeButton.color = clickedColor;
    themeButton.display();
    displayTheme();
    displayLanguage();
    if (saveButton.clicked || loadButton.clicked)
        displayFileManager();
}
