Circuit circuit;
int selectedNode;
int selectedEdge;

string currentLanguage[50];
float currentLanguageOffset[50];

unsigned long long int menuColor;
unsigned long long int clickedColor;
unsigned long long int hoveredColor;

void printText(Position pos, string text, float size, float outlineSize, float offset) {
    Font font;
    font.loadFromFile(PREFIX + "Resources/font.ttf");
    Text textSFML;
    textSFML.setFont(font);
    textSFML.setCharacterSize(size);
    textSFML.setFillColor(Color::White);
    textSFML.setOutlineColor(Color::Black);
    textSFML.setOutlineThickness(outlineSize);
    textSFML.setString(text);
    textSFML.setPosition(pos.x + offset, pos.y);
    window->draw(textSFML);
}

// TOP MENU

struct TopMenu {
    float width;
    float height;
    unsigned int color;
    Position startPos;
    float buttonWidth;
    float buttonHeight;
    float logoWidth;

    void init() {
        width = WDTH;
        height = HGHT / 15 / 20 * 20 + 20;
        buttonWidth = width / 9;
        buttonHeight = height;
        color = menuColor;
        startPos = {0, 0};
        logoWidth = WDTH / 13;
    }

    void logo() {
        Texture image;
        image.loadFromFile(PREFIX + "Resources/logo.png");
        RectangleShape rectangle(Vector2f(logoWidth, buttonHeight));
        rectangle.setTexture(&image);
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-1.f);
        rectangle.setPosition(0, 0);
        window->draw(rectangle);
    }

    void buttonTemplate(float buttonNumber, string text, unsigned int buttonColor, float offset) {
        RectangleShape rectangle(Vector2f(buttonWidth, buttonHeight));
        rectangle.setFillColor(Color(buttonColor));
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-1.f);
        rectangle.setPosition(logoWidth + buttonNumber * buttonWidth, 0);
        window->draw(rectangle);
        printText({logoWidth + buttonNumber * buttonWidth + WDTH / 25, HGHT / 65}, text, 24, 2.f, offset);
    }

    void box() {
        RectangleShape rectangle(Vector2f(width, height));
        rectangle.setFillColor(Color(color));
        rectangle.setOutlineColor(Color(0xffffffff));
        rectangle.setOutlineThickness(-1.f);
        window->draw(rectangle);
        logo();
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + width
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + height;
    }

    bool isWithinLogo(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + logoWidth
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + buttonHeight;
    }

} topMenu;

struct SettingsButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 0;
        startPosButton = {topMenu.logoWidth + buttonNumber * topMenu.buttonWidth, 0};
    }

    void display() {
        topMenu.buttonTemplate(buttonNumber, currentLanguage[1], color, currentLanguageOffset[1]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + topMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + topMenu.buttonHeight;
    }
} settingsButton;

void displayTopMenu() {
    topMenu.box();
    settingsButton.display();
}

// SETTINGS MENU

struct SettingsMenu {
    float width;
    float height;
    unsigned int color;
    Position startPos;
    float buttonWidth;
    float buttonHeight;
    bool visibility;

    void init() {
        color = menuColor;
        width = WDTH * 0.25;
        buttonWidth = width;
        buttonHeight = 70;
        height = 8 * buttonHeight;
        startPos = {(WDTH - width) / 2, (HGHT - height) / 2};
        visibility = false;
    }

    void buttonTemplate(float buttonNumber, string text, unsigned int buttonColor, float offset) {
        RectangleShape rectangle((Vector2f(buttonWidth, buttonHeight)));
        rectangle.setFillColor(Color(buttonColor));
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(startPos.x, startPos.y + buttonNumber * (buttonHeight - 2));
        window->draw(rectangle);
        printText({static_cast<float>(startPos.x + buttonWidth * 0.42), static_cast<float>(startPos.y + buttonNumber * (buttonHeight - 2) + buttonHeight * 0.2)}, text, 30, 2.f, offset);
    }

    void blur() {
        RectangleShape blur(Vector2f(WDTH, HGHT));
        blur.setFillColor(Color(0x000000bb));
        blur.setPosition(0, 0);
        window->draw(blur);
    }

    void display() {
        RectangleShape rectangle(Vector2f(width, height));
        rectangle.setFillColor(Color(color));
        rectangle.setOutlineColor(Color(0xffffffff));
        rectangle.setOutlineThickness(-2.f);
        rectangle.setPosition(startPos.x, startPos.y);
        window->draw(rectangle);
        settingsButton.color = menuColor;
    }
} settingsMenu, settingsMenuSecondary;

void settingsMenuSecondaryInit() {
    settingsMenuSecondary.color = menuColor;
    settingsMenuSecondary.width = WDTH * 0.33;
    settingsMenuSecondary.buttonWidth = settingsMenuSecondary.width;
    settingsMenuSecondary.buttonHeight = 70;
    settingsMenuSecondary.height = 8 * 68.25;
    settingsMenuSecondary.startPos = settingsMenu.startPos;
    settingsMenuSecondary.startPos.x += settingsMenu.width - 2;
    settingsMenuSecondary.visibility = false;
}

struct ThemeButton1 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 0;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[21], color, currentLanguageOffset[21]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} themeButton1;

struct ThemeButton2 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 1;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[22], color, currentLanguageOffset[22]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} themeButton2;

struct ThemeButton3 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 2;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[23], color, currentLanguageOffset[23]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} themeButton3;

struct LanguageButton1 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 0;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[18], color, currentLanguageOffset[18]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} languageButton1;

struct LanguageButton2 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 1;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[19], color, currentLanguageOffset[19]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} languageButton2;

struct LanguageButton3 {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 2;
        startPosButton = {settingsMenuSecondary.startPos.x, settingsMenuSecondary.startPos.y + buttonNumber * (settingsMenuSecondary.buttonHeight - 2)};
    }

    void display() {
        settingsMenuSecondary.buttonTemplate(buttonNumber, currentLanguage[20], color, currentLanguageOffset[20]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenuSecondary.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenuSecondary.buttonHeight;
    }
} languageButton3;

struct SaveButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool clicked;

    void init() {
        buttonNumber = 0;
        clicked = false;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[12], color, currentLanguageOffset[12]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} saveButton;

struct LoadButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool clicked;

    void init() {
        buttonNumber = 1;
        clicked = false;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[13], color, currentLanguageOffset[13]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} loadButton;

struct CloseButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 4;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[16], color, currentLanguageOffset[16]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} closeButton;

struct LanguageButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool clicked;

    void init() {
        buttonNumber = 3;
        clicked = false;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[15], color, currentLanguageOffset[15]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} languageButton;

struct ThemeButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;
    bool clicked;

    void init() {
        buttonNumber = 2;
        clicked = false;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[14], color, currentLanguageOffset[14]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} themeButton;

struct ExitButton {
    int buttonNumber;
    unsigned int color;
    Position startPosButton;

    void init() {
        buttonNumber = 8;
        startPosButton = {settingsMenu.startPos.x, settingsMenu.startPos.y + buttonNumber * (settingsMenu.buttonHeight - 2)};
    }

    void display() {
        settingsMenu.buttonTemplate(buttonNumber, currentLanguage[17], color, currentLanguageOffset[17]);
    }

    bool isWithin(Position searchedPos) {
        return startPosButton.x < searchedPos.x && searchedPos.x < startPosButton.x + settingsMenu.buttonWidth
            && startPosButton.y < searchedPos.y && searchedPos.y < startPosButton.y + settingsMenu.buttonHeight;
    }
} exitButton;

// MOUSE MENU

void displayTheme() {
    if (themeButton.clicked) {
        themeButton1.display();
        themeButton2.display();
        themeButton3.display();
    }
}

void displayLanguage() {
    if (languageButton.clicked) {
        languageButton1.display();
        languageButton2.display();
        languageButton3.display();
    }
}

void settingsMenuSecondaryButtons() {
    themeButton1.init();
    themeButton2.init();
    themeButton3.init();
    languageButton1.init();
    languageButton2.init();
    languageButton3.init();
}

void initSettingsMenu() {
    settingsMenu.init();
    saveButton.init();
    loadButton.init();
    exitButton.init();
    closeButton.init();
    languageButton.init();
    themeButton.init();
}

void hoverThemes(Position mousePos) {
    themeButton1.color = (themeButton1.isWithin(mousePos) ? hoveredColor : menuColor);
    themeButton2.color = (themeButton2.isWithin(mousePos) ? hoveredColor : menuColor);
    themeButton3.color = (themeButton3.isWithin(mousePos) ? hoveredColor : menuColor);
}

void hoverLanguages(Position mousePos) {
    languageButton1.color = (languageButton1.isWithin(mousePos) ? hoveredColor : menuColor);
    languageButton2.color = (languageButton2.isWithin(mousePos) ? hoveredColor : menuColor);
    languageButton3.color = (languageButton3.isWithin(mousePos) ? hoveredColor : menuColor);
}

void hoverSettingsMenu(Position mousePos) {
    saveButton    .color = (saveButton    .isWithin(mousePos) ? hoveredColor : menuColor);
    loadButton    .color = (loadButton    .isWithin(mousePos) ? hoveredColor : menuColor);
    closeButton   .color = (closeButton   .isWithin(mousePos) ? hoveredColor : menuColor);
    exitButton    .color = (exitButton    .isWithin(mousePos) ? hoveredColor : menuColor);
    languageButton.color = (languageButton.isWithin(mousePos) ? hoveredColor : menuColor);
    themeButton   .color = (themeButton   .isWithin(mousePos) ? hoveredColor : menuColor);
    hoverThemes(mousePos);
    hoverLanguages(mousePos);
}

struct MouseMenu {
    float width;
    float height;
    unsigned int color;
    Position startPos;
    bool visibility;
    float numberOfButtons;
    float buttonWidth;
    float buttonHeight;

    void init() {
        width = WDTH * 0.0785;
        color = menuColor;
        visibility = false;
        numberOfButtons = 4;
        buttonWidth = width;
        buttonHeight = 30;
        height = buttonHeight * numberOfButtons;
    }

    void buttonTemplate(float buttonNumber, string text, unsigned int buttonColor) {
        RectangleShape rectangle(Vector2f(buttonWidth, buttonHeight));
        rectangle.setFillColor(Color(buttonColor));
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-1.f);
        rectangle.setPosition(startPos.x, startPos.y + buttonNumber * buttonHeight);
        window->draw(rectangle);
        printText({static_cast<float>(startPos.x + buttonWidth * 0.04), startPos.y + buttonNumber * buttonHeight + 2}, text, 18, 1.f, 0);
    }

    void box() {
        RectangleShape rectangle(Vector2f(width, height));
        rectangle.setFillColor(Color(color));
        rectangle.setOutlineColor(Color(0xffffffff));
        rectangle.setOutlineThickness(-1.f);
        rectangle.setPosition(startPos.x, startPos.y);
        window->draw(rectangle);
    }

    void shadow() {
        RectangleShape rectangle(Vector2f(width, height));
        rectangle.setFillColor(Color(0x00000055));
        rectangle.setPosition(startPos.x + 13, startPos.y + 13);
        window->draw(rectangle);
    }

    void display() {
        shadow();
        box();
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + width
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + height;
    }
} menuMouse, menuNew, menuEdge, menuNode;

struct MouseButtonRemoveNode {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 0;
        startPosButton = {menuNode.startPos.x, menuNode.startPos.y + buttonNumber * menuNode.buttonHeight};
    }

    void display() {
        menuNode.buttonTemplate(buttonNumber, currentLanguage[29], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNode.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNode.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} mouseButtonRemoveNode;

struct MouseButtonRotateLeft {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 1;
        startPosButton = {menuNode.startPos.x, menuNode.startPos.y + buttonNumber * menuNode.buttonHeight};
    }

    void display() {
        menuNode.buttonTemplate(buttonNumber, currentLanguage[30], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNode.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNode.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} mouseButtonRotateLeft;

struct MouseButtonRemove {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 0;
        startPosButton = {menuEdge.startPos.x, menuEdge.startPos.y + buttonNumber * menuEdge.buttonHeight};
    }

    void display() {
        menuEdge.buttonTemplate(buttonNumber, currentLanguage[10], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuEdge.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuEdge.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonRemove;

struct MouseButtonRotate {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 1;
        startPosButton = {menuEdge.startPos.x, menuEdge.startPos.y + buttonNumber * menuEdge.buttonHeight};
    }

    void display() {
        menuEdge.buttonTemplate(buttonNumber, currentLanguage[11], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuEdge.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuEdge.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonRotate;

void menuNewInit() {
    menuNew.width = WDTH * 0.096;
    menuNew.color = menuColor;
    menuNew.visibility = false;
    menuNew.numberOfButtons = 8;
    menuNew.buttonWidth = menuNew.width;
    menuNew.buttonHeight = 30;
    menuNew.height = menuNew.numberOfButtons * menuNew.buttonHeight;
}

struct MouseButtonNew {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 0;
        startPosButton = {menuMouse.startPos.x, menuMouse.startPos.y + buttonNumber * menuMouse.buttonHeight};
    }

    void display() {
        menuMouse.buttonTemplate(buttonNumber, currentLanguage[2], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuMouse.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuMouse.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonNew;

struct MouseButtonResistor {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 0;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[3], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonResistor;

struct MouseButtonDiode {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 1;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[4], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonDiode;

struct MouseButtonSwitch {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 2;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[5], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonSwitch;

struct MouseButtonCapacitor {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 3;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[6], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonCapacitor;

struct MouseButtonVoltageSource {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 4;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[7], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonVoltageSource;

struct MouseButtonCurrentSource {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 5;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[8], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonCurrentSource;

struct MouseButtonIntersection4 {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 6;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[9], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonIntersection4;

struct MouseButtonIntersection3 {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 7;
        startPosButton = {menuNew.startPos.x, menuNew.startPos.y + buttonNumber * menuNew.buttonHeight};
    }

    void display() {
        menuNew.buttonTemplate(buttonNumber, currentLanguage[32], color);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNew.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNew.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} buttonIntersection3;

void mouseHoverMenuEdge(Position mousePosition) {
    buttonRemove.color = (buttonRemove.isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonRotate.color = (buttonRotate.isWithin(mousePosition) ? hoveredColor : menuColor);
}

void mouseHoverMenuNew(Position mousePosition) {
    buttonResistor     .color = (buttonResistor     .isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonDiode        .color = (buttonDiode        .isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonSwitch       .color = (buttonSwitch       .isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonCapacitor    .color = (buttonCapacitor    .isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonVoltageSource.color = (buttonVoltageSource.isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonCurrentSource.color = (buttonCurrentSource.isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonIntersection4.color = (buttonIntersection4.isWithin(mousePosition) ? hoveredColor : menuColor);
    buttonIntersection3.color = (buttonIntersection3.isWithin(mousePosition) ? hoveredColor : menuColor);
}

void displayMouseMenu() {
    if (menuNew.visibility) {
        buttonNew.color = clickedColor;
        menuNew.shadow();
        menuMouse.shadow();
        menuNew.box();
        menuMouse.box();
        buttonResistor.display();
        buttonDiode.display();
        buttonSwitch.display();
        buttonCapacitor.display();
        buttonVoltageSource.display();
        buttonCurrentSource.display();
        buttonIntersection4.display();
        buttonIntersection3.display();
    }
    else
        menuMouse.display();
    buttonNew.display();
}

void displayMenuEdge() {
    menuEdge.display();
    buttonRemove.display();
    buttonRotate.display();
}

// APP SCREEN

struct AppScreen {
    float width;
    float height;
    unsigned int color;
    Position startPos;
    string imgFile;
    unsigned int imgVisibility;

    void init() {
        width = WDTH;
        height = HGHT - topMenu.height;
        startPos = {0, topMenu.height};
    }

    void display() {
        Texture image;
        image.loadFromFile(imgFile);

        RectangleShape rectangle;
        rectangle.setTexture(&image);
        rectangle.setOutlineColor(Color::White);
        rectangle.setOutlineThickness(-1.f);
        rectangle.setPosition(startPos.x, startPos.y);
        rectangle.setFillColor(Color(imgVisibility));
        rectangle.setSize(Vector2f(width, height));

        RectangleShape rectangle2;
        rectangle2.setPosition(startPos.x, startPos.y);
        rectangle2.setSize(Vector2f(width, height));
        rectangle2.setFillColor(Color(color));

        window->draw(rectangle2);
        window->draw(rectangle);
    }

    bool isWithin(Position searchedPos) {
        return startPos.x < searchedPos.x && searchedPos.x < startPos.x + width
            && startPos.y < searchedPos.y && searchedPos.y < startPos.y + height;
    }
} appScreen;

void loadTheme(const string& file) {
    ifstream fin(file);
    fin >> menuColor >> clickedColor >> hoveredColor;
    fin >> appScreen.color >> appScreen.imgVisibility >> appScreen.imgFile;
    appScreen.imgFile = PREFIX + appScreen.imgFile;
    fin.close();
    menuMouse.color = menuNew.color = menuEdge.color = topMenu.color = menuNode.color = settingsMenu.color = settingsMenuSecondary.color = menuColor;
}

void loadLanguage(const string& file) {
    ifstream fin(file);
    for (int i = 1; i <= 32; i++) {
        fin >> currentLanguage[i];
        for (char& ch : currentLanguage[i])
            if (ch == '_')
                ch = ' ';
        fin >> currentLanguageOffset[i];
    }
    fin.close();
}

struct Settings {
    string theme;
    string language;

    void load() {
        ifstream fin(PREFIX + "Resources/settings.electron");
        fin >> theme >> language;
        loadTheme(PREFIX + theme);
        loadLanguage(PREFIX + language);
        fin.close();
    }

    void save() {
        ofstream fout(PREFIX + "Resources/settings.electron");
        fout << theme << '\n' << language << '\n';
        fout.close();
    }
} settings;

void settingsMenuReset() {
    saveButton.color = menuColor;
    loadButton.color = menuColor;
    themeButton.color = menuColor;
    languageButton.color = menuColor;
    saveButton.clicked = false;
    loadButton.clicked = false;
    themeButton.clicked = false;
    languageButton.clicked = false;
}

void updateMouseMenu() {
    menuMouse.visibility = false;
    buttonNew.init();
    exitButton.init();
    buttonResistor.init();
    buttonDiode.init();
    buttonSwitch.init();
    buttonCapacitor.init();
    buttonVoltageSource.init();
    buttonCurrentSource.init();
    buttonIntersection4.init();
    buttonIntersection3.init();
}

void updateMenuEdge() {
    buttonRemove.init();
    buttonRotate.init();
}

struct MouseButtonRotateRight {
    Position startPosButton;
    float buttonNumber;
    unsigned int color;

    void init() {
        buttonNumber = 2;
        startPosButton = {menuNode.startPos.x, menuNode.startPos.y + buttonNumber * menuNode.buttonHeight};
    }

    void display() {
        menuNode.buttonTemplate(buttonNumber, currentLanguage[31], color);
        if (settings.language == "Languages/english.language")
            printText({startPosButton.x + 4, startPosButton.y + 32}, "Value: " + circuit.getNodeInfo(selectedNode), 18, 1, 0);
        else if (settings.language == "Languages/romana.language")
            printText({startPosButton.x + 4, startPosButton.y + 32}, "Valoare: " + circuit.getNodeInfo(selectedNode), 18, 1, 0);
        else if (settings.language == "Languages/deutsch.language")
            printText({startPosButton.x + 4, startPosButton.y + 32}, "Value: " + circuit.getNodeInfo(selectedNode), 18, 1, 0);
    }

    bool isWithin(Position searchedPos) {
        if ((searchedPos.x < startPosButton.x + menuNode.buttonWidth && searchedPos.x > startPosButton.x) &&
            (searchedPos.y < startPosButton.y + menuNode.buttonHeight && searchedPos.y > startPosButton.y))
            return true;
        return false;
    }
} mouseButtonRotateRight;

void mouseHoverMenuNode(Position mousePosition) {
    mouseButtonRemoveNode .color = (mouseButtonRemoveNode .isWithin(mousePosition) ? hoveredColor : menuColor);
    mouseButtonRotateLeft .color = (mouseButtonRotateLeft .isWithin(mousePosition) ? hoveredColor : menuColor);
    mouseButtonRotateRight.color = (mouseButtonRotateRight.isWithin(mousePosition) ? hoveredColor : menuColor);
}

void displayMenuNode() {
    menuNode.display();
    mouseButtonRemoveNode.display();
    mouseButtonRotateLeft.display();
    mouseButtonRotateRight.display();
}

void updateMenuNode() {
    mouseButtonRemoveNode.init();
    mouseButtonRotateLeft.init();
    mouseButtonRotateRight.init();
}
