#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

using namespace std;

struct Vec2i
{
    int x, y;
};

struct Random
{
    random_device rd;
    default_random_engine gen;
    uniform_int_distribution<size_t> distrib;

    Random();
    void setSize(size_t size);
    int getNumber();
};

struct Text
{
    string value;
    SDL_Texture* texture = nullptr;
    Vec2i position;

    void updateTexture();
    void setValue(const string& str);
    void assignValue(int count, char c);
    void appendValue(const string& str);
    void setAt(int pos, char c);

    void renderCenterH();
    void render();
};

struct Button
{
    Vec2i position;
    Vec2i size;
    Text text;
    SDL_Color backgroundColor;
    SDL_Color textColor;

    void render();
};


const SDL_Color BLACK{ 0, 0, 0,255 };
const SDL_Color WHITE{ 255, 255, 255,255 };
const SDL_Color WINDOW_COLOR{ 230, 230, 230 };
const SDL_Color BUTTON_COLOR{ 198, 140, 83,255 };
const Vec2i BUTTON_SIZE{ 140, 40 };
const Vec2i WINDOW_SIZE{ 800,600 };


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

Button btnReset;
Button btnReveal;

SDL_Texture* human[6];
Vec2i humanPosition;
TTF_Font* font24;
TTF_Font* font14;

bool running = true;
bool didReveal = false;
bool gameOver = false;


void buildAssets();

void loadWordList();
bool isAlpha(string& str);
void toUpper(string& str);

SDL_Texture* loadTexture(const char* path);

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
    font14 = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 14);

    loadWordList();

    random.setSize(wordList.size());

    publicWord.position = { 0,50 };
    label_wrongLetters.position = { 0,100 };
    wrongLetters.position = { 0,150 };
    label_currentLetter.position = { 0,200 };;
    currentLetter.position = { 0,250 };

    for (int i = 0; i < 6; ++i) {
        string path = "assets/human_" + to_string(i) + ".png";
        human[i] = loadTexture(path.c_str());
    }

    int w;
    SDL_QueryTexture(human[0], nullptr, nullptr, &w, nullptr);
    humanPosition.x = (WINDOW_SIZE.x - w) / 2;
    humanPosition.y = 300;

    btnReveal = {
        { 20, 20 },
        BUTTON_SIZE,
        { "" },
        BUTTON_COLOR,
        BLACK
    };

    btnReveal.text.setValue("Reveal");

    btnReset = {
        { 20, 70 },
        BUTTON_SIZE,
        { "" },
        BUTTON_COLOR,
        BLACK
    };

    btnReset.text.setValue("Reset");
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

bool isAlpha(string& str)
{
    return all_of(str.begin(), str.end(), [](char& c) {
        return isalpha(c);
        });
}

void toUpper(string& str)
{
    for_each(str.begin(), str.end(), [](char& c)
        {
            c = toupper(c);
        });
}

SDL_Texture* loadTexture(const char* path)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
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
                reveal();
            }
            else if (scancode == SDL_SCANCODE_F3) {
                resetGame();
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            auto x = ev.button.x;
            auto y = ev.button.y;

            if (x >= btnReset.position.x && x <= btnReset.position.x + btnReset.size.x &&
                y >= btnReset.position.y && y <= btnReset.position.y + btnReset.size.y) {
                resetGame();
            }
            else if (x >= btnReveal.position.x && x <= btnReveal.position.x + btnReveal.size.x &&
                y >= btnReveal.position.y && y <= btnReveal.position.y + btnReveal.size.y) {
                reveal();
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
}

void resetGame()
{
    gameOver = false;
    didReveal = false;

    auto idx = random.getNumber();
    secretWord.setValue(wordList[idx]);
    publicWord.assignValue(secretWord.value.size(), '_');

    wrongLetters.setValue("");
    label_wrongLetters.setValue("Wrong Letters (0):");
    label_currentLetter.setValue("Current Letter:");
    currentLetter.setValue("");

    // cout << secretWord.value + "\n" << endl;
}

void prepareLetter(int scancode)
{
    if (gameOver)
        return;

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
                if (wrongLetters.value.size() / 2 > 5) 
                    label_currentLetter.setValue("You Lose!");
                else
                    label_currentLetter.setValue("You Win!");
            }
        }
    }

    currentLetter.setValue("");
}


Random::Random()
{
    gen.seed(rd());
}

void Random::setSize(size_t size)
{
    distrib.param(uniform_int_distribution<size_t>::param_type(0, size - 1));
}

int Random::getNumber()
{
    return distrib(gen);
}

void Text::updateTexture()
{
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (!value.empty()) {
        auto surface = TTF_RenderText_Blended(font24, value.c_str(), BLACK);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Text::setValue(const string& str)
{
    value = str;
    updateTexture();
}

void Text::assignValue(int count, char c)
{
    value.assign(count, c);
    updateTexture();
}

void Text::appendValue(const string& str)
{
    value.append(str);
    updateTexture();
}

void Text::setAt(int pos, char c)
{
    value[pos] = c; 
    updateTexture();
}

void Text::renderCenterH()
{
    if (texture != nullptr)
    {
        SDL_Rect r;
        SDL_QueryTexture(texture, nullptr, nullptr, &r.w, &r.h);
        r.y = position.y;
        r.x = (WINDOW_SIZE.x - r.w) / 2;
        SDL_RenderCopy(renderer, texture, nullptr, &r);
    }
}

void Text::render()
{
    if (texture != nullptr) {
        SDL_Rect r;
        SDL_QueryTexture(texture, nullptr, nullptr, &r.w, &r.h);
        r.x = position.x;
        r.y = position.y;
        SDL_RenderCopy(renderer, texture, nullptr, &r);
    }
}

void Button::render()
{
    SDL_Rect buttonRect = { position.x, position.y, size.x, size.y };
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Rect textRect;
    TTF_SizeText(font24, text.value.c_str(), &textRect.w, &textRect.h);
    textRect.x = position.x + (size.x - textRect.w) / 2;
    textRect.y = position.y + (size.y - textRect.h) / 2;

    SDL_RenderCopy(renderer, text.texture, nullptr, &textRect);

}

void draw()
{
    publicWord.renderCenterH();
    label_wrongLetters.renderCenterH();
    wrongLetters.renderCenterH();
    label_currentLetter.renderCenterH();
    currentLetter.renderCenterH();

    SDL_Rect r;
    SDL_QueryTexture(human[0], nullptr, nullptr, &r.w, &r.h);
    r.x = humanPosition.x;
    r.y = humanPosition.y;

    int humanIndex;
    if (didReveal) {
        humanIndex = 5;
    }
    else {
        humanIndex = wrongLetters.value.size() / 2 > 5 ? 5 : wrongLetters.value.size() / 2;
    }

    SDL_RenderCopy(renderer, human[humanIndex], nullptr, &r);

    btnReveal.render();
    btnReset.render();
}
