// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <time.h>
#include <sstream>
#include <fstream> 
#include <iostream>
#include <iomanip>
#include <string>
#include "gamestate.h"

using namespace sf;
using namespace std;

const int M = 25;
const int N = 40;

int grid[M][N] = { 0 };
int ts = 18;

struct Enemy
{
    int x, y, dx, dy;

    Enemy()
    {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
    }

    void move()
    {
        x += dx;
        if (grid[y / ts][x / ts] == 1)
        {
            dx = -dx;
            x += dx;
        }
        y += dy;
        if (grid[y / ts][x / ts] == 1)
        {
            dy = -dy;
            y += dy;
        }
    }
};

void drop(int y, int x)
{
    if (grid[y][x] == 0)
    {
        grid[y][x] = -1;
    }
    if (grid[y - 1][x] == 0)
    {
        drop(y - 1, x);
    }
    if (grid[y + 1][x] == 0)
    {
        drop(y + 1, x);
    }
    if (grid[y][x - 1] == 0)
    {
        drop(y, x - 1);
    }
    if (grid[y][x + 1] == 0)
    {
        drop(y, x + 1);
    }
}

void ensureSavesDirectoryExists()
{
    system("mkdir saves");
}

string getUserInput(RenderWindow& window, Font& font, const string& prompt)
{
    string input;
    bool inputting = true;

    Text promptText(prompt, font, 20);
    promptText.setPosition(90, 200);
    promptText.setFillColor(Color::White);

    Text inputText("", font, 20);
    inputText.setPosition(430, 200);
    inputText.setFillColor(Color::White);

    sf::Texture saveLoadBackgroundTexture;
    if (!saveLoadBackgroundTexture.loadFromFile("images1/mode.jpg"))
    {
        cout << "Failed to load save/load background texture!" << endl;
    }
    sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

    sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    saveLoadBackgroundSprite.setScale(
        windowSize.x / static_cast<float>(saveLoadTextureSize.x),
        windowSize.y / static_cast<float>(saveLoadTextureSize.y)
    );

    while (inputting && window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::TextEntered)
            {
                if (event.text.unicode == 8)
                {
                    if (!input.empty())
                    {
                        input.pop_back();
                    }
                }
                else if (event.text.unicode == 13)
                {
                    inputting = false;
                }
                else if (event.text.unicode >= 32 && event.text.unicode < 128)
                {
                    input += static_cast<char>(event.text.unicode);
                }
            }
        }

        inputText.setString(input);

        window.clear();
        window.draw(saveLoadBackgroundSprite);
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }

    return input;
}

int getUserChoice(RenderWindow& window, Font& font, const string& prompt,
    const string options[], int optionCount)
{
    int selectedOption = 0;
    bool selecting = true;
    Text promptText(prompt, font, 30);
    promptText.setPosition(110, 90);
    sf::Texture saveLoadBackgroundTexture;
    if (!saveLoadBackgroundTexture.loadFromFile("images1/mode.jpg"))
    {
        cout << "Failed to load save/load background texture!" << endl;
    }
    sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);
    sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    saveLoadBackgroundSprite.setScale(
        windowSize.x / static_cast<float>(saveLoadTextureSize.x),
        windowSize.y / static_cast<float>(saveLoadTextureSize.y)
    );


    Text* optionTexts = new Text[optionCount];
    for (int i = 0; i < optionCount; i++)
    {
        optionTexts[i] = Text(options[i], font, 18);
        optionTexts[i].setPosition(180, 200 + i * 40);
    }

    while (selecting && window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    selectedOption = max(0, selectedOption - 1);
                }
                else if (event.key.code == Keyboard::Down)
                {
                    selectedOption = min(optionCount - 1, selectedOption + 1);
                }
                else if (event.key.code == Keyboard::Return)
                {
                    selecting = false;
                }
            }
        }

        window.clear();
        window.draw(saveLoadBackgroundSprite);
        window.draw(promptText);

        for (int i = 0; i < optionCount; i++)
        {
            if (i == selectedOption)
            {
                optionTexts[i].setFillColor(Color::Yellow);
                optionTexts[i].setString("> " + options[i]);
            }
            else
            {
                optionTexts[i].setFillColor(Color::White);
                optionTexts[i].setString("  " + options[i]);
            }
            window.draw(optionTexts[i]);
        }

        window.display();
    }


    delete[] optionTexts;

    return selectedOption;
}

bool isValidIntegerInput(const string& input)
{
    if (input.empty())
    {
        return false;
    }

    for (char c : input)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }

    return true;
}

void saveGridToGameState(GameState& state, int x, int y)
{
    state.clearTiles();

    state.playerX = x;
    state.playerY = y;

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] != 0)
            {
                state.addTile(j, i);
            }
        }
    }
}

void loadGridFromGameState(const GameState& state, int& x, int& y)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = 0;
        }
    }

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
            {
                grid[i][j] = 1;
            }
        }
    }

    x = state.playerX;
    y = state.playerY;

    TileNode* curr = state.head;
    while (curr)
    {
        grid[curr->y][curr->x] = 1;
        curr = curr->next;
    }
}
void rungame(RenderWindow& window, bool regis)
{

    ThemeManager* themeManager = ThemeManager::getInstance();

    sf::Texture gameBackgroundTexture;
    if (!gameBackgroundTexture.loadFromFile(themeManager->getCurrentBackgroundPath()))
    {
        window.clear();
    }

    sf::Sprite gameBackgroundSprite(gameBackgroundTexture);

    sf::Vector2u textureSize = gameBackgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    gameBackgroundSprite.setScale(
        windowSize.x / static_cast<float>(textureSize.x),
        windowSize.y / static_cast<float>(textureSize.y)
    );

    ensureSavesDirectoryExists();

    srand(time(0));
    window.setFramerateLimit(60);

    Texture tTile, tGameover, tEnemy, tActiveTile, tHero;
    tTile.loadFromFile("images1/tile.png");
    tGameover.loadFromFile("images1/gameover.png");
    tEnemy.loadFromFile("images1/enemy.png");
    tActiveTile.loadFromFile("images1/active_tile.png");
    tHero.loadFromFile("images1/hero.png");

    Sprite sTile(tTile), sGameover(tGameover), sEnemy(tEnemy), sHero(tHero);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);
    sHero.setOrigin(0, 0);

    int enemyCount = 4;
    Enemy a[10];

    bool Game = true;
    bool gameOverScoreSaved = false;
    int x = 0, y = 0, dx = 0, dy = 0;
    float timer = 0, delay = 0.07;
    Clock clock;

    bool activeTiles = false;

    int score = 0;
    int tilesCapturedThisMove = 0;
    bool capturing = false;

    int availablePowerups = 0;
    bool powerupActive = false;
    float powerupTimer = 0.0f;
    float powerupDuration = 3.0f;
    int nextPowerupScore = 50;

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
        }
    }

    window.clear();
    Font font;
    if (!font.loadFromFile("images1/Emulogic-zrEw.ttf"))
    {
        cout << "Error loading font!" << endl;
        return;
    }

    GameState gameState;


    const string options[] = { "New Game", "Load Existing Game" };

    const int optionCount = 2;

    int choice = getUserChoice(window, font, "Select an option:", options, optionCount);

    string saveId = "";

    if (choice == 1)
    {
        sf::Texture saveLoadBackgroundTexture;
        if (!saveLoadBackgroundTexture.loadFromFile("images1/mode.jpg"))
        {
            cout << "Failed to load save/load background texture!" << endl;
        }
        sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

        sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
        sf::Vector2u windowSize = window.getSize();
        saveLoadBackgroundSprite.setScale(
            windowSize.x / static_cast<float>(saveLoadTextureSize.x),
            windowSize.y / static_cast<float>(saveLoadTextureSize.y)
        );

        bool validId = false;
        while (!validId && window.isOpen())
        {
            saveId = getUserInput(window, font, "Enter save ID:");

            if (isValidIntegerInput(saveId))
            {
                strcpy_s(gameState.saveId, sizeof(gameState.saveId), saveId.c_str());

                if (gameState.loadFromFile(saveId.c_str()))
                {
                    cout << "Game loaded successfully from save ID: " << saveId << endl;
                    loadGridFromGameState(gameState, x, y);

                    if (gameState.availablePowerups >= 0)
                    {
                        availablePowerups = gameState.availablePowerups;
                        score = gameState.score;
                        nextPowerupScore = 50;
                        while (nextPowerupScore <= score)
                        {
                            nextPowerupScore += (nextPowerupScore < 100) ? 20 : 30;
                        }
                    }

                    validId = true;
                }
                else
                {
                    Text errorText("Save ID not found.Try again.", font, 20);
                    errorText.setFillColor(Color::Red);
                    errorText.setPosition(80, 210);

                    window.clear();
                    window.draw(saveLoadBackgroundSprite);
                    window.draw(errorText);
                    window.display();
                    sleep(milliseconds(2000));
                }
            }
            else
            {
                Text errorText("Please enter a valid integer ID.", font, 20);
                errorText.setFillColor(Color::Red);
                errorText.setPosition(40, 180);

                window.clear();
                window.draw(saveLoadBackgroundSprite);
                window.draw(errorText);
                window.display();
                sleep(milliseconds(2000));
            }
        }
    }
    else {
        sf::Texture saveLoadBackgroundTexture;
        if (!saveLoadBackgroundTexture.loadFromFile("images1/mode.jpg"))
        {
            cout << "Failed to load save/load background texture!" << endl;
        }
        sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

        sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
        sf::Vector2u windowSize = window.getSize();
        saveLoadBackgroundSprite.setScale(
            windowSize.x / static_cast<float>(saveLoadTextureSize.x),
            windowSize.y / static_cast<float>(saveLoadTextureSize.y)
        );

        x = 10; y = 0;

        bool validId = false;
        while (!validId && window.isOpen())
        {
            saveId = getUserInput(window, font, "Enter a Save ID:");

            if (isValidIntegerInput(saveId))
            {
                if (gameState.loadFromFile(saveId.c_str()))
                {
                    Text errorText("Save ID already exists.\n\n Try a different ID.", font, 20);
                    errorText.setFillColor(Color::Red);
                    errorText.setPosition(130, 180);

                    window.clear();
                    window.draw(saveLoadBackgroundSprite);
                    window.draw(errorText);
                    window.display();
                    sleep(milliseconds(2000));
                }
                else
                {
                    validId = true;
                }
            }
            else {
                Text errorText("Please enter a valid integer ID.", font, 20);
                errorText.setFillColor(Color::Red);
                errorText.setPosition(50, 230);

                window.clear();
                window.draw(saveLoadBackgroundSprite);
                window.draw(errorText);
                window.display();
                sleep(milliseconds(2000));
            }
        }

        strcpy_s(gameState.saveId, sizeof(gameState.saveId), saveId.c_str());
        cout << "New game started with save ID: " << saveId << endl;

        availablePowerups = 0;
        gameState.availablePowerups = 0;
        gameState.score = 0;
    }

    sf::RectangleShape powerupButton(sf::Vector2f(160, 30));
    powerupButton.setPosition(40, 25);
    powerupButton.setFillColor(sf::Color::Cyan);

    Text powerupButtonText("*Power_up*", font, 14);
    powerupButtonText.setFillColor(sf::Color::Black);
    powerupButtonText.setPosition(50, 30);

    Text scoreText("", font, 20);
    scoreText.setPosition(510, 30);

    Text powerupText("", font, 18);
    powerupText.setPosition(250, 30);
    powerupText.setFillColor(sf::Color::Yellow);

    Text saveIdText("", font, 20);
    saveIdText.setPosition(250, 500);
    saveIdText.setString("Save_ID:" + string(gameState.saveId));
    saveIdText.setFillColor(sf::Color::White);

    auto saveScoreToFile = [&](int currentScore)
        {
            if (regis && !gameOverScoreSaved && currentScore > 0)
            {
                ofstream player_File("players.txt", ios::app);
                if (player_File.is_open())
                {
                    player_File << "," << currentScore << "\n";
                    player_File.close();
                    gameOverScoreSaved = true;
                    cout << "Score " << currentScore << " saved to players.txt" << endl;
                }
                else
                {
                    cout << "Unable to open players.txt file!" << endl;
                }
            }
        };

    while (window.isOpen())
    {
        window.clear();
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        if (powerupActive)
        {
            powerupTimer += time;
            if (powerupTimer >= powerupDuration)
            {
                powerupActive = false;
                powerupTimer = 0.0f;
            }
        }

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                if (Game && regis)
                {
                    saveScoreToFile(score);
                }

                saveGridToGameState(gameState, x, y);
                gameState.score = score;
                gameState.availablePowerups = availablePowerups;
                gameState.saveToFile();
                cout << "Game saved with ID: " << gameState.saveId << endl;
                window.close();
            }

            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            {
                if (Game && score > 0 && regis)
                {
                    saveScoreToFile(score);
                }
                sf::Texture saveLoadBackgroundTexture;
                if (!saveLoadBackgroundTexture.loadFromFile("images1/gameov.jpg"))
                {
                    cout << "Failed to load save/load background texture!" << endl;
                }
                sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

                sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
                sf::Vector2u windowSize = window.getSize();
                saveLoadBackgroundSprite.setScale(
                    windowSize.x / static_cast<float>(saveLoadTextureSize.x),
                    windowSize.y / static_cast<float>(saveLoadTextureSize.y)
                );

                saveGridToGameState(gameState, x, y);
                gameState.score = score;
                gameState.availablePowerups = availablePowerups;
                gameState.saveToFile();
                cout << "Game ended with ESC key. Score: " << score << endl;

                window.clear();
                window.draw(saveLoadBackgroundSprite);

                Text exitText("GAME ENDED", font, 40);
                exitText.setFillColor(Color::Yellow);
                exitText.setPosition(180, 170);

                stringstream finalScoreStream;
                finalScoreStream << "Final Score: " << score;
                Text finalScoreText(finalScoreStream.str(), font, 25);
                finalScoreText.setFillColor(Color::White);
                finalScoreText.setPosition(180, 230);

                Text returnText("Returning to the menu..", font, 20);
                returnText.setFillColor(Color::White);
                returnText.setPosition(180, 280);

                window.draw(exitText);
                window.draw(finalScoreText);
                window.draw(returnText);
                window.display();

                sleep(milliseconds(2000));

                return;
            }

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (Game &&
                    availablePowerups > 0 &&
                    !powerupActive &&
                    powerupButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    powerupActive = true;
                    powerupTimer = 0.0f;
                    availablePowerups--;
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            dx = -1;
            dy = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            dx = 1;
            dy = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            dx = 0;
            dy = -1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            dx = 0;
            dy = 1;
        }

        if (!Game && !gameOverScoreSaved && regis)
        {
            saveScoreToFile(score);
        }

        if (!Game)
        {
            sf::Texture saveLoadBackgroundTexture;
            if (!saveLoadBackgroundTexture.loadFromFile("images1/gameov.jpg"))
            {
                cout << "Failed to load save/load background texture!" << endl;
            }
            sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

            sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
            sf::Vector2u windowSize = window.getSize();
            saveLoadBackgroundSprite.setScale(
                windowSize.x / static_cast<float>(saveLoadTextureSize.x),
                windowSize.y / static_cast<float>(saveLoadTextureSize.y)
            );



            Text gameOverText("GAME OVER", font, 40);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(180, 170);

            stringstream finalScoreStream;
            finalScoreStream << "Final Score: " << score;
            Text finalScoreText(finalScoreStream.str(), font, 25);
            finalScoreText.setFillColor(Color::Yellow);
            finalScoreText.setPosition(180, 230);

            window.draw(saveLoadBackgroundSprite);

            window.draw(gameOverText);
            window.draw(finalScoreText);
            window.draw(saveIdText);

            window.display();
            sleep(milliseconds(3000));
            return;
        }

        if (timer > delay)
        {
            x += dx; y += dy;
            if (x < 0)
            {
                x = 0;
            }
            if (x >= N)
            {
                x = N - 1;
            }
            if (y < 0)
            {
                y = 0;
            }
            if (y >= M)
            {
                y = M - 1;
            }

            if (grid[y][x] == 2)
            {
                Game = false;
            }

            if (grid[y][x] == 0)
            {
                grid[y][x] = 2;
                tilesCapturedThisMove++;
                capturing = true;
                if (!activeTiles)
                {
                    sTile.setTexture(tActiveTile);
                    activeTiles = true;
                }
            }

            if (capturing && grid[y][x] == 1)
            {
                dx = dy = 0;

                for (int i = 0; i < enemyCount; i++)
                {
                    drop(a[i].y / ts, a[i].x / ts);
                }


                int filledTiles = 0;

                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }

                        else if (grid[i][j] == 0 || grid[i][j] == 2)
                        {
                            grid[i][j] = 1;
                            filledTiles++;
                        }
                    }
                }

                int multiplier = (filledTiles > 10) ? 2 : 1;
                int scoreIncrease = filledTiles * multiplier;
                int oldScore = score;
                score += scoreIncrease;

                while (oldScore < nextPowerupScore && score >= nextPowerupScore)
                {
                    availablePowerups++;

                    if (nextPowerupScore < 100)
                    {
                        nextPowerupScore += 20;
                    }
                    else
                    {
                        nextPowerupScore += 30;
                    }
                }

                capturing = false;
                tilesCapturedThisMove = 0;
            }

            timer = 0;
        }

        if (!powerupActive)
        {
            for (int i = 0; i < enemyCount; i++) a[i].move();
        }

        for (int i = 0; i < enemyCount; i++)
        {
            if (grid[a[i].y / ts][a[i].x / ts] == 2)
            {
                Game = false;
            }

        }


        window.draw(gameBackgroundSprite);

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                {
                    continue;
                }
                if (grid[i][j] == 1)
                {
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                }
                if (grid[i][j] == 2)
                {
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                }
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }
        }

        sHero.setPosition(x * ts, y * ts);
        window.draw(sHero);

        if (!powerupActive)
        {
            sEnemy.rotate(10);
        }

        for (int i = 0; i < enemyCount; i++)
        {
            if (powerupActive)
            {
                sEnemy.setColor(sf::Color(100, 100, 255, 200));
            }
            else
            {
                sEnemy.setColor(sf::Color::White);
            }
            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        stringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());
        window.draw(scoreText);

        stringstream pss;
        pss << "Power-ups: " << availablePowerups;
        powerupText.setString(pss.str());
        window.draw(powerupText);

        if (availablePowerups > 0 && !powerupActive)
        {
            powerupButton.setFillColor(sf::Color::Cyan);
            powerupButtonText.setFillColor(sf::Color::Black);
        }
        else
        {
            powerupButton.setFillColor(sf::Color(100, 100, 100, 150));
            powerupButtonText.setFillColor(sf::Color(200, 200, 200, 200));
        }

        window.draw(powerupButton);
        window.draw(powerupButtonText);

        if (powerupActive)
        {
            stringstream tss;
            tss << "Freeze: " << fixed << setprecision(1) << (powerupDuration - powerupTimer) << "s";
            Text freezeText(tss.str(), font, 16);
            freezeText.setFillColor(sf::Color::Cyan);
            freezeText.setPosition(480, 270);
            window.draw(freezeText);

            sf::RectangleShape freezeIndicator(sf::Vector2f(window.getSize().x - 10, window.getSize().y - 10));
            freezeIndicator.setPosition(5, 5);
            freezeIndicator.setFillColor(sf::Color::Transparent);
            freezeIndicator.setOutlineThickness(3);
            int pulseAlpha = 100 + static_cast<int>(85 * sin(powerupTimer * 5));
            freezeIndicator.setOutlineColor(sf::Color(100, 200, 255, pulseAlpha));
            window.draw(freezeIndicator);
        }

        window.draw(saveIdText);

        window.display();
    }

    if (Game && regis && !gameOverScoreSaved && score > 0)
    {
        ofstream player_File("players.txt", ios::app);
        if (player_File.is_open())
        {
            player_File << "," << score << "\n";
            player_File.close();
            cout << "Score " << score << " saved to players.txt before exiting" << endl;
        }
    }

    saveGridToGameState(gameState, x, y);
    gameState.score = score;
    gameState.availablePowerups = availablePowerups;
    gameState.saveToFile();
    cout << "Game saved with ID: " << gameState.saveId << " before exiting" << endl;
}
void runtwogame(RenderWindow& window)
{
    ThemeManager* themeManager = ThemeManager::getInstance();

    sf::Texture gameBackgroundTexture;
    if (!gameBackgroundTexture.loadFromFile(themeManager->getCurrentBackgroundPath()))
    {
        window.clear();
    }

    sf::Sprite gameBackgroundSprite(gameBackgroundTexture);

    sf::Vector2u textureSize = gameBackgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    gameBackgroundSprite.setScale(
        windowSize.x / static_cast<float>(textureSize.x),
        windowSize.y / static_cast<float>(textureSize.y)
    );

    srand(time(0));
    window.setFramerateLimit(60);

    Texture tTile, tGameover, tEnemy, tActiveTile, tHero1, tHero2;
    tTile.loadFromFile("images1/tile.png");
    tGameover.loadFromFile("images1/gameover.png");
    tEnemy.loadFromFile("images1/enemy.png");
    tActiveTile.loadFromFile("images1/active_tile.png");
    tHero1.loadFromFile("images1/hero.png");
    tHero2.loadFromFile("images1/hero.png");

    Sprite sTile(tTile), sGameover(tGameover), sEnemy(tEnemy), sHero1(tHero1), sHero2(tHero2);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);
    sHero1.setOrigin(0, 0);
    sHero2.setOrigin(0, 0);

    int enemyCount = 4;
    Enemy a[10];

    bool Game = true;

    int x1 = 5, y1 = 0, dx1 = 0, dy1 = 0;
    int score1 = 0;
    int tilesCapturedThisMove1 = 0;
    bool capturing1 = false;
    bool activeTiles1 = false;

    int availablePowerups1 = 0;
    bool powerupActive1 = false;
    float powerupTimer1 = 0.0f;
    float powerupDuration = 3.0f;
    int nextPowerupScore1 = 50;

    int x2 = 35, y2 = 0, dx2 = 0, dy2 = 0;
    int score2 = 0;
    int tilesCapturedThisMove2 = 0;
    bool capturing2 = false;
    bool activeTiles2 = false;

    int availablePowerups2 = 0;
    bool powerupActive2 = false;
    float powerupTimer2 = 0.0f;
    int nextPowerupScore2 = 50;

    float timer = 0, delay = 0.07;
    Clock clock;

    const float GAME_TIME = 120.0f;
    float gameTimer = GAME_TIME;
    Clock gameTimerClock;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

    window.clear();
    Font font;
    if (!font.loadFromFile("images1/Emulogic-zrEw.ttf"))
    {
        cout << "Error loading font!" << endl;
        return;
    }

    Text score1Text("", font, 20);
    score1Text.setPosition(100, 20);
    score1Text.setFillColor(Color::Blue);

    Text score2Text("", font, 20);
    score2Text.setPosition(550, 20);
    score2Text.setFillColor(Color::Red);

    Text timerText("", font, 20);
    timerText.setPosition(260, 20);
    timerText.setFillColor(Color::White);

    Text gameStateText("", font, 24);
    gameStateText.setPosition(200, 400);
    gameStateText.setFillColor(Color::Yellow);
    float messageTimer = 0;
    bool showMessage = false;

    Text winnerText("", font, 36);
    winnerText.setPosition(200, 250);
    winnerText.setFillColor(Color::White);

    Text powerupText1("", font, 18);
    powerupText1.setPosition(50, 50);
    powerupText1.setFillColor(Color::Blue);

    Text powerupText2("", font, 18);
    powerupText2.setPosition(420, 50);
    powerupText2.setFillColor(Color::Red);

    sf::RectangleShape powerupButton1(sf::Vector2f(160, 30));
    powerupButton1.setPosition(100, 80);
    powerupButton1.setFillColor(sf::Color::Cyan);

    Text powerupButtonText1("*Powerup_p2*", font, 12);
    powerupButtonText1.setFillColor(sf::Color::Black);
    powerupButtonText1.setPosition(105, 88);

    sf::RectangleShape powerupButton2(sf::Vector2f(160, 30));
    powerupButton2.setPosition(530, 80);
    powerupButton2.setFillColor(sf::Color::Cyan);

    Text powerupButtonText2("*Powerup_p2*", font, 12);
    powerupButtonText2.setFillColor(sf::Color::Black);
    powerupButtonText2.setPosition(535, 88);

    Text freezeText1("", font, 16);
    freezeText1.setPosition(80, 120);
    freezeText1.setFillColor(sf::Color::Cyan);

    Text freezeText2("", font, 16);
    freezeText2.setPosition(550, 120);
    freezeText2.setFillColor(sf::Color::Cyan);

    bool showEndScreen = false;

    while (window.isOpen())
    {
        window.clear();
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        if (showMessage)
        {
            messageTimer -= time;
            if (messageTimer <= 0)
            {
                showMessage = false;
            }
        }

        if (Game)
        {
            gameTimer -= gameTimerClock.restart().asSeconds();
            if (gameTimer <= 0)
            {
                gameTimer = 0;
                Game = false;
            }
        }

        if (powerupActive1)
        {
            powerupTimer1 += time;
            if (powerupTimer1 >= powerupDuration)
            {
                powerupActive1 = false;
                powerupTimer1 = 0.0f;
            }
        }

        if (powerupActive2)
        {
            powerupTimer2 += time;
            if (powerupTimer2 >= powerupDuration)
            {
                powerupActive2 = false;
                powerupTimer2 = 0.0f;
            }
        }

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
            }


            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            {
                sf::Texture saveLoadBackgroundTexture;
                if (!saveLoadBackgroundTexture.loadFromFile("images1/gameov.jpg"))
                {
                    cout << "Failed to load save/load background texture!" << endl;
                }
                sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

                sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
                sf::Vector2u windowSize = window.getSize();
                saveLoadBackgroundSprite.setScale(
                    windowSize.x / static_cast<float>(saveLoadTextureSize.x),
                    windowSize.y / static_cast<float>(saveLoadTextureSize.y)
                );

                window.clear();
                window.draw(saveLoadBackgroundSprite);

                Text exitText("GAME ENDED", font, 40);
                exitText.setFillColor(Color::Yellow);
                exitText.setPosition(180, 120);

                stringstream finalScoreStream;
                finalScoreStream << "Score_P1: " << score1;
                Text finalScoreText(finalScoreStream.str(), font, 25);
                finalScoreText.setFillColor(Color::White);
                finalScoreText.setPosition(150, 230);

                stringstream finalScoreStream2;
                finalScoreStream2 << "Score_P2: " << score2;
                Text finalScoreText2(finalScoreStream2.str(), font, 25);
                finalScoreText2.setFillColor(Color::White);
                finalScoreText2.setPosition(150, 280);

                Text returnText("Returning to menu...", font, 20);
                returnText.setFillColor(Color::White);
                returnText.setPosition(150, 330);

                window.draw(exitText);
                window.draw(finalScoreText);
                window.draw(finalScoreText2);
                window.draw(returnText);
                window.display();

                sleep(milliseconds(2000));

                return;
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::RShift && availablePowerups1 > 0 && !powerupActive1 && Game)
                {
                    powerupActive1 = true;
                    powerupTimer1 = 0.0f;
                    availablePowerups1--;

                }

                if (e.key.code == Keyboard::Tab && availablePowerups2 > 0 && !powerupActive2 && Game)
                {
                    powerupActive2 = true;
                    powerupTimer2 = 0.0f;
                    availablePowerups2--;

                }
            }

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (Game &&
                    availablePowerups1 > 0 &&
                    !powerupActive1 &&
                    powerupButton1.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    powerupActive1 = true;
                    powerupTimer1 = 0.0f;
                    availablePowerups1--;

                }

                if (Game &&
                    availablePowerups2 > 0 &&
                    !powerupActive2 &&
                    powerupButton2.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    powerupActive2 = true;
                    powerupTimer2 = 0.0f;
                    availablePowerups2--;
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            dx1 = -1;
            dy1 = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            dx1 = 1;
            dy1 = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            dx1 = 0;
            dy1 = -1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            dx1 = 0;
            dy1 = 1;
        }

        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            dx2 = -1;
            dy2 = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            dx2 = 1;
            dy2 = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            dx2 = 0;
            dy2 = -1;
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            dx2 = 0;
            dy2 = 1;
        }

        if (!Game)
        {
            sf::Texture saveLoadBackgroundTexture;
            if (!saveLoadBackgroundTexture.loadFromFile("images1/gameov.jpg"))
            {
                cout << "Failed to load save/load background texture!" << endl;
            }
            sf::Sprite saveLoadBackgroundSprite(saveLoadBackgroundTexture);

            sf::Vector2u saveLoadTextureSize = saveLoadBackgroundTexture.getSize();
            sf::Vector2u windowSize = window.getSize();
            saveLoadBackgroundSprite.setScale(
                windowSize.x / static_cast<float>(saveLoadTextureSize.x),
                windowSize.y / static_cast<float>(saveLoadTextureSize.y)
            );


            if (!showEndScreen)
            {
                showEndScreen = true;

                string winnerMessage;
                if (score1 > score2)
                {
                    winnerMessage = "PLAYER 1 WINS!";
                    winnerText.setFillColor(Color::Blue);
                }
                else if (score2 > score1)
                {
                    winnerMessage = "PLAYER 2 WINS!";
                    winnerText.setFillColor(Color::Red);
                }
                else
                {
                    winnerMessage = "IT'S A TIE!";
                    winnerText.setFillColor(Color::Yellow);
                }

                winnerText.setString(winnerMessage);

                FloatRect textBounds = winnerText.getLocalBounds();
                winnerText.setOrigin(textBounds.width / 2, textBounds.height / 2);
                winnerText.setPosition(window.getSize().x / 2, window.getSize().y / 2 - 50);
            }
            window.draw(saveLoadBackgroundSprite);
            window.draw(winnerText);

            Text finalScore1("Player 1: " + std::to_string(score1), font, 24);
            finalScore1.setFillColor(Color::White);
            finalScore1.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 + 30);

            Text finalScore2("Player 2: " + std::to_string(score2), font, 24);
            finalScore2.setFillColor(Color::White);
            finalScore2.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 + 70);

            // Text restartText("Press ESC to End", font, 18);
             //restartText.setFillColor(Color::White);
            // restartText.setPosition(window.getSize().x / 2 - 170, window.getSize().y / 2 + 130);

            window.draw(finalScore1);
            window.draw(finalScore2);
            // window.draw(restartText);

            window.display();
            sleep(milliseconds(3000));
            return;
        }

        if (timer > delay)
        {
            int prevX1 = x1, prevY1 = y1;
            int prevX2 = x2, prevY2 = y2;

            x1 += dx1; y1 += dy1;
            if (x1 < 0) x1 = 0; if (x1 >= N) x1 = N - 1;
            if (y1 < 0) y1 = 0; if (y1 >= M) y1 = M - 1;

            x2 += dx2; y2 += dy2;
            if (x2 < 0) x2 = 0; if (x2 >= N) x2 = N - 1;
            if (y2 < 0) y2 = 0; if (y2 >= M) y2 = M - 1;

            if (x1 == x2 && y1 == y2)
            {
                if ((dx1 != 0 || dy1 != 0) && (dx2 != 0 || dy2 != 0))
                {
                    Game = false;
                    gameStateText.setString("Both players collided! Game over!");
                    gameStateText.setPosition(70, 400);
                    showMessage = true;
                    messageTimer = 3.0f;
                }
                else if ((dx1 != 0 || dy1 != 0) && (dx2 == 0 && dy2 == 0))
                {
                    Game = false;
                    gameStateText.setString("Player 1 collided with Player 2! Player 1 loses!");
                    gameStateText.setPosition(70, 400);
                    showMessage = true;
                    messageTimer = 3.0f;
                }
                else if ((dx1 == 0 && dy1 == 0) && (dx2 != 0 || dy2 != 0))
                {
                    Game = false;
                    gameStateText.setString("Player 2 collided with Player 1! Player 2 loses!");
                    gameStateText.setPosition(70, 400);
                    showMessage = true;
                    messageTimer = 3.0f;
                }
            }

            if (grid[y1][x1] == 3)
            {
                Game = false;
                gameStateText.setString("Player 1 hit their own trail!");
                gameStateText.setPosition(70, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }

            if (grid[y2][x2] == 4)
            {
                Game = false;
                gameStateText.setString("Player 2 hit their own trail!");
                gameStateText.setPosition(70, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }

            if (grid[y1][x1] == 4)
            {
                Game = false;
                gameStateText.setString("Player 1 hit Player 2's trail! Player 1 loses!");
                gameStateText.setPosition(70, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }

            if (grid[y2][x2] == 3)
            {
                Game = false;
                gameStateText.setString("Player 2 hit Player 1's trail! Player 2 loses!");
                gameStateText.setPosition(80, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }

            if (grid[y1][x1] == 0 && Game)
            {
                grid[y1][x1] = 3;
                tilesCapturedThisMove1++;
                capturing1 = true;
                if (!activeTiles1) {
                    activeTiles1 = true;
                }
            }

            if (grid[y2][x2] == 0 && Game)
            {
                grid[y2][x2] = 4;
                tilesCapturedThisMove2++;
                capturing2 = true;
                if (!activeTiles2)
                {
                    activeTiles2 = true;
                }
            }

            if (capturing1 && grid[y1][x1] == 1)
            {
                dx1 = dy1 = 0;

                for (int i = 0; i < enemyCount; i++)
                {
                    drop(a[i].y / ts, a[i].x / ts);
                }

                int filledTiles = 0;

                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }
                        else if (grid[i][j] == 0 || grid[i][j] == 3)
                        {
                            grid[i][j] = 1;
                            filledTiles++;
                        }
                    }
                }

                int multiplier = (filledTiles > 10) ? 2 : 1;
                int oldScore1 = score1;
                score1 += filledTiles * multiplier;

                while (oldScore1 < nextPowerupScore1 && score1 >= nextPowerupScore1)
                {
                    availablePowerups1++;

                    if (nextPowerupScore1 < 100)
                    {
                        nextPowerupScore1 += 20;
                    }
                    else
                    {
                        nextPowerupScore1 += 30;
                    }
                }

                capturing1 = false;
                tilesCapturedThisMove1 = 0;
            }

            if (capturing2 && grid[y2][x2] == 1)
            {
                dx2 = dy2 = 0;

                for (int i = 0; i < enemyCount; i++)
                {
                    drop(a[i].y / ts, a[i].x / ts);
                }

                int filledTiles = 0;

                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }
                        else if (grid[i][j] == 0 || grid[i][j] == 4)
                        {
                            grid[i][j] = 1;
                            filledTiles++;
                        }
                    }
                }

                int multiplier = (filledTiles > 10) ? 2 : 1;
                int oldScore2 = score2;
                score2 += filledTiles * multiplier;

                while (oldScore2 < nextPowerupScore2 && score2 >= nextPowerupScore2)
                {
                    availablePowerups2++;

                    if (nextPowerupScore2 < 100)
                    {
                        nextPowerupScore2 += 20;
                    }
                    else {
                        nextPowerupScore2 += 30;
                    }
                }

                capturing2 = false;
                tilesCapturedThisMove2 = 0;
            }

            timer = 0;
        }

        if (!powerupActive1 && !powerupActive2)
        {
            for (int i = 0; i < enemyCount; i++)
            {
                a[i].move();
            }
        }

        for (int i = 0; i < enemyCount; i++)
        {
            int enemyGridX = a[i].x / ts;
            int enemyGridY = a[i].y / ts;

            if (grid[enemyGridY][enemyGridX] == 3)
            {
                Game = false;
                gameStateText.setString("Enemy hit Player 1's trail!");
                gameStateText.setPosition(70, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }
            if (grid[enemyGridY][enemyGridX] == 4)
            {
                Game = false;
                gameStateText.setString("Enemy hit Player 2's trail!");
                gameStateText.setPosition(70, 400);
                showMessage = true;
                messageTimer = 3.0f;
            }
        }

        window.draw(gameBackgroundSprite);

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                {
                    continue;
                }
                if (grid[i][j] == 1)
                {
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                }
                if (grid[i][j] == 3)
                {
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setColor(Color::Blue);
                }
                if (grid[i][j] == 4)
                {
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setColor(Color::Red);
                }
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
                sTile.setColor(Color::White);
            }
        }

        sHero1.setPosition(x1 * ts, y1 * ts);
        sHero1.setColor(Color::Blue);
        window.draw(sHero1);

        sHero2.setPosition(x2 * ts, y2 * ts);
        sHero2.setColor(Color::Red);
        window.draw(sHero2);
        sHero1.setColor(Color::White);
        sHero2.setColor(Color::White);

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++)
        {
            if (powerupActive1 && powerupActive2)
            {
                sEnemy.setColor(sf::Color(150, 100, 255, 200));
            }
            else if (powerupActive1)
            {
                sEnemy.setColor(sf::Color(100, 100, 255, 200));
            }
            else if (powerupActive2)
            {
                sEnemy.setColor(sf::Color(255, 100, 100, 200));
            }
            else
            {
                sEnemy.setColor(sf::Color::White);
            }

            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        sEnemy.setColor(sf::Color::White);

        stringstream ss1, ss2, ss3;
        ss1 << "P1: " << score1;
        score1Text.setString(ss1.str());
        window.draw(score1Text);

        ss2 << "P2: " << score2;
        score2Text.setString(ss2.str());
        window.draw(score2Text);

        int minutes = static_cast<int>(gameTimer) / 60;
        int seconds = static_cast<int>(gameTimer) % 60;
        ss3 << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
        timerText.setString(ss3.str());
        window.draw(timerText);

        stringstream pss1, pss2;
        pss1 << "P1 Power-ups: " << availablePowerups1;
        powerupText1.setString(pss1.str());
        window.draw(powerupText1);

        pss2 << "P2 Power-ups: " << availablePowerups2;
        powerupText2.setString(pss2.str());
        window.draw(powerupText2);

        window.draw(powerupText1);
        window.draw(powerupText2);

        window.draw(powerupButton1);
        window.draw(powerupButtonText1);
        window.draw(powerupButton2);
        window.draw(powerupButtonText2);

        if (availablePowerups1 > 0 && !powerupActive1)
        {
            powerupButton1.setFillColor(sf::Color::Cyan);
            powerupButtonText1.setFillColor(sf::Color::Black);
        }
        else
        {
            powerupButton1.setFillColor(sf::Color(100, 100, 100, 150));
            powerupButtonText1.setFillColor(sf::Color(200, 200, 200, 200));
        }

        if (availablePowerups2 > 0 && !powerupActive2)
        {
            powerupButton2.setFillColor(sf::Color::Cyan);
            powerupButtonText2.setFillColor(sf::Color::Black);
        }
        else
        {
            powerupButton2.setFillColor(sf::Color(100, 100, 100, 150));
            powerupButtonText2.setFillColor(sf::Color(200, 200, 200, 200));
        }
        if (showMessage)
        {
            window.draw(gameStateText);
        }

        window.display();
    }
}


#endif
