#ifndef GAME_H
#define GAME_H

#include <string>

struct PlacedWord 
{
    int row, col;
    char direction; 
    int number;
    std::string word;
    std::string question;
};

void initGrid();
void loadWords(const char*);
void loadQuestions(const char*);
bool tryPlaceHorizontally(const char* word, int row, int col, int crossIndex, int number, int wordIndex);
bool tryPlaceVertically(const char* word, int row, int col, int crossIndex, int number, int wordIndex);
bool placeWithIntersection(const char* word, int number, int wordIndex);
std::string inputString(int x, int y, int maxLen);
void drawGrid();
void game();

#endif