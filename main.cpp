#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "Vec2.h"
#include "Random.h"
#include "Renderable.h"
#include "String.h"
#include "Renderable/SpriteSheet.h"

using namespace std;

const SDL_Color BLACK{ 0, 0, 0,255 };
const SDL_Color WHITE{ 255, 255, 255,255 };
const SDL_Color WINDOW_COLOR{ 230, 230, 230 };
const SDL_Color BUTTON_COLOR{ 198, 140, 83,255 };
const Vec2i BUTTON_SIZE{ 160, 35 };
const Vec2i WINDOW_SIZE{ 800,600 };

const Vec2i GHOST_SIZE{ 48, 64 };
const vector<Vec2i> GHOST_FRAMES = {
    {0,2}, {1,2}, {2,2},
};
const int GHOST_FRAME_DURATION = 500;
const float GHOST_SCALE = 3.0f;


Random random;
SDL_Window* window;
SDL_Renderer* renderer;

vector<string> wordList;
Text secretWord;
Text publicWord;
Text wrongLetters;
Text currentLetter;
Text label_wrongLetters;
Text label_currentLetter;

Button btnReveal;
Button btnAnalysis;

SpriteSheet human;
Vec2i humanPosition;
AnimatedSprite ghost;
TTF_Font* font24;
TTF_Font* font18;
SDL_TimerID ghostTimer;

bool running = true;
bool didReveal = false;
bool gameOver = false;
bool isGhost = false;


void buildAssets();

void loadWordList();


void processEvents();
void prepareLetter(int scancode);
void confirmLetter();
void reveal();
void resetGame();

void draw();


int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow(
        "Hangman",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_SIZE.x, WINDOW_SIZE.y, 0
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    buildAssets();

    resetGame();

    while (running)
    {
        processEvents();

        SDL_SetRenderDrawColor(renderer, WINDOW_COLOR.r, WINDOW_COLOR.g, WINDOW_COLOR.b, WINDOW_COLOR.a);
        SDL_RenderClear(renderer);
        draw();
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}

void buildAssets()
{
    font24 = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 24);
    font18 = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 18);

    loadWordList();

    random.setSize(wordList.size());

    auto windowRect = SDL_Rect{ 0, 0, WINDOW_SIZE.x, 0 };

    publicWord.init(renderer, font24, BLACK, { 0, 50 }, windowRect);
    label_wrongLetters.init(renderer, font24, BLACK, { 0, 100 }, windowRect);
    wrongLetters.init(renderer, font24, BLACK, { 0, 150 }, windowRect);
    label_currentLetter.init(renderer, font24, BLACK, { 0, 200 }, windowRect);
    currentLetter.init(renderer, font24, BLACK, { 0, 250 }, windowRect);

    human.load("assets/human.png", { 0,300 }, { 100,200 }, { 0, 0, WINDOW_SIZE.x, 0 }, renderer);
    human.currentIndex = 0;

    Vec2i humanCenter = { human.position.x + human.spriteSize.x / 2,  human.position.y + human.spriteSize.y / 2 };

    ghost.load("assets/ghost/ghost.png", GHOST_SIZE, renderer);
    ghost.renderer = renderer;
    ghost.scaledSize = { (int)(GHOST_SCALE * ghost.gridSize.x), (int)(GHOST_SCALE * ghost.gridSize.y) };
    ghost.frames = GHOST_FRAMES;
    ghost.duration = GHOST_FRAME_DURATION;
    ghost.position = { humanCenter.x - ghost.scaledSize.x / 2, humanCenter.y - ghost.scaledSize.y / 2 };

    SDL_AddTimer(ghost.duration, [](Uint32 interval, void* param) {
        auto ghost = (AnimatedSprite*)param;
        ghost->currentFrame = (ghost->currentFrame + 1) % ghost->frames.size();
        return interval;
        }, &ghost);

    btnReveal.init(BUTTON_SIZE, { 20,20 }, BUTTON_COLOR, font18, BLACK, renderer);
    btnAnalysis.init("Analysis [+]", BUTTON_SIZE, { 20,70 }, BUTTON_COLOR, font18, BLACK, renderer);
}

void loadWordList()
{
    ifstream infile("assets/words.txt");
    string line;
    while (getline(infile, line))
    {
        if (!isAlpha(line) || line.size() < 6)
            continue;

        toUpper(line);
        wordList.push_back(line);
    }
}


void processEvents()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
        case SDL_WINDOWEVENT:
            if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
                running = false;

        case SDL_KEYDOWN:
        {
            auto scancode = ev.key.keysym.scancode;
            if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
                prepareLetter(scancode);
            }
            else if (scancode == SDL_SCANCODE_RETURN) {
                confirmLetter();
            }
            else if (scancode == SDL_SCANCODE_F2) {
                if (!gameOver) {
                    reveal();
                }
                else {
                    resetGame();
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            auto x = ev.button.x;
            auto y = ev.button.y;

            if (x >= btnReveal.position.x && x <= btnReveal.position.x + btnReveal.size.x &&
                y >= btnReveal.position.y && y <= btnReveal.position.y + btnReveal.size.y)
            {
                if (!gameOver) {
                    reveal();
                }
                else {
                    resetGame();
                }
            }

            break;
        }

        default:
            break;
        }
    }
}

void reveal()
{
    if (gameOver)
        return;
    publicWord.setValue(secretWord.value);
    didReveal = true;
    gameOver = true;
    label_currentLetter.setValue("You Lose!");
    btnReveal.text.setValue("Reset");

    if (!isGhost) {
        human.currentIndex = human.maxIndex;
    }
}

void resetGame()
{
    gameOver = false;
    didReveal = false;
    isGhost = false;

    auto idx = random.getNumber();
    secretWord.setValue(wordList[idx]);
    publicWord.assignValue(secretWord.value.size(), '_');

    wrongLetters.setValue("");
    label_wrongLetters.setValue("Wrong Letters (0):");
    label_currentLetter.setValue("Current Letter:");
    currentLetter.setValue("");

    btnReveal.text.setValue("Reveal");

    human.currentIndex = 0;

    // cout << secretWord.value + "\n" << endl;
}

void prepareLetter(int scancode)
{
    if (gameOver)
        return;

    if (wrongLetters.value.size() / 2 == 5)
        isGhost = true;

    char c = 'A' + (scancode - SDL_SCANCODE_A);
    string letter;
    letter.push_back(c);
    currentLetter.setValue(letter);
}

void confirmLetter()
{
    string letter = currentLetter.value;
    if (letter.empty())
        return;

    if (letter.size() > 1)
        return;

    if (!isAlpha(letter))
        return;

    toUpper(letter);

    bool found = false;
    auto pos = -1;
    while (true)
    {
        pos = secretWord.value.find(letter, pos + 1);
        if (pos == string::npos)
        {
            if (!found)
            {
                if (wrongLetters.value.find(letter) == string::npos)
                {
                    wrongLetters.appendValue(letter + " ");
                    label_wrongLetters.setValue("Wrong Letters (" + to_string(wrongLetters.value.size() / 2) + "):");
                    if (human.currentIndex < human.maxIndex) {
                        human.currentIndex++;
                    }
                }
            }
            break;
        }
        else
        {
            found = true;
            publicWord.setAt(pos, letter[0]);
            if (publicWord.value == secretWord.value)
            {
                gameOver = true;
                btnReveal.text.setValue("Reset");
                if (wrongLetters.value.size() / 2 > 5)
                    label_currentLetter.setValue("You Lose!");
                else
                    label_currentLetter.setValue("You Win!");
            }
        }
    }

    currentLetter.setValue("");
}

void draw()
{
    publicWord.render();
    label_wrongLetters.render();
    wrongLetters.render();
    label_currentLetter.render();
    currentLetter.render();


    if (isGhost) {
        ghost.render();
    }
    else {
        human.render();
    }

    btnReveal.render();
    btnAnalysis.render();
}