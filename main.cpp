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
#include "String.h"
#include "Renderable/Text.h"
#include "Renderable/Button.h"
#include "Renderable/AnimatedSprite.h"
#include "Renderable/SpriteSheet.h"

using namespace std;

const SDL_Color BLACK{ 0, 0, 0,255 };
const SDL_Color WHITE{ 255, 255, 255,255 };
const SDL_Color WINDOW_COLOR{ 230, 230, 230 };
const SDL_Color BUTTON_COLOR{ 198, 140, 83,255 };
const Vec2i BUTTON_SIZE{ 160, 35 };
const Vec2i BTN_LOOK_SIZE{ 50, 20 };
const Vec2i BTN_LETTERS_SIZE{ 70, 20 };
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
vector<string> wordLengthList;
vector<string> wordComptibleList;
Text secretWord;
Text publicWord;
Text wrongLetters;
Text currentLetter;
Text label_wrongLetters;
Text label_currentLetter;

Text label_wordsDictionary;
Text label_wordsLength;
Text label_wordsCompatible;

Text count_wordsDictionary;
Text count_wordsLength;
Text count_wordsCompatible;

Button btnReveal;
Button btnAnalysis;

Button btnLook_1;
Button btnLetters_1;

Button btnLook_2;
Button btnLetters_2;

Button btnLook_3;
Button btnLetters_3;

SpriteSheet human;
Vec2i humanPosition;
AnimatedSprite ghost;
TTF_Font* font24;
TTF_Font* font18;
TTF_Font* font12;
SDL_TimerID ghostTimer;

bool running = true;
bool didReveal = false;
bool gameOver = false;
bool isGhost = false;
bool isAnalysisOpen = true;


void buildAssets();

void loadWordList();


void processEvents();
void prepareLetter(int scancode);
void confirmLetter();
void reveal();
void resetGame();

void draw();

void updateWordsCompatibleCorrect(vector<string>& compList, const string& letter, const string& secretWord);
void updateWordsCompatibleWrong(vector<string>& compList, const string& letter);


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
    font12 = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 12);

    loadWordList();

    random.setSize(wordList.size());

    auto windowRect = SDL_Rect{ 0, 0, WINDOW_SIZE.x, 0 };

    publicWord.init(renderer, font24, BLACK, { 0, 50 }, windowRect);
    label_wrongLetters.init(renderer, font24, BLACK, { 0, 100 }, windowRect);
    wrongLetters.init(renderer, font24, BLACK, { 0, 150 }, windowRect);
    label_currentLetter.init(renderer, font24, BLACK, { 0, 200 }, windowRect);
    currentLetter.init(renderer, font24, BLACK, { 0, 250 }, windowRect);

    label_wordsDictionary.init(renderer, "All words in dictionary", font12, BLACK, { 20, 150 });
    count_wordsDictionary.init(renderer, to_string(wordList.size()), font12, BLACK, { 20, 170 });

    label_wordsLength.init(renderer, "Words of length {}", font12, BLACK, { 20, 200 });
    count_wordsLength.init(renderer, to_string(wordLengthList.size()), font12, BLACK, { 20, 220 });

    label_wordsCompatible.init(renderer, "Words compatible", font12, BLACK, { 20, 250 });
    count_wordsCompatible.init(renderer, to_string(wordComptibleList.size()), font12, BLACK, { 20, 270 });

    btnLook_1.init("Look", BTN_LOOK_SIZE, { 65, 170 }, BUTTON_COLOR, font12, BLACK, renderer);
    btnLetters_1.init("Letters", BTN_LETTERS_SIZE, { 65 + BTN_LOOK_SIZE.x + 15, 170 }, BUTTON_COLOR, font12, BLACK, renderer);

    btnLook_2.init("Look", BTN_LOOK_SIZE, { 65, 220 }, BUTTON_COLOR, font12, BLACK, renderer);
    btnLetters_2.init("Letters", BTN_LETTERS_SIZE, { 65 + BTN_LOOK_SIZE.x + 15, 220 }, BUTTON_COLOR, font12, BLACK, renderer);

    btnLook_3.init("Look", BTN_LOOK_SIZE, { 65, 270 }, BUTTON_COLOR, font12, BLACK, renderer);
    btnLetters_3.init("Letters", BTN_LETTERS_SIZE, { 65 + BTN_LOOK_SIZE.x + 15, 270 }, BUTTON_COLOR, font12, BLACK, renderer);

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

    std::sort(wordList.begin(), wordList.end());
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
            else if (scancode == SDL_SCANCODE_F3) {
                if (isAnalysisOpen) {
                    isAnalysisOpen = false;
                    btnAnalysis.text.setValue("Analysis [+]");
                }
                else {
                    isAnalysisOpen = true;
                    btnAnalysis.text.setValue("Analysis [-]");
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            auto x = ev.button.x;
            auto y = ev.button.y;

            if (btnReveal.containsPoint(x, y))
            {
                if (!gameOver) {
                    reveal();
                }
                else {
                    resetGame();
                }
            }
            else if (btnAnalysis.containsPoint(x, y))
            {
                if (isAnalysisOpen) {
                    isAnalysisOpen = false;
                    btnAnalysis.text.setValue("Analysis [+]");
                }
                else {
                    isAnalysisOpen = true;
                    btnAnalysis.text.setValue("Analysis [-]");
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

    label_wordsLength.setValue("Words of length " + to_string(secretWord.value.size()));

    btnReveal.text.setValue("Reveal");


    wordLengthList.clear();
    std::copy_if(wordList.begin(), wordList.end(), std::back_inserter(wordLengthList), [](string word) {return word.size() == secretWord.value.size(); });
    wordComptibleList = wordLengthList;
    count_wordsLength.setValue(to_string(wordLengthList.size()));
    count_wordsCompatible.setValue(to_string(wordLengthList.size()));

    human.currentIndex = 0;

    // debug
    cout << secretWord.value + "\n\n";
    //for (auto& w : wordComptibleList) {
    //    cout << w << "\n";
    //}
    //cout << "\n";
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
                    updateWordsCompatibleWrong(wordComptibleList, letter);
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
            updateWordsCompatibleCorrect(wordComptibleList, letter, secretWord.value);
        }
    }

    currentLetter.setValue("");

    // debug
    //for (auto& w : wordComptibleList) {
    //    cout << w << endl;
    //}
    //cout << endl;
}

void draw()
{
    publicWord.render();
    label_wrongLetters.render();
    wrongLetters.render();
    label_currentLetter.render();
    currentLetter.render();

    if (isAnalysisOpen) {
        label_wordsDictionary.render();
        count_wordsDictionary.render();
        label_wordsLength.render();
        count_wordsLength.render();
        label_wordsCompatible.render();
        count_wordsCompatible.render();

        btnLook_1.render();
        btnLetters_1.render();
        btnLook_2.render();
        btnLetters_2.render();
        btnLook_3.render();
        btnLetters_3.render();
    }

    if (isGhost) {
        ghost.render();
    }
    else {
        human.render();
    }

    btnReveal.render();
    btnAnalysis.render();
}

void updateWordsCompatibleCorrect(vector<string>& compList, const string& letter, const string& secretWord)
{
    auto rem = std::remove_if(compList.begin(), compList.end(), [letter, secretWord](string word) {
        // get positions of letter in word
        vector<int> positions;
        auto pos = -1;
        while (true)
        {
            pos = word.find(letter, pos + 1);
            if (pos == string::npos)
                break;
            else
                positions.push_back(pos);
        }

        // get positions of letter in secret word
        vector<int> secretPositions;
        pos = -1;
        while (true)
        {
            pos = secretWord.find(letter, pos + 1);
            if (pos == string::npos)
                break;
            else
                secretPositions.push_back(pos);
        }

        // if positions are not equal, remove word
        if (positions != secretPositions)
            return true;
        else
            return false;
        });
    compList.erase(rem, compList.end());
    count_wordsCompatible.setValue(to_string(compList.size()));
}

void updateWordsCompatibleWrong(vector<string>& compList, const string& letter)
{
    auto rem = std::remove_if(compList.begin(), compList.end(), [letter](string word) {
        if (word.find(letter) != string::npos)
            return true;
        else
            return false;
        });
    compList.erase(rem, compList.end());
    count_wordsCompatible.setValue(to_string(compList.size()));
}