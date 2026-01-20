// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"
#include "game.h"
#include "auth.h"
#include "login.h"
#include "profile.h"
#include "friend.h"
#include "leaderboard.h"
#include "theme.h"
#include "matchmaking.h"

using namespace std;

void handleThemeSelection(sf::RenderWindow& window, sf::Font& font, Menu& menu);
void handleGameMode(sf::RenderWindow& window, sf::Font& font, bool& reg);

int main()
{
    sf::RenderWindow window(sf::VideoMode(720, 450), "Main Menu");

    sf::Font* font = new sf::Font();
    if (!font->loadFromFile("images1/Emulogic-zrEw.ttf")) 
    {
        cout << "Error loading font" << endl;
        delete font;
        return -1;
    }


    Menu* menu = new Menu();
    PlayerRegistration* auth = new PlayerRegistration();
    PlayerLogin* login = new PlayerLogin();
    Player* loggedInPlayer = new Player();
    bool isLoggedIn = false;
    bool reg = false;


    sf::RectangleShape* registerButton = new sf::RectangleShape(sf::Vector2f(90, 20));
    registerButton->setPosition(550, 90);
    registerButton->setFillColor(sf::Color::White);

    sf::Text* registerText = new sf::Text("Register", *font, 10);
    registerText->setFillColor(sf::Color::Black);
    registerText->setPosition(555, 94);

    sf::RectangleShape* friendsButton = new sf::RectangleShape(sf::Vector2f(90, 20));
    friendsButton->setPosition(550, 130);
    friendsButton->setFillColor(sf::Color::White);

    sf::Text* friendsText = new sf::Text("Friends", *font, 10);
    friendsText->setFillColor(sf::Color::Black);
    friendsText->setPosition(555, 134);

    sf::RectangleShape* themesButton = new sf::RectangleShape(sf::Vector2f(90, 20));
    themesButton->setPosition(550, 170);
    themesButton->setFillColor(sf::Color::White);

    sf::Text* themesText = new sf::Text("Themes", *font, 10);
    themesText->setFillColor(sf::Color::Black);
    themesText->setPosition(555, 174);

    sf::RectangleShape* matchmakingButton = new sf::RectangleShape(sf::Vector2f(90, 20));
    matchmakingButton->setPosition(550, 210);
    matchmakingButton->setFillColor(sf::Color::White);

    sf::Text* matchmakingText = new sf::Text("Matches", *font, 10);
    matchmakingText->setFillColor(sf::Color::Black);
    matchmakingText->setPosition(555, 214);


    ThemeManager* themeManager = ThemeManager::getInstance();
    MatchMakingSystem* matchmaking = new MatchMakingSystem();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) 
            {
                if (event.key.code == sf::Keyboard::Up) menu->moveUp();
                else if (event.key.code == sf::Keyboard::Down) menu->moveDown();
                else if (event.key.code == sf::Keyboard::Enter) {
                    int selected = menu->getSelectedIndex();

                    if (selected == 0) {
                        handleGameMode(window, *font, reg);
                    }
                    else if (selected == 1) 
                    {
                        window.setTitle("Login Page");
                        isLoggedIn = login->loginPlayer(window, *font, *loggedInPlayer);
                        window.setTitle("Main Menu");
                    }
                    else if (selected == 2)
                    {
                        if (isLoggedIn) 
                        {
                            PlayerProfile* profile = new PlayerProfile();
                            profile->showProfile(window, *font, *loggedInPlayer);
                            delete profile;
                        }
                        else 
                        {
                            cout << "Please login first to view profile.\n";
                        }
                    }
                    else if (selected == 3)
                    {
                        Leaderboard* leaderboard = new Leaderboard();
                        leaderboard->showLeaderboard(window, *font);
                        delete leaderboard;
                    }
                    else if (selected == 4)
                    {
                        window.close();
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) 
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (registerButton->getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.setTitle("Registration Page");
                    Player registeredPlayer = auth->registerPlayer(window, *font);
                    if (registeredPlayer.username[0] != '\0') 
                    {
                        auth->displayPlayerData(window, *font, registeredPlayer.username, registeredPlayer.nickname, true);
                        reg = true;
                    }
                    else
                    {
                        cout << "Registration failed. Returning to menu.\n";
                    }
                    window.setTitle("Main Menu");
                }
                else if (friendsButton->getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (isLoggedIn) 
                    {
                        FriendSystem* friendManager = new FriendSystem();
                        friendManager->display_friend(window, *font, *loggedInPlayer, isLoggedIn);
                        delete friendManager;
                    }
                    else 
                    {
                        cout << "Please login first to view friends.\n";
                    }
                }
                else if (themesButton->getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    handleThemeSelection(window, *font, *menu);
                }
                else if (matchmakingButton->getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    matchmaking->displayMatchmaking(window, *font);
                }
            }
        }

        window.clear();
        menu->draw(window);
        window.draw(*registerButton);
        window.draw(*registerText);
        window.draw(*friendsButton);
        window.draw(*friendsText);
        window.draw(*themesButton);
        window.draw(*themesText);
        window.draw(*matchmakingButton);
        window.draw(*matchmakingText);
        window.display();
    }

    delete menu;
    delete auth;
    delete login;
    delete loggedInPlayer;
    delete registerButton;
    delete registerText;
    delete friendsButton;
    delete friendsText;
    delete themesButton;
    delete themesText;
    delete matchmakingButton;
    delete matchmakingText;
    delete matchmaking;
    delete font;

    return 0;
}

