#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>

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
    std::random_device rd;
    std::default_random_engine gen;
    std::uniform_int_distribution<size_t> distrib;

    Random();
    void setSize(size_t size);
    int getNumber();
};

const SDL_Color BLACK{ 0,0,0,255 };

struct Text
{
    string value;
    SDL_Texture* texture = nullptr;

    void setValue(const string& str);
    SDL_Rect getRect(int y);
    void render(int y);
};

Vec2i windowSize{ 800,600 };
SDL_Window* window;
SDL_Renderer* renderer;

vector<string> wordList;
Text secretWord;
Text publicWord;
Text wrongLetters;
Text currentLetter;
Text label_wrongLetters;
Text label_currentLetter;

SDL_Texture* human[6];

SDL_Texture* loadTexture(const char* path)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

TTF_Font* font24;

Random random;

bool running = true;

bool isAlpha(string& str);
void toUpper(string& str);
void loadWordList();

void resetGame();

void handleEvent(SDL_Event& ev);
void draw();

void prepareLetter(int scancode)
{
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
    while (true) {
        pos = secretWord.value.find(letter, pos + 1);
        if (pos == string::npos) {
            if (!found) {
                if (wrongLetters.value.find(letter) == string::npos) {
                    wrongLetters.setValue(wrongLetters.value + letter + " ");
                    label_wrongLetters.setValue("Wrong Letters (" + to_string(wrongLetters.value.size() / 2) + "):");
                }
            }
            break;
        }
        else {
            found = true;
            auto pubw = publicWord.value;
            pubw[pos] = letter[0];
            publicWord.setValue(pubw);
        }
    }


    currentLetter.setValue("");
}


int main(int argc, char* argv[])
{
    // assuming no errors

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    TTF_Init();

    window = SDL_CreateWindow(
        "Hangman",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowSize.x, windowSize.y, 0
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    font24 = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 24);

    loadWordList();

    random.setSize(wordList.size());

    label_wrongLetters.setValue("Wrong Letters (0):");
    label_currentLetter.setValue("Current Letter:");

    for (int i = 0; i < 6; ++i) {
        string path = "assets/human_" + to_string(i) + ".png";
        human[i] = loadTexture(path.c_str());
    }

    resetGame();

    // ---

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleEvent(event);
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);
        draw();
        SDL_RenderPresent(renderer);
    }


    // ---

    TTF_CloseFont(font24);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

// ---

void Text::setValue(const string& str)
{
    value = str;

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

SDL_Rect Text::getRect(int y)
{
    SDL_Rect r;
    SDL_QueryTexture(texture, nullptr, nullptr, &r.w, &r.h);
    r.y = y;
    r.x = (windowSize.x - r.w) / 2;
    return r;
}

void Text::render(int y)
{
    if (texture != nullptr)
    {
        SDL_Rect r = getRect(y);
        SDL_RenderCopy(renderer, texture, nullptr, &r);
    }
}

// ---

Random::Random()
{
    gen.seed(rd());
}

void Random::setSize(size_t size)
{
    distrib.param(std::uniform_int_distribution<size_t>::param_type(0, size - 1));
}

int Random::getNumber()
{
    return distrib(gen);
}

// ---

bool isAlpha(string& str)
{
    for (auto c : str)
    {
        if (!isalpha(c))
            return false;
    }
    return true;
}

void toUpper(string& str)
{
    for (auto& c : str)
    {
        c = toupper(c);
    }
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

// ---

void resetGame()
{
    auto idx = random.getNumber();
    secretWord.setValue(wordList[idx]);

    string pubw;
    pubw.reserve();
    for (auto c : secretWord.value)
    {
        pubw.push_back('_');
    }

    publicWord.setValue(pubw);
}

// ---

void handleEvent(SDL_Event& ev)
{
    switch (ev.type) {

    case SDL_WINDOWEVENT:

        switch (ev.window.event) {

        case SDL_WINDOWEVENT_CLOSE:
            running = false;
            break;

        }

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
            resetGame();
        }
    }
    break;

    default:
        break;
    }
}


void draw()
{
    publicWord.render(50);
    label_wrongLetters.render(100);
    wrongLetters.render(150);
    label_currentLetter.render(200);
    currentLetter.render(250);

    SDL_Rect r;
    SDL_QueryTexture(human[0], nullptr, nullptr, &r.w, &r.h);
    r.x = (windowSize.x - r.w) / 2;
    r.y = 300;

    int humanIndex = wrongLetters.value.size() / 2 > 5 ? 5 : wrongLetters.value.size() / 2;

    SDL_RenderCopy(renderer, human[humanIndex], nullptr, &r);
}