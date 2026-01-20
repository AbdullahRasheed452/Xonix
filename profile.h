// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef PROFILE_H
#define PROFILE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "auth.h"
using namespace std;

class StringArray 
{
private:
    string* data;
    int capacity;
    int size;


    void copyStrings(string* dest, const string* src, int count) 
    {
        for (int i = 0; i < count; i++)
        {
            dest[i] = src[i];
        }
    }

public:
    StringArray() : data(nullptr), capacity(0), size(0) {}


    StringArray(const StringArray& other) : data(nullptr), capacity(0), size(0) 
    {
        if (other.size > 0) 
        {
            data = new string[other.capacity];
            capacity = other.capacity;
            size = other.size;
            copyStrings(data, other.data, size);
        }
    }

    ~StringArray() 
    {
        delete[] data;
    }


    StringArray& operator=(const StringArray& other) 
    {
        if (this != &other) 
        {
            delete[] data;
            data = nullptr;

            if (other.size > 0) 
            {
                data = new string[other.capacity];
                capacity = other.capacity;
                size = other.size;
                copyStrings(data, other.data, size);
            }
            else 
            {
                capacity = 0;
                size = 0;
            }
        }
        return *this;
    }

    void push_back(const string& value)
    {
        if (size >= capacity) 
        {
            int newCapacity = capacity == 0 ? 1 : capacity * 2;
            string* newData = new string[newCapacity];

            if (data != nullptr) 
            {
                copyStrings(newData, data, size);
                delete[] data;
            }

            data = newData;
            capacity = newCapacity;
        }

        if (size < capacity && data != nullptr) 
        {
            data[size] = value;
            size++;
        }
    }

    string& operator[](int index) 
    {
        if (index >= 0 && index < size && data != nullptr) 
        {
            return data[index];
        }
        static string empty_string;
        return empty_string;
    }

    const string& operator[](int index) const 
    {
        if (index >= 0 && index < size && data != nullptr)
        {
            return data[index];
        }
        static string empty_string;
        return empty_string;
    }

    int getSize() const 
    {
        return size;
    }

    bool empty() const 
    {
        return size == 0;
    }
};

class PlayerProfile
{
private:
    int getPlayerScore(const string& username) 
    {
        ifstream file("players.txt");
        string line;

        while (getline(file, line)) 
        {
            stringstream ss(line);
            string name, password, extra1, extra2, dateTime;
            int gamesPlayed, score;

            getline(ss, name, ',');
            getline(ss, password, ',');
            getline(ss, extra1, ',');
            getline(ss, extra2, ',');
            getline(ss, dateTime, ',');
            ss >> gamesPlayed;
            ss.ignore();
            ss >> score;

            if (name == username) 
            {
                return score;
            }
        }
        return -1;
    }

    StringArray getFriends(const string& username) 
    {
        ifstream file("friends_data.txt");
        string line;
        StringArray friends;

        while (getline(file, line)) 
        {
            stringstream ss(line);
            string firstPlayer, secondPlayer;

            getline(ss, firstPlayer, ':');
            getline(ss, secondPlayer);

            if (firstPlayer == username) {
                friends.push_back(secondPlayer);
            }
        }

        return friends;
    }

public:
    void showProfile(sf::RenderWindow& window, sf::Font& font, const Player& player)
    {
        window.setTitle("Profile");

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("images1/profile.jpg")) {
            cout << "Failed to load profile background image.\n";
        }

        sf::Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            float(window.getSize().x) / backgroundTexture.getSize().x,
            float(window.getSize().y) / backgroundTexture.getSize().y
        );

        sf::Text title("Player Profile", font, 35);
        title.setPosition(115, 55);
        title.setFillColor(sf::Color::White);

        sf::Text usernameLabel("Username: ", font, 20);
        usernameLabel.setPosition(100, 150);
        usernameLabel.setFillColor(sf::Color::White);

        sf::Text usernameValue(player.username, font, 20);
        usernameValue.setPosition(300, 150);
        usernameValue.setFillColor(sf::Color::Yellow);

        sf::Text passwordLabel("Password: ", font, 20);
        passwordLabel.setPosition(100, 200);
        passwordLabel.setFillColor(sf::Color::White);

        sf::Text passwordValue(player.password, font, 20);
        passwordValue.setPosition(300, 200);
        passwordValue.setFillColor(sf::Color::Yellow);

        int score = getPlayerScore(player.username);

        sf::Text scoreLabel("Score: ", font, 20);
        scoreLabel.setPosition(100, 250);
        scoreLabel.setFillColor(sf::Color::White);

        sf::Text scoreValue("N/A", font, 20);
        scoreValue.setPosition(300, 250);
        scoreValue.setFillColor(sf::Color::Yellow);

        if (score != -1) {
            scoreValue.setString(to_string(score));
        }
        else {
            scoreValue.setString("Not Found");
        }

        StringArray friends = getFriends(player.username);

        sf::Text friendsLabel("Friends: ", font, 20);
        friendsLabel.setPosition(100, 300);
        friendsLabel.setFillColor(sf::Color::White);


        int friendCount = friends.getSize();
        sf::Text* friendTexts = nullptr;
        if (friendCount > 0) 
        {
            friendTexts = new sf::Text[friendCount];


            for (int i = 0; i < friendCount; i++)
            {

                friendTexts[i] = sf::Text(friends[i], font, 20);
                friendTexts[i].setPosition(300, 330 + i * 30 - 30);
                friendTexts[i].setFillColor(sf::Color::Yellow);
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(usernameLabel);
        window.draw(usernameValue);
        window.draw(passwordLabel);
        window.draw(passwordValue);
        window.draw(scoreLabel);
        window.draw(scoreValue);
        window.draw(friendsLabel);


        if (friendTexts != nullptr) 
        {
            for (int i = 0; i < friendCount; i++) {
                window.draw(friendTexts[i]);
            }
        }

        window.display();

        if (friendTexts != nullptr) 
        {
            delete[] friendTexts;
            friendTexts = nullptr;
        }

        while (window.isOpen()) 
        {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                {
                    window.clear();
                    window.setTitle("Main Menu");
                    window.display();
                    return;
                }
            }
        }
    }
};

#endif