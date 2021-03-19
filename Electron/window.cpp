void initStartup() {
    settings.load();
    topMenu.init();
    appScreen.init();
    menuMouse.init();
    menuNewInit();
    menuEdge.init();
    menuNode.init();
    exitButton.init();
    settingsButton.init();
    initSettingsMenu();
    settingsMenuSecondaryInit();
    settingsMenuSecondaryButtons();

    srand(time(nullptr));
    Piece::LoadPieceTypes();

    fileManager.readInfo();
    fileManager.currentPage = 0;
    updateFileManager();
    updatePage();
    selectedNode = -1;
    selectedEdge = -1;
}

void openWindow() {
    window = new RenderWindow(VideoMode(WDTH, HGHT), "Electron", Style::Close);
    window->setFramerateLimit(50);
    initStartup();
}

void mouseHoverAction(Position mousePosition) {
    buttonNew     .color = (buttonNew     .isWithin(mousePosition) ? hoveredColor : menuColor);
    settingsButton.color = (settingsButton.isWithin(mousePosition) ? hoveredColor : menuColor);
}

bool draggingCanvas = false;
Position lastMousePosition;

void pollEvents() {
    Event event;
    while (window->pollEvent(event)) {
        if (!gameOpen) {
            Position mousePosition;
            mousePosition.x = Mouse::getPosition(*window).x;
            mousePosition.y = Mouse::getPosition(*window).y;
            lastMousePos = {mousePosition.x + 5, mousePosition.y + 5};

            if (draggingCanvas) {
                circuit.xOffset += mousePosition.x - lastMousePosition.x;
                circuit.yOffset += mousePosition.y - lastMousePosition.y;
                bool ok = false;
                if (circuit.xOffset + 2 * WDTH * circuit.getZoom() < WDTH) ok = true;
                if (circuit.xOffset - 1 * WDTH * circuit.getZoom() >    0) ok = true;
                if (circuit.yOffset + 2 * HGHT * circuit.getZoom() < HGHT) ok = true;
                if (circuit.yOffset - 1 * HGHT * circuit.getZoom() >    0) ok = true;
                if (ok) {
                    circuit.xOffset -= mousePosition.x - lastMousePosition.x;
                    circuit.yOffset -= mousePosition.y - lastMousePosition.y;
                }
            }
            lastMousePosition = mousePosition;
            bool draggable = true;

            if (circuit.activeNode != -1 && (
                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20 != circuit.getNodeX(circuit.activeNode) ||
                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20 != circuit.getNodeY(circuit.activeNode)))
                    circuit.moveNode(
                        circuit.activeNode,
                        circuit.canvasToCircuitX(mousePosition.x) / 20 * 20 - circuit.getNodeX(circuit.activeNode),
                        circuit.canvasToCircuitY(mousePosition.y) / 20 * 20 - circuit.getNodeY(circuit.activeNode)
                    );

            if (event.type == Event::Closed)
                window->close();

            if (!settingsMenu.visibility)
                mouseHoverAction(mousePosition);
            else
                hoverSettingsMenu(mousePosition);

            if (menuNew.visibility)
                mouseHoverMenuNew(mousePosition);
            if (menuEdge.visibility)
                mouseHoverMenuEdge(mousePosition);
            if (menuNode.visibility)
                mouseHoverMenuNode(mousePosition);
            if (saveButton.clicked || loadButton.clicked)
                hoverFileManager(mousePosition);

            if (settingsMenu.visibility)
                draggable = false;

            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    if (topMenu.isWithin(mousePosition) || menuMouse.visibility || menuEdge.visibility || menuNode.visibility)
                        draggable = false;
                    if (menuNode.visibility) {
                        if (mouseButtonRemoveNode.isWithin(mousePosition)) {
                            mouseButtonRemoveNode.color = clickedColor;
                            mouseButtonRemoveNode.display();
                            window->display();
                            menuEdge.visibility = false;
                            circuit.eraseNode(selectedNode);
                            menuNode.visibility = false;
                            draggable = false;
                        }
                        if (mouseButtonRotateLeft.isWithin(mousePosition)) {
                            mouseButtonRotateLeft.color = clickedColor;
                            mouseButtonRotateLeft.display();
                            window->display();
                            menuEdge.visibility = false;
                            circuit.rotateNode(selectedNode);
                            menuNode.visibility = false;
                            draggable = false;
                        }
                        if (mouseButtonRotateRight.isWithin(mousePosition)) {
                            mouseButtonRotateRight.color = clickedColor;
                            mouseButtonRotateRight.display();
                            window->display();
                            menuEdge.visibility = false;
                            circuit.rotateNode(selectedNode);
                            circuit.rotateNode(selectedNode);
                            circuit.rotateNode(selectedNode);
                            menuNode.visibility = false;
                            draggable = false;
                        }
                    }
                    if (menuEdge.visibility) {
                        if (buttonRemove.isWithin(mousePosition)) {
                            buttonRemove.color = clickedColor;
                            buttonRemove.display();
                            window->display();
                            menuEdge.visibility = false;
                            circuit.eraseEdge(selectedEdge);
                            draggable = false;
                        }
                        if (buttonRotate.isWithin(mousePosition)) {
                            buttonRotate.color = clickedColor;
                            buttonRotate.display();
                            window->display();
                            menuEdge.visibility = false;
                            circuit.changeEdge(selectedEdge);
                            draggable = false;
                        }
                    }

                    bool justCreatedNode = false;
                    if (menuNew.visibility) {
                        if (buttonResistor.isWithin(mousePosition)) {
                            buttonResistor.color = clickedColor;
                            buttonResistor.display();
                            window->display();
                            circuit.addNode(
                                "Resistor",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonDiode.isWithin(mousePosition)) {
                            buttonDiode.color = clickedColor;
                            buttonDiode.display();
                            window->display();
                            circuit.addNode(
                                "Diode",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonSwitch.isWithin(mousePosition)) {
                            buttonSwitch.color = clickedColor;
                            buttonSwitch.display();
                            window->display();
                            circuit.addNode(
                                "Switch",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonCapacitor.isWithin(mousePosition)) {
                            buttonCapacitor.color = clickedColor;
                            buttonCapacitor.display();
                            window->display();
                            circuit.addNode(
                                "Capacitor",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonVoltageSource.isWithin(mousePosition)) {
                            buttonVoltageSource.color = clickedColor;
                            buttonVoltageSource.display();
                            window->display();
                            circuit.addNode(
                                "VoltageSource",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonCurrentSource.isWithin(mousePosition)) {
                            buttonCurrentSource.color = clickedColor;
                            buttonCurrentSource.display();
                            window->display();
                            circuit.addNode(
                                "CurrentSource",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        if (buttonIntersection4.isWithin(mousePosition)) {
                            buttonIntersection4.color = clickedColor;
                            buttonIntersection4.display();
                            window->display();
                            circuit.addNode(
                                "Intersection4",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        delay(150);
                        if (buttonIntersection3.isWithin(mousePosition)) {
                            buttonIntersection3.color = clickedColor;
                            buttonIntersection3.display();
                            window->display();
                            circuit.addNode(
                                "Intersection3",
                                circuit.canvasToCircuitX(mousePosition.x) / 20 * 20,
                                circuit.canvasToCircuitY(mousePosition.y) / 20 * 20
                            );
                            justCreatedNode = true;
                            draggable = false;
                        }
                        delay(150);
                    }

                    if (settingsMenu.visibility) {
                        if (topMenu.isWithinLogo(mousePosition))
                            openSnake();
                        if (saveButton.clicked || loadButton.clicked) {
                            if (buttonPrevPage.isWithin(mousePosition) && buttonPrevPage.isActive) {
                                buttonPrevPage.color = clickedColor;
                                buttonPrevPage.display();
                                window->display();
                                fileManager.currentPage--;
                                updatePage();
                            }
                            if (buttonNextPage.isWithin(mousePosition) && buttonNextPage.isActive) {
                                buttonNextPage.color = clickedColor;
                                buttonNextPage.display();
                                window->display();
                                fileManager.currentPage++;
                                updatePage();
                            }
                            if (buttonFile1.isWithin(mousePosition) && buttonFile1.visibility) {
                                buttonFile1.color = clickedColor;
                                buttonFile1.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile1.path;
                                fileManager.selectedFileName = buttonFile1.name;
                            }
                            if (buttonFile2.isWithin(mousePosition) && buttonFile2.visibility) {
                                buttonFile2.color = clickedColor;
                                buttonFile2.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile2.path;
                                fileManager.selectedFileName = buttonFile2.name;
                            }
                            if (buttonFile3.isWithin(mousePosition) && buttonFile3.visibility) {
                                buttonFile3.color = clickedColor;
                                buttonFile3.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile3.path;
                                fileManager.selectedFileName = buttonFile3.name;
                            }
                            if (buttonFile4.isWithin(mousePosition) && buttonFile4.visibility) {
                                buttonFile4.color = clickedColor;
                                buttonFile4.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile4.path;
                                fileManager.selectedFileName = buttonFile4.name;
                            }
                            if (buttonFile5.isWithin(mousePosition) && buttonFile5.visibility) {
                                buttonFile5.color = clickedColor;
                                buttonFile5.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile5.path;
                                fileManager.selectedFileName = buttonFile5.name;
                            }
                            if (buttonFile6.isWithin(mousePosition) && buttonFile6.visibility) {
                                buttonFile6.color = clickedColor;
                                buttonFile6.display();
                                window->display();
                                fileManager.selectedFilePath = buttonFile6.path;
                                fileManager.selectedFileName = buttonFile6.name;
                            }
                            if (selectButton.isWithin(mousePosition)) {
                                selectButton.color = clickedColor;
                                selectButton.display();
                                window->display();
                                if (loadButton.clicked && fileManager.selectedFilePath != "")
                                    circuit.loadFromFile(fileManager.selectedFilePath);
                                if (saveButton.clicked && fileManager.selectedFilePath != "")
                                    circuit.saveInFile(fileManager.selectedFilePath);
                            }
                            if (eraseButton.isWithin(mousePosition)) {
                                eraseButton.color = clickedColor;
                                eraseButton.display();
                                window->display();
                                if (fileManager.selectedFilePath != "")
                                    fileManager.removeFile(fileManager.selectedFilePath, fileManager.selectedFileName);
                                fileManager.readInfo();
                                updatePage();
                            }
                            if (newFileButton.isWithin(mousePosition) && newFileButton.visibility) {
                                newFileButton.color = clickedColor;
                                newFileButton.display();
                                window->display();
                                if (textType.text != "")
                                    fileManager.newFile(PREFIX + "Saves/" + textType.text + ".circuit", textType.text);
                                textType.text = "";
                                textType.isActive = false;
                                fileManager.readInfo();
                                updatePage();
                            }
                            if (textType.isWithin(mousePosition)) {
                                textType.text = "";
                                textType.isActive = true;
                            }
                            else if (textType.isActive)
                                textType.isActive = false;
                        }
                        if (themeButton.clicked) {
                            if (themeButton1.isWithin(mousePosition)) {
                                themeButton1.color = clickedColor;
                                themeButton1.display();
                                window->display();
                                loadTheme(PREFIX + "Themes/blue.theme");
                                settings.theme = "Themes/blue.theme";
                                settings.save();
                            }
                            if (themeButton2.isWithin(mousePosition)) {
                                themeButton2.color = clickedColor;
                                themeButton2.display();
                                window->display();
                                loadTheme(PREFIX + "Themes/red.theme");
                                settings.theme = "Themes/red.theme";
                                settings.save();
                            }
                            if (themeButton3.isWithin(mousePosition)) {
                                themeButton3.color = clickedColor;
                                themeButton3.display();
                                window->display();
                                loadTheme(PREFIX + "Themes/christmas.theme");
                                settings.theme = "Themes/christmas.theme";
                                settings.save();
                            }
                        }
                        if (languageButton.clicked) {
                            if (languageButton1.isWithin(mousePosition)) {
                                languageButton1.color = clickedColor;
                                languageButton1.display();
                                window->display();
                                loadLanguage(PREFIX + "Languages/romana.language");
                                settings.language = "Languages/romana.language";
                                settings.save();
                            }
                            if (languageButton2.isWithin(mousePosition)) {
                                languageButton2.color = clickedColor;
                                languageButton2.display();
                                window->display();
                                loadLanguage(PREFIX + "Languages/english.language");
                                settings.language = "Languages/english.language";
                                settings.save();
                            }
                            if (languageButton3.isWithin(mousePosition)) {
                                languageButton3.color = clickedColor;
                                languageButton3.display();
                                window->display();
                                loadLanguage(PREFIX + "Languages/deutsch.language");
                                settings.language = "Languages/deutsch.language";
                                settings.save();
                            }
                        }
                        if (closeButton.isWithin(mousePosition)) {
                            closeButton.color = clickedColor;
                            closeButton.display();
                            window->display();
                            delay(200);
                            settingsMenu.visibility = false;
                            settingsMenuSecondary.visibility = false;
                            settingsMenuReset();
                        }
                        else if (exitButton.isWithin(mousePosition)) {
                            exitButton.color = clickedColor;
                            exitButton.display();
                            window->display();
                            delay(200);
                            window->close();
                        }
                        else if (saveButton.isWithin(mousePosition)) {
                            saveButton.color = clickedColor;
                            saveButton.display();
                            window->display();
                            bool temp = saveButton.clicked;
                            settingsMenuReset();
                            settingsMenuSecondary.visibility = !temp;
                            saveButton.clicked = !temp;
                            textType.init();
                            fileManager.selectedFileName = "";
                            fileManager.selectedFilePath = "";
                            newFileButton.visibility = true;
                        }
                        else if (loadButton.isWithin(mousePosition)) {
                            loadButton.color = clickedColor;
                            loadButton.display();
                            window->display();
                            bool temp = loadButton.clicked;
                            settingsMenuReset();
                            settingsMenuSecondary.visibility = !temp;
                            loadButton.clicked = !temp;
                            newFileButton.visibility = false;
                            textType.init();
                            fileManager.selectedFileName = "";
                            fileManager.selectedFilePath = "";
                        }
                        else if (themeButton.isWithin(mousePosition)) {
                            themeButton.color = clickedColor;
                            themeButton.display();
                            window->display();
                            bool temp = themeButton.clicked;
                            settingsMenuReset();
                            settingsMenuSecondary.visibility = !temp;
                            themeButton.clicked = !temp;
                        }
                        else if (languageButton.isWithin(mousePosition)) {
                            languageButton.color = clickedColor;
                            languageButton.display();
                            window->display();
                            bool temp = languageButton.clicked;
                            settingsMenuReset();
                            settingsMenuSecondary.visibility = !temp;
                            languageButton.clicked = !temp;
                        }
                    }
                    else if (settingsButton.isWithin(mousePosition)) {
                        draggingCanvas = false;
                        settingsButton.color = clickedColor;
                        settingsMenu.visibility = true;
                        menuMouse.visibility = false;
                        menuNew.visibility = false;
                        menuEdge.visibility = false;
                        menuNode.visibility = false;
                    }
                    else if (menuMouse.visibility) {
                        if (buttonNew.isWithin(mousePosition))
                            menuNew.visibility = 1 - menuNew.visibility;
                        else if (!menuMouse.isWithin(mousePosition)) {
                            menuMouse.visibility = false;
                            menuNew.visibility = false;
                        }
                    }
                    else if (!menuNode.isWithin(mousePosition) && menuNode.visibility)
                        menuNode.visibility = false;
                    else if (!menuEdge.isWithin(mousePosition) && menuEdge.visibility)
                        menuEdge.visibility = false;

                    if (!menuEdge.visibility && !menuNew.visibility && !settingsMenu.visibility && !menuMouse.visibility && !menuNode.visibility && !justCreatedNode) {
                        if (circuit.activeEnd.first == -1) {
                            circuit.activeEnd = circuit.hoveredEnd(mousePosition.x, mousePosition.y);
                            if (circuit.activeEnd.first != -1)
                                continue;
                        }
                        else {
                            auto end = circuit.hoveredEnd(mousePosition.x, mousePosition.y);
                            if (end.first != -1)
                                circuit.addEdge(circuit.activeEnd.first, circuit.activeEnd.second, end.first, end.second);
                            circuit.activeEnd = make_pair(-1, -1);
                            draggable = false;
                        }
                        if (circuit.activeNode == -1) {
                            circuit.activeNode = circuit.hoveredNode(mousePosition.x, mousePosition.y);
                            if (circuit.activeNode != -1)
                                draggable = false;
                        }
                        else {
                            circuit.activeNode = -1;
                            draggable = false;
                        }
                        if (circuit.hoveredNode(mousePosition.x, mousePosition.y) == -1 && circuit.hoveredEnd(mousePosition.x, mousePosition.y).first == -1 && draggable)
                            draggingCanvas ^= true;
                    }
                }

                if (event.mouseButton.button == Mouse::Right && !settingsMenu.visibility && !draggingCanvas) {
                    selectedEdge = circuit.hoveredEdge(mousePosition.x, mousePosition.y);
                    if (selectedEdge != -1 && !menuEdge.isWithin(mousePosition) && !topMenu.isWithin(mousePosition)) {
                        menuMouse.visibility = false;
                        menuNode.visibility = false;
                        Position posOfMenu = mousePosition;
                        if (posOfMenu.x + menuEdge.width + 15 >= WDTH)
                            posOfMenu.x -= menuEdge.width;
                        if (posOfMenu.y + menuEdge.height + 15 >= HGHT)
                            posOfMenu.y -= menuEdge.height;
                        menuEdge.startPos = posOfMenu;
                        menuEdge.visibility = true;
                        continue;
                    }

                    selectedNode = circuit.hoveredNode(mousePosition.x, mousePosition.y);
                    if (selectedNode != -1 && !menuNode.isWithin(mousePosition) && !topMenu.isWithin(mousePosition)) {
                        menuMouse.visibility = false;
                        menuEdge.visibility = false;
                        Position posOfMenu = mousePosition;
                        if (posOfMenu.x + menuNode.width + 15 >= WDTH)
                            posOfMenu.x -= menuNode.width;
                        if (posOfMenu.y + menuNode.height + 15 >= HGHT)
                            posOfMenu.y -= menuNode.height;
                        menuNode.startPos = posOfMenu;
                        menuNode.visibility = true;
                        continue;
                    }

                    if (appScreen.isWithin(mousePosition) && !menuMouse.isWithin(mousePosition)) {
                        menuEdge.visibility = false;
                        menuNode.visibility = false;
                        menuNew.visibility = false;
                        Position posOfMenu = mousePosition;
                        if (posOfMenu.x + menuNew.width + 15 >= WDTH)
                            posOfMenu.x -= menuMouse.width;
                        if (posOfMenu.y + menuNew.height + 15 >= HGHT)
                            posOfMenu.y -= menuMouse.height;
                        menuMouse.startPos = posOfMenu;
                        posOfMenu.x -= menuNew.width;
                        if (posOfMenu.x < 15)
                            posOfMenu.x += menuNew.width + menuMouse.width;
                        if (posOfMenu.y + menuNew.height + 15 >= HGHT)
                            posOfMenu.y += menuMouse.buttonHeight - menuNew.height;
                        menuNew.startPos = posOfMenu;
                        updateMouseMenu();
                        menuMouse.visibility = true;
                    }
                }
            }
            else if (event.type == Event::MouseWheelMoved && !settingsMenu.visibility) {
                if (event.mouseWheel.delta < 0 && circuit.getZoom() > 0.7) circuit.incZoom(-0.1);
                if (event.mouseWheel.delta > 0 && circuit.getZoom() < 2.5) circuit.incZoom(+0.1);
            }
            else if (event.type == Event::KeyPressed && !textType.isActive) {
                if (event.key.code == Keyboard::Escape) {
                    settingsMenu.visibility = !settingsMenu.visibility;
                     settingsMenuSecondary.visibility = false;
                    	draggingCanvas = false;
                        menuMouse.visibility = false;
                        menuNew.visibility = false;
                        menuEdge.visibility = false;
                        menuNode.visibility = false;
                     settingsMenuReset();
                }
                else if (event.key.code == Keyboard::BackSpace) {
                    if (selectedNode != -1 && menuNode.visibility) {
                        if (!circuit.getNodeInfo(selectedNode).empty())
                            circuit.setNodeInfo(selectedNode, circuit.getNodeInfo(selectedNode).substr(0, circuit.getNodeInfo(selectedNode).size() - 1));
                    }
                    else
                        circuit.undo();
                }
                else if (event.key.code == Keyboard::Tab)
                    circuit.redo();
            }
            else if (event.type == Event::TextEntered) {
                if (textType.isActive) {
                    Text playerText;
                    if (textType.text.size() < 40 && (
                        ('0' <= event.text.unicode && event.text.unicode <= '9') ||
                        ('a' <= event.text.unicode && event.text.unicode <= 'z') ||
                        ('A' <= event.text.unicode && event.text.unicode <= 'Z')))
                            textType.text += event.text.unicode;
                    else if (event.text.unicode == '\b' && !textType.text.empty())
                        textType.text.erase(textType.text.end() - 1, textType.text.end());
                    else if (event.text.unicode == 13) {
                        if (textType.text != "")
                            fileManager.newFile(PREFIX + "Saves/" + textType.text + ".circuit", textType.text);
                        textType.text = "";
                        textType.isActive = false;
                        fileManager.readInfo();
                        updatePage();
                    }
                }
                else if (menuNode.visibility && selectedNode != -1 && circuit.getNodeInfo(selectedNode).size() < 4 && (
                    ('0' <= event.text.unicode && event.text.unicode <= '9') ||
                    ('a' <= event.text.unicode && event.text.unicode <= 'z') ||
                    ('A' <= event.text.unicode && event.text.unicode <= 'Z') ||
                    event.text.unicode == '.' || event.text.unicode == ','))
                        circuit.setNodeInfo(selectedNode, circuit.getNodeInfo(selectedNode) + event.text.unicode);
            }
        }
        else if (event.type == Event::KeyPressed) {
            if (moveDone) {
                if ((event.key.code == Keyboard::W || event.key.code == Keyboard::Up   ) && snake.state != UP    && snake.state != DOWN) snake.state = UP;
                if ((event.key.code == Keyboard::S || event.key.code == Keyboard::Down ) && snake.state != UP    && snake.state != DOWN) snake.state = DOWN;
                if ((event.key.code == Keyboard::A || event.key.code == Keyboard::Left ) && snake.state != RIGHT && snake.state != LEFT) snake.state = LEFT;
                if ((event.key.code == Keyboard::D || event.key.code == Keyboard::Right) && snake.state != RIGHT && snake.state != LEFT) snake.state = RIGHT;
                moveDone = false;
            }
            if (event.key.code == Keyboard::Enter)
                appWindowSnake.displayGrid ^= true;
            if (event.key.code == Keyboard::Space && lose) {
                points = 0;
                snakeNew();
                fruit.repos();
                lose = false;
                retrieveMaxScore();
            }
            if (event.key.code == Keyboard::Escape) {
                writeScore();
                closeSnake();
            }
        }
    }
}

void renderGame() {
    if (!lose)
        updateSnake();
    drawSnake();
}

void render() {
    window->clear();
    Position mousePosition;
    mousePosition.x = Mouse::getPosition(*window).x;
    mousePosition.y = Mouse::getPosition(*window).y;

    appScreen.display();
    circuit.draw(*window, mousePosition.x, mousePosition.y);
    displayTopMenu();
    updateMenuEdge();
    updateMenuNode();
    updateFileManager();

    if (menuMouse.visibility)
        displayMouseMenu();
    if (menuEdge.visibility)
        displayMenuEdge();
    if (menuNode.visibility)
        displayMenuNode();
    if (settingsMenu.visibility) {
        settingsMenu.blur();
        if (settingsMenuSecondary.visibility)
            settingsMenuSecondary.display();
        displaySettingsMenu();
    }
    if (gameOpen)
        renderGame();
    window->display();
}

void displayGraphics() {
    openWindow();
    while (window->isOpen()) {
        pollEvents();
        render();
    }
}
