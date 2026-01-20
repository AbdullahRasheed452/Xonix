// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

struct Player_leader
{
    string name;
    int score;

    Player_leader() : name(""), score(0) {}
    Player_leader(const string& _name, int _score) : name(_name), score(_score)
    {}
};

struct PlayerDataPair
{
    string name;
    int score;

    PlayerDataPair() : name(""), score(0) {}
    PlayerDataPair(const string& n, int s) : name(n), score(s) {}
};

class Leaderboard 
{
private:
    static const int MAX_SIZE = 5;
    Player_leader minHeap[MAX_SIZE];
    int heapSize;
    sf::Font font;

    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;
            if (minHeap[index].score < minHeap[parent].score) 
            {
                swap(minHeap[index], minHeap[parent]);
                index = parent;
            }
            else
            {
                break;
            }
        }
    }

    void heapifyDown(int index) 
    {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heapSize && minHeap[left].score < minHeap[smallest].score)
        {
            smallest = left;
        }
        if (right < heapSize && minHeap[right].score < minHeap[smallest].score)
        {
            smallest = right;
        }

        if (smallest != index)
        {
            swap(minHeap[index], minHeap[smallest]);
            heapifyDown(smallest);
        }
    }

    void swap(Player_leader& a, Player_leader& b)
    {
        Player_leader temp = a;
        a = b;
        b = temp;
    }

    void splitString(const string& str, char delimiter, string* tokens, int maxTokens)
    {
        stringstream ss(str);
        string token;
        int count = 0;
        while (getline(ss, token, delimiter) && count < maxTokens) 
        {
            tokens[count++] = token;
        }
    }

    int stringToInt(const string& str)
    {
        int result = 0;
        bool negative = false;
        size_t i = 0;
        if (!str.empty() && str[0] == '-')
        {
            negative = true;
            i = 1;
        }
        for (; i < str.length(); ++i)
        {
            if (str[i] >= '0' && str[i] <= '9') 
            {
                result = result * 10 + (str[i] - '0');
            }
            else
            {
                break;
            }
        }
        return negative ? -result : result;
    }

    PlayerDataPair parsePlayerData(const string& line)
    {
        int commaCount = 0;
        for (size_t i = 0; i < line.length(); ++i) 
        {
            if (line[i] == ',') commaCount++;
        }

        if (commaCount < 1)
        {
            return PlayerDataPair("", 0);
        }

        int tokenCount = commaCount + 1;
        string* tokens = new string[tokenCount];


        for (int i = 0; i < tokenCount; ++i)
        {
            tokens[i] = "";
        }

        splitString(line, ',', tokens, tokenCount);

        string name = tokens[0];
        int score = stringToInt(tokens[tokenCount - 1]);

        delete[] tokens;
        return PlayerDataPair(name, score);
    }

    void loadPlayersFromFile(const string& filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) 
        {
            PlayerDataPair playerData = parsePlayerData(line);
            if (!playerData.name.empty())
            {
                insertPlayer(playerData.name, playerData.score);
            }
        }

        file.close();
    }

public:
    Leaderboard() : heapSize(0) 
    {

        for (int i = 0; i < MAX_SIZE; ++i) 
        {
            minHeap[i] = Player_leader();
        }

        if (!font.loadFromFile("images1/Emulogic-zrEw.ttf"))
        {
            cout << "Error loading font!" << endl;
        }
    }

    bool initialize(const string& filename) 
    {

        heapSize = 0;
        for (int i = 0; i < MAX_SIZE; ++i)
        {
            minHeap[i] = Player_leader();
        }

        loadPlayersFromFile(filename);
        return true;
    }

    void insertPlayer(const string& name, int score)
    {

        if (heapSize < MAX_SIZE)
        {
            minHeap[heapSize] = Player_leader(name, score);
            heapifyUp(heapSize);
            heapSize++;
        }

        else if (score > minHeap[0].score)
        {
            minHeap[0] = Player_leader(name, score);
            heapifyDown(0);
        }

    }

    void getTopPlayers(Player_leader* topPlayers) const
    {

        if (!topPlayers)
        {
            return;
        }

        Player_leader copy[MAX_SIZE];
        int copySize = heapSize;


        for (int i = 0; i < MAX_SIZE; ++i)
        {
            if (i < copySize) 
            {
                copy[i] = minHeap[i];
            }
            else 
            {
                copy[i] = Player_leader();
            }
        }


        for (int i = 0; i < copySize - 1; ++i) 
        {
            for (int j = i + 1; j < copySize; ++j) 
            {
                if (copy[i].score < copy[j].score) 
                {
                    Player_leader temp = copy[i];
                    copy[i] = copy[j];
                    copy[j] = temp;
                }
            }
        }


        for (int i = 0; i < MAX_SIZE; ++i) 
        {
            if (i < copySize)
            {
                topPlayers[i] = copy[i];
            }
            else 
            {
                topPlayers[i] = Player_leader();
            }
        }
    }

    void showLeaderboard(sf::RenderWindow& window, sf::Font& fontParam) 
    {

        sf::Font& usedFont = fontParam;

        if (!initialize("players.txt")) 
        {
            cout << "Failed to initialize leaderboard.\n";
            return;
        }

        window.setTitle("Leaderboard");

        Player_leader topPlayers[MAX_SIZE];
        for (int i = 0; i < MAX_SIZE; ++i) 
        {
            topPlayers[i] = Player_leader();
        }
        getTopPlayers(topPlayers);

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("images1/leaderboard.jpg")) 
        {
            cout << "Error loading background image!" << endl;
        }
        sf::Sprite backgroundSprite(backgroundTexture);

        sf::Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            static_cast<float>(windowSize.x) / backgroundTexture.getSize().x,
            static_cast<float>(windowSize.y) / backgroundTexture.getSize().y
        );

        sf::Text title;
        title.setFont(usedFont);
        title.setString("Top 5 Players");
        title.setCharacterSize(33);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(170, 30);

        sf::Text playerText[MAX_SIZE];
        for (int i = 0; i < MAX_SIZE; ++i) 
        {
            playerText[i].setFont(usedFont);
            playerText[i].setCharacterSize(24);
            playerText[i].setFillColor(sf::Color::White);
            playerText[i].setPosition(210, 130 + i * 50);

            if (!topPlayers[i].name.empty())
            {
                playerText[i].setString(to_string(i + 1) + ". " + topPlayers[i].name + " - " + to_string(topPlayers[i].score));
            }
            else 
            {
                playerText[i].setString(to_string(i + 1) + ". ---");
            }
        }

        bool inLeaderboard = true;
        while (window.isOpen() && inLeaderboard) 
        {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::KeyPressed) 
                {
                    if (event.key.code == sf::Keyboard::Escape) 
                    {
                        window.clear();
                        window.setTitle("Main Menu");
                        window.display();
                        inLeaderboard = false;
                    }
                }
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(title);
            for (int i = 0; i < MAX_SIZE; ++i) 
            {
                window.draw(playerText[i]);
            }
            window.display();
        }
    }
};

#endif