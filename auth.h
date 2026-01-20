// Group names : M.Abdullah , Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures

#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>
using namespace std;

struct Player 
{
public:
    char username[50];
    char password[50];
    char nickname[50];
    char email[50];
    char registrationTime[30];
    int playerID;
};

class PlayerRegistration
{
public:
    const char* filename = "players.txt";

    static int stringLength(const char* str) 
    {
        int len = 0;
        while (str[len] != '\0')
        {
            len++;
        }
        return len;
    }

    static void stringCopy(char* dest, const char* src)
    {
        while (*src != '\0')
        {
            *dest = *src;
            dest++;
            src++;
        }
        *dest = '\0';
    }

    static bool stringsEqual(const char* a, const char* b)
    {
        int i = 0;
        while (a[i] != '\0' && b[i] != '\0')
        {
            if (a[i] != b[i]) return false;
            i++;
        }
        return a[i] == b[i];
    }

    bool isUsernameTaken(const char* username)
    {
        std::ifstream in(filename);
        if (!in.is_open())
        {
            return false;
        }
        char buffer[300];
        while (in.getline(buffer, 300))
        {
            char storedUsername[50];
            int i = 0;
            while (buffer[i] != ',' && buffer[i] != '\0') 
            {
                storedUsername[i] = buffer[i];
                i++;
            }
            storedUsername[i] = '\0';
            if (stringsEqual(username, storedUsername)) 
            {
                in.close();
                return true;
            }
        }
        in.close();
        return false;
    }

    bool isPasswordStrong(const char* password) 
    {
        return stringLength(password) >= 6;
    }

    int generatePlayerID() 
    {
        std::ifstream in(filename);
        if (!in.is_open()) return 1;
        int count = 0;
        char ch;
        while (in.get(ch)) {
            if (ch == '\n') count++;
        }
        in.close();
        return count + 1;
    }

    void generateFakeTimestamp(char* timeBuffer)
    {
        stringCopy(timeBuffer, "2025-04-30 12:00:00");
    }

    Player registerPlayer(sf::RenderWindow& window, sf::Font& font)
    {
        sf::Texture regBackgroundTexture;
        sf::Sprite regBackgroundSprite;

        if (!regBackgroundTexture.loadFromFile("images1/reg.jpg")) 
        {
            cout << "Failed to load registration background."<<endl;
        }
        regBackgroundSprite.setTexture(regBackgroundTexture);
        regBackgroundSprite.setScale(
            float(window.getSize().x) / regBackgroundTexture.getSize().x,
            float(window.getSize().y) / regBackgroundTexture.getSize().y
        );

        sf::String inputs[4];
        std::string labels[4] = { "Username", "Password", "Nickname", "Email" };
        sf::Text inputText[4], labelText[4];
        sf::RectangleShape boxes[4];
        int currentField = 0;

        for (int i = 0; i < 4; ++i)
        {
            labelText[i].setFont(font);
            labelText[i].setCharacterSize(18);
            labelText[i].setString(labels[i] + ":");
            labelText[i].setPosition(100, 160 + i * 60);

            inputText[i].setFont(font);
            inputText[i].setCharacterSize(18);
            inputText[i].setFillColor(sf::Color::Black);
            inputText[i].setPosition(300, 160 + i * 60);

            boxes[i].setSize(sf::Vector2f(300, 30));
            boxes[i].setFillColor(sf::Color::White);
            boxes[i].setPosition(290, 160 + i * 60);
        }

        bool isRegistered = false;
        while (window.isOpen() && !isRegistered) 
        {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                {
                    return Player();
                }
                   
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) 
                {
                    return Player();
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
                            if (currentField < 3)
                            {
                                currentField++;
                            }
                            else
                            {
                                isRegistered = true;
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

            for (int i = 0; i < 4; ++i) 
            {
                sf::String display = (i == 1) ? sf::String(std::string(inputs[i].getSize(), '*')) : inputs[i];
                inputText[i].setString(display);
            }

            window.clear();
            window.draw(regBackgroundSprite);

            sf::Text title("REGISTRATION", font, 40);
            title.setOutlineColor(sf::Color::Black);
            title.setOutlineThickness(2.0f);
            title.setPosition(140, 30);
            window.draw(title);

            for (int i = 0; i < 4; ++i) 
            {
                window.draw(boxes[i]);
                window.draw(labelText[i]);
                window.draw(inputText[i]);
            }

            window.display();
        }

        Player p;
        stringCopy(p.username, inputs[0].toAnsiString().c_str());
        stringCopy(p.password, inputs[1].toAnsiString().c_str());
        stringCopy(p.nickname, inputs[2].toAnsiString().c_str());
        stringCopy(p.email, inputs[3].toAnsiString().c_str());
        generateFakeTimestamp(p.registrationTime);
        p.playerID = generatePlayerID();

        if (isUsernameTaken(p.username))
        {
            cout << "Username already exists."<<endl;
            p.username[0] = '\0';
            return p;
        }

        if (!isPasswordStrong(p.password)) 
        {
            cout << "Weak password."<<endl;
            p.username[0] = '\0';
            return p;
        }

        std::ofstream out(filename, std::ios::app);
        if (out.is_open()) 
        {
            out << p.username << "," << p.password << "," << p.nickname << "," << p.email << "," << p.registrationTime << "," << p.playerID;
            out.close();
        }
        else
        {
            cout << "Unable to open file to save registration."<<endl;
        }

        cout << "Registration successful. ID: " << p.playerID << endl;
        return p;
    }

    void displayPlayerData(sf::RenderWindow& window, sf::Font& font, const std::string& nameStr, const std::string& passStr, bool rememberChecked) 
    {
        window.clear(sf::Color(255, 230, 180));

        sf::Text title("LOGIN DETAILS", font, 30);
        title.setPosition(250, 30);

        sf::Text nameLabel("Username: " + nameStr, font, 20);
        nameLabel.setPosition(150, 100);

        sf::Text passLabel("Password: " + std::string(passStr.length(), '*'), font, 20);
        passLabel.setPosition(150, 150);

        sf::Text rememberText("Remember Me: " + std::string(rememberChecked ? "Yes" : "No"), font, 18);
        rememberText.setPosition(150, 200);

        window.draw(title);
        window.draw(nameLabel);
        window.draw(passLabel);
        window.draw(rememberText);

        window.display();
    }
};

#endif