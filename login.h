// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef LOGIN_H
#define LOGIN_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "auth.h"

using namespace std;

void copyToCharArray(const string& src, char* dest, int maxSize)
{
    int i = 0;
    while (i < maxSize - 1 && i < src.length()) 
    {
        *(dest + i) = src[i];
        ++i;
    }
    *(dest + i) = '\0';
}

class PlayerLogin
{
public:
    bool loginPlayer(sf::RenderWindow& window, sf::Font& font, Player& loggedInPlayer)
    {
        window.setTitle("Login Page");

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("images1/login.jpg"))
        {
            cout << "Could not load login background image.\n";
        }
        sf::Sprite backgroundSprite(backgroundTexture);

        sf::Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            static_cast<float>(windowSize.x) / backgroundTexture.getSize().x,
            static_cast<float>(windowSize.y) / backgroundTexture.getSize().y
        );

        sf::String inputs[2];
        string labels[2] = { "Username", "Password" };
        sf::Text inputText[2], labelText[2];
        sf::RectangleShape boxes[2];
        int currentField = 0;

        float labelYOffset = 180;
        float inputYOffset = 180;

        for (int i = 0; i < 2; ++i) 
        {
            labelText[i].setFont(font);
            labelText[i].setCharacterSize(18);
            labelText[i].setString(labels[i] + ":");
            labelText[i].setPosition(100, labelYOffset + i * 80);
            labelText[i].setOutlineThickness(2);
            labelText[i].setOutlineColor(sf::Color::Black);

            inputText[i].setFont(font);
            inputText[i].setCharacterSize(18);
            inputText[i].setFillColor(sf::Color::Black);
            inputText[i].setPosition(300, inputYOffset + i * 80);

            boxes[i].setSize(sf::Vector2f(300, 30));
            boxes[i].setFillColor(sf::Color::White);
            boxes[i].setPosition(290, inputYOffset + i * 80);
        }

        sf::Text title("LOGIN", font, 50);
        title.setPosition(240, 50);
        title.setOutlineThickness(2);
        title.setOutlineColor(sf::Color::Black);

        bool loginAttempted = false, loginSuccess = false;

        while (window.isOpen() && !loginAttempted)
        {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                {
                    return false;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                {
                    return false;
                }

                if (event.type == sf::Event::TextEntered) 
                {
                    if (event.text.unicode == '\b') 
                    {
                        if (!inputs[currentField].isEmpty())
                            inputs[currentField].erase(inputs[currentField].getSize() - 1, 1);
                    }
                    else if (event.text.unicode == '\r')
                    {
                        if (inputs[currentField].getSize() > 0)
                        {
                            if (currentField < 1)
                            {
                                currentField++;
                            }
                            else 
                            {
                                loginAttempted = true;
                                break;
                            }
                        }
                    }
                    else if (event.text.unicode < 128) 
                    {
                        inputs[currentField] += static_cast<char>(event.text.unicode);
                    }
                }
            }

            for (int i = 0; i < 2; ++i) 
            {
                sf::String display = (i == 1) ? sf::String(string(inputs[i].getSize(), '*')) : inputs[i];
                inputText[i].setString(display);
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(title);
            for (int i = 0; i < 2; ++i) 
            {
                window.draw(boxes[i]);
                window.draw(labelText[i]);
                window.draw(inputText[i]);
            }
            window.display();
        }

        if (loginAttempted)
        {
            ifstream in("players.txt");
            if (!in.is_open())
            {
                cout << "Could not open credentials file.\n";
                return false;
            }

            string line;
            string uname = inputs[0].toAnsiString();
            string pass = inputs[1].toAnsiString();

            while (getline(in, line)) 
            {
                string storedUsername, storedPassword;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);

                storedUsername = line.substr(0, pos1);
                storedPassword = line.substr(pos1 + 1, pos2 - pos1 - 1);


                if (storedUsername == uname && storedPassword == pass)
                {
                    loginSuccess = true;
                    copyToCharArray(storedUsername, loggedInPlayer.username, sizeof(loggedInPlayer.username));
                    copyToCharArray(storedPassword, loggedInPlayer.password, sizeof(loggedInPlayer.password));
                    break;
                }
            }
            in.close();
        }

        return loginSuccess;
    }
};

#endif