void handleGameMode(sf::RenderWindow& window, sf::Font& font, bool& reg) 
{
    window.setTitle("Player Mode");

    sf::Texture* backgroundTexture = new sf::Texture();
    if (!backgroundTexture->loadFromFile("images1/mode.jpg"))
    {
        delete backgroundTexture;
        return;
    }

    sf::Sprite* backgroundSprite = new sf::Sprite(*backgroundTexture);

    GameModeMenu* gameModeMenu = new GameModeMenu(font);
    bool selectingMode = true;

    while (selectingMode && window.isOpen())
    {
        sf::Event modeEvent;
        while (window.pollEvent(modeEvent)) 
        {
            if (modeEvent.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (modeEvent.type == sf::Event::KeyPressed)
            {
                if (modeEvent.key.code == sf::Keyboard::Up)
                {
                    gameModeMenu->moveUp();
                }
                else if (modeEvent.key.code == sf::Keyboard::Down)
                {
                    gameModeMenu->moveDown();
                }
                else if (modeEvent.key.code == sf::Keyboard::Enter) 
                {
                    int mode = gameModeMenu->getSelectedIndex();
                    selectingMode = false;
                    if (mode == 0)
                    {
                        window.setTitle("Single Player Mode");
                        rungame(window, reg);
                        reg = false;
                    }
                    else 
                    {
                        window.setTitle("Double Player Mode");
                        runtwogame(window);
                    }
                    window.setTitle("Main Menu");
                }
                else if (modeEvent.key.code == sf::Keyboard::Escape)
                {
                    selectingMode = false;
                    window.setTitle("Main Menu");
                }
            }
        }

        backgroundSprite->setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture->getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture->getSize().y
        );

        window.clear();
        window.draw(*backgroundSprite);
        gameModeMenu->draw(window);
        window.display();
    }

    delete backgroundTexture;
    delete backgroundSprite;
    delete gameModeMenu;
}

void handleThemeSelection(sf::RenderWindow& window, sf::Font& font, Menu& menu)
{
    bool themeSelectionMode = true;
    string originalTitle = "Main Menu";
    window.setTitle("Themes");

    ThemeManager* themeManager = ThemeManager::getInstance();
    AVLTree& themeTree = themeManager->getThemeTree();

    sf::Texture* themeBackgroundTexture = new sf::Texture();
    if (!themeBackgroundTexture->loadFromFile("images1/theme.jpg")) 
    {
        cout << "Failed to load theme background image." << endl;
        delete themeBackgroundTexture;
        return;
    }

    sf::Sprite* themeBackgroundSprite = new sf::Sprite(*themeBackgroundTexture);

    sf::Vector2u textureSize = themeBackgroundTexture->getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    themeBackgroundSprite->setScale(scaleX, scaleY);

    sf::Text* titleText = new sf::Text("THEMES", font, 40);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition(250, 20);

    const int MAX_THEMES = 100;
    Theme** allThemes = new Theme * [MAX_THEMES];
    int themeCount = 0;

    for (int i = 1; i <= MAX_THEMES; ++i)
    {
        Theme* theme = themeTree.searchByID(i);
        if (theme != nullptr)
        {
            allThemes[themeCount] = new Theme(*theme);
            ++themeCount;
        }
    }

    sf::Text** themeTexts = new sf::Text * [MAX_THEMES];
    for (int i = 0; i < themeCount; ++i)
    {
        themeTexts[i] = new sf::Text();
        themeTexts[i]->setFont(font);
        themeTexts[i]->setCharacterSize(25);
        themeTexts[i]->setFillColor(sf::Color::White);

        char* buffer = new char[256];
        int id = allThemes[i]->themeID;
        const char* name = allThemes[i]->themeName;

        int len = 0, temp = id;
        char* idStr = new char[10];
        do 
        {
            idStr[len++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        for (int j = 0; j < len / 2; ++j) 
        {
            char t = idStr[j];
            idStr[j] = idStr[len - j - 1];
            idStr[len - j - 1] = t;
        }
        idStr[len] = '\0';

        int idx = 0;
        while (idStr[idx] != '\0') 
        {
            buffer[idx] = idStr[idx];
            ++idx;
        }
        buffer[idx++] = '.'; buffer[idx++] = ' ';
        int j = 0;
        while (name[j] != '\0') buffer[idx++] = name[j++];
        buffer[idx] = '\0';

        themeTexts[i]->setString(buffer);
        themeTexts[i]->setPosition(50, 160 + i * 50);

        delete[] idStr;
        delete[] buffer;
    }

    int currentThemeID = themeManager->getCurrentThemeID();
    int selectedThemeIndex = -1;

    for (int i = 0; i < themeCount; ++i) 
    {
        if (allThemes[i]->themeID == currentThemeID)
        {
            selectedThemeIndex = i;
            break;
        }
    }

    if (selectedThemeIndex == -1 && themeCount > 0)
    {
        selectedThemeIndex = 0;
    }

    if (selectedThemeIndex >= 0) 
    {
        themeTexts[selectedThemeIndex]->setFillColor(sf::Color::Green);
    }

    sf::Clock* keyClock = new sf::Clock();
    const float keyDelay = 0.15f;

    while (themeSelectionMode && window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) 
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    themeSelectionMode = false;
                }
                else if (keyClock->getElapsedTime().asSeconds() >= keyDelay) 
                {
                    if (event.key.code == sf::Keyboard::Up) 
                    {
                        if (selectedThemeIndex > 0)
                        {
                            themeTexts[selectedThemeIndex]->setFillColor(sf::Color::White);
                            selectedThemeIndex--;
                            themeTexts[selectedThemeIndex]->setFillColor(sf::Color::Green);
                            keyClock->restart();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        if (selectedThemeIndex < themeCount - 1) 
                        {
                            themeTexts[selectedThemeIndex]->setFillColor(sf::Color::White);
                            selectedThemeIndex++;
                            themeTexts[selectedThemeIndex]->setFillColor(sf::Color::Green);
                            keyClock->restart();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
                    {
                        if (selectedThemeIndex >= 0) 
                        {
                            themeManager->setCurrentTheme(allThemes[selectedThemeIndex]->themeID);
                            cout << "Theme " << allThemes[selectedThemeIndex]->themeName
                                << " selected and saved.\n";

                            themeSelectionMode = false;
                            menu.refreshBackground();
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                for (int i = 0; i < themeCount; ++i)
                {
                    if (themeTexts[i]->getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (selectedThemeIndex >= 0) 
                        {
                            themeTexts[selectedThemeIndex]->setFillColor(sf::Color::White);
                        }

                        selectedThemeIndex = i;
                        themeTexts[i]->setFillColor(sf::Color::Green);

                        themeManager->setCurrentTheme(allThemes[i]->themeID);

                        cout << "Theme " << allThemes[i]->themeName << " selected and saved.\n";

                        themeSelectionMode = false;
                        menu.refreshBackground();
                        break;
                    }
                }
            }
        }

        window.clear();
        window.draw(*themeBackgroundSprite);
        window.draw(*titleText);
        for (int i = 0; i < themeCount; ++i) 
        {
            window.draw(*themeTexts[i]);
        }
        window.display();
    }

    window.setTitle(originalTitle);


    delete keyClock;

    for (int i = 0; i < themeCount; ++i) 
    {
        delete allThemes[i];
        delete themeTexts[i];
    }

    delete[] allThemes;
    delete[] themeTexts;
    delete titleText;
    delete themeBackgroundSprite;
    delete themeBackgroundTexture;
}