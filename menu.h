// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "themes_manager.h"
using namespace std;
using namespace sf;

class Menu 
{
private:
    int selectedIndex;
    Font font;
    Text menuItems[5];
    Texture backgroundTexture;
    Sprite backgroundSprite;

    Text titleText;
    Text message;
    bool showingMessage = false;
    Clock messageTimer;
    Clock blinkClock;

public:
    Menu(float width = 720, float height = 450) : selectedIndex(0)
    {
        string items[5] = 
        {
            "Start Game",
            "Login",
            "Player Profile",
            "Leaderboard",
            "Exit"
        };

        if (!font.loadFromFile("images1/Emulogic-zrEw.ttf")) 
        {
            cout << "Error loading font" << endl;
        }

        ThemeManager* themeManager = ThemeManager::getInstance();

        if (!backgroundTexture.loadFromFile("images1/bg.jpg"))
        {
            cout << "Error loading background texture" << endl;
        }

        backgroundSprite.setTexture(backgroundTexture);
        Vector2u textureSize = backgroundTexture.getSize();

        float backgroundHeight = 180.0f + 6.5 * 45.0f;
        backgroundSprite.setScale(
            width / static_cast<float>(textureSize.x),
            backgroundHeight / static_cast<float>(textureSize.y)
        );

        float xPosition = 270.0f;
        float yStartPosition = 180.0f;
        float ySpacing = 45.0f;

        for (int i = 0; i < 5; ++i)
        {
            menuItems[i].setFont(font);
            menuItems[i].setString(items[i]);
            menuItems[i].setCharacterSize(15);
            menuItems[i].setFillColor(Color::White);
            menuItems[i].setPosition(Vector2f(xPosition, yStartPosition + i * ySpacing));
        }

        menuItems[0].setFillColor(Color::Red);

        titleText.setFont(font);
        titleText.setCharacterSize(40);
        titleText.setFillColor(Color::White);
        titleText.setOutlineColor(Color::Black);
        titleText.setOutlineThickness(2.0f);
        titleText.setString("Game Menu");
        titleText.setPosition(Vector2f(170.0f, 100.0f));

        message.setFont(font);
        message.setCharacterSize(24);
        message.setFillColor(Color::Yellow);
        message.setPosition(Vector2f(50, 460));
    }

    void refreshBackground() 
    {
        ThemeManager* themeManager = ThemeManager::getInstance();

        if (!backgroundTexture.loadFromFile("images1/bg.jpg")) 
        {
            cout << "Error loading background texture" << endl;
        }

        backgroundSprite.setTexture(backgroundTexture);
    }

    void draw(RenderWindow& window)
    {
        window.draw(backgroundSprite);
        window.draw(titleText);

        for (int i = 0; i < 5; ++i)
        {
            if (i == selectedIndex) 
            {
                float time = blinkClock.getElapsedTime().asSeconds();
                if (static_cast<int>(time / 0.3f) % 2 == 0)
                {
                    window.draw(menuItems[i]);
                }
            }
            else 
            {
                window.draw(menuItems[i]);
            }
        }

        if (showingMessage && messageTimer.getElapsedTime().asSeconds() < 3)
        {
            window.draw(message);
        }
    }

    void moveUp() 
    {
        if (selectedIndex - 1 >= 0)
        {
            menuItems[selectedIndex].setFillColor(Color::White);
            selectedIndex--;
            menuItems[selectedIndex].setFillColor(Color::Red);
            blinkClock.restart();
        }
    }

    void moveDown() 
    {
        if (selectedIndex + 1 < 5) 
        {
            menuItems[selectedIndex].setFillColor(Color::White);
            selectedIndex++;
            menuItems[selectedIndex].setFillColor(Color::Red);
            blinkClock.restart();
        }
    }

    int getSelectedIndex() const 
    {
        return selectedIndex;
    }

    void saveGame() 
    {
        message.setString("Game Saved!");
        showingMessage = true;
        messageTimer.restart();
    }
};

class GameModeMenu 
{
private:
    Font& font;
    Text modeItems[2];
    int selectedIndex;
    Clock blinkClock;
    Text titleedText;

public:
    GameModeMenu(Font& f) : font(f), selectedIndex(0) 
    {
        string modes[2] = { "Single Player", "Double Player" };
        for (int i = 0; i < 2; ++i) 
        {
            modeItems[i].setFont(font);
            modeItems[i].setString(modes[i]);
            modeItems[i].setCharacterSize(15);
            modeItems[i].setFillColor(i == 0 ? Color::Red : Color::White);

            modeItems[i].setPosition(250.0f, 180.0f + i * 50.0f);
        }

        titleedText.setFont(font);
        titleedText.setCharacterSize(40);
        titleedText.setFillColor(Color::Black);
        titleedText.setOutlineColor(Color::White);
        titleedText.setOutlineThickness(2.0f);
        titleedText.setString("Player mode");
        titleedText.setPosition(Vector2f(150.0f, 50.0f));
    }

    void draw(RenderWindow& window)
    {
        window.draw(titleedText);

        for (int i = 0; i < 2; ++i)
        {
            if (i == selectedIndex)
            {
                float time = blinkClock.getElapsedTime().asSeconds();
                if (static_cast<int>(time / 0.3f) % 2 == 0)
                {
                    window.draw(modeItems[i]);
                }
            }
            else 
            {
                window.draw(modeItems[i]);
            }
        }
    }

    void moveUp() 
    {
        if (selectedIndex > 0) 
        {
            modeItems[selectedIndex].setFillColor(Color::White);
            selectedIndex--;
            modeItems[selectedIndex].setFillColor(Color::Red);
            blinkClock.restart();
        }
    }

    void moveDown() 
    {
        if (selectedIndex < 1)
        {
            modeItems[selectedIndex].setFillColor(Color::White);
            selectedIndex++;
            modeItems[selectedIndex].setFillColor(Color::Red);
            blinkClock.restart();
        }
    }

    int getSelectedIndex() const 
    {
        return selectedIndex;
    }
};

#endif 