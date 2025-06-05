#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <graphics.h> 

#include "game.h"
#include "menu.h"

const int MAX_WORDS = 100;
const int MAX_LEN = 30;
const int GRID_SIZE = 20;
const int CELL_SIZE = 30;

char words[MAX_WORDS][MAX_LEN];
bool used[MAX_WORDS] = {false};
bool guessed[MAX_WORDS] = {false};
int wordCount = 0;

char grid[GRID_SIZE][GRID_SIZE];
char dirGrid[GRID_SIZE][GRID_SIZE];

std::string questions[MAX_WORDS];

PlacedWord placedWords[MAX_WORDS];
int placedCount = 0;

void initGrid() {
    for (int i = 0; i < GRID_SIZE; ++i)
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = '.';
            dirGrid[i][j] = '.';
        }
}

void loadWords(const char* filename) {
    std::ifstream file(filename);
    while (file && wordCount < MAX_WORDS) {
        file >> words[wordCount];
        ++wordCount;
    }
    file.close();
}

void loadQuestions(const char* filename) {
    std::ifstream file(filename);
    std::string line;
    int i = 0;
    while (std::getline(file, line) && i < MAX_WORDS) {
        questions[i++] = line;
    }
    file.close();
}

bool tryPlaceHorizontally(const char* word, int row, int col, int crossIndex, int number, int wordIndex) {
    int len = strlen(word);
    int startCol = col - crossIndex;
    int endCol = startCol + len - 1;

    if (startCol < 0 || endCol >= GRID_SIZE) return false;

    for (int i = -1; i <= len; ++i) {
        int c = startCol + i;
        if (c < 0 || c >= GRID_SIZE) continue;

        if (row > 0 && dirGrid[row - 1][c] == 'H') return false;
        if (row < GRID_SIZE - 1 && dirGrid[row + 1][c] == 'H') return false;

        if (i == -1 || i == len) {
            if (row >= 0 && row < GRID_SIZE && grid[row][c] != '.') return false;
        }
        if (startCol - 1 >= 0 && grid[row][startCol - 1] != '.') return false;
        if (endCol + 1 < GRID_SIZE && grid[row][endCol + 1] != '.') return false;

    }

    for (int i = 0; i < len; ++i) {
        int c = startCol + i;
        char cur = grid[row][c];
        char dir = dirGrid[row][c];

        if (cur != '.' && (cur != word[i] || dir == 'H')) return false;
    }

    for (int i = 0; i < len; ++i) {
        grid[row][startCol + i] = word[i];
        dirGrid[row][startCol + i] = 'H';
    }

    placedWords[placedCount++] = {row, startCol, 'H', number, word, questions[wordIndex]};
    return true;
}

bool tryPlaceVertically(const char* word, int row, int col, int crossIndex, int number, int wordIndex) {
    int len = strlen(word);
    int startRow = row - crossIndex;
    int endRow = startRow + len - 1;

    if (startRow < 0 || endRow >= GRID_SIZE) return false;

    for (int i = -1; i <= len; ++i) {
        int r = startRow + i;
        if (r < 0 || r >= GRID_SIZE) continue;

        if (col > 0 && dirGrid[r][col - 1] == 'V') return false;
        if (col < GRID_SIZE - 1 && dirGrid[r][col + 1] == 'V') return false;

        if (i == -1 || i == len) {
            if (col >= 0 && col < GRID_SIZE && grid[r][col] != '.') return false;
        }
        if (startRow - 1 >= 0 && grid[startRow - 1][col] != '.') return false;
        if (endRow + 1 < GRID_SIZE && grid[endRow + 1][col] != '.') return false;

    }

    for (int i = 0; i < len; ++i) {
        int r = startRow + i;
        char cur = grid[r][col];
        char dir = dirGrid[r][col];

        if (cur != '.' && (cur != word[i] || dir == 'V')) return false;
    }

    for (int i = 0; i < len; ++i) {
        grid[startRow + i][col] = word[i];
        dirGrid[startRow + i][col] = 'V';
    }

    placedWords[placedCount++] = {startRow, col, 'V', number, word, questions[wordIndex]};
    return true;
}

bool placeWithIntersection(const char* word, int number, int wordIndex) {
    int len = strlen(word);

    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            char cell = grid[r][c];
            if (cell == '.') continue;

            for (int i = 0; i < len; ++i) {
                if (word[i] != cell) continue;

                if (dirGrid[r][c] == 'V') {
                    if (tryPlaceHorizontally(word, r, c, i, number, wordIndex)) return true;
                } else if (dirGrid[r][c] == 'H') {
                    if (tryPlaceVertically(word, r, c, i, number, wordIndex)) return true;
                }
            }
        }
    }

    return false;
}

std::string inputString(int x, int y, int maxLen = 20) {
    std::string result;
    char ch;

    while (true) {
        ch = getch();

        if (ch == 13) break; // Enter
        else if (ch == 8) { // Backspace
            if (!result.empty()) {
                result.pop_back();
            }
        }
        else if (isprint(ch) && result.length() < maxLen) {
            result += ch;
        }

        // Перерисовка всей строки
        setcolor(BLACK);
        setfillstyle(SOLID_FILL, BLACK);
        bar(x, y, x + maxLen * 12, y + 25); // Очищаем всю строку

        setcolor(WHITE);
        setbkcolor(BLACK);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
        outtextxy(x, y, result.c_str());
    }

    return result;
}

void drawGrid() {
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);

    int selectedNumber = 0;

    // Устанавливаем первый неугаданный номер по умолчанию
    for (int i = 0; i < placedCount; ++i) {
        if (!guessed[i]) {
            selectedNumber = placedWords[i].number;
            break;
        }
    }

    bool waitingForInput = false;
    const int GRID_OFFSET_X = 50;
    const int GRID_OFFSET_Y = 50;
    const int CELL_SIZE = 30;
    const int QUESTIONS_X = 600;

    while (true) {
        cleardevice();

        // Проверка на победу
        bool allGuessed = true;
        for (int i = 0; i < placedCount; ++i) {
            if (!guessed[i]) {
                allGuessed = false;
                break;
            }
        }

        if (allGuessed) {
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
            setcolor(GREEN);
            outtextxy(500, 700, (char*)"Ты победил!");
            getch();
            break;
        }

        // Отрисовка сетки (ограничиваем размер, чтобы поместилась на экране)
        int visibleGridSize = std::min(GRID_SIZE, 15); // Показываем 15x15 клеток
        for (int i = 0; i < visibleGridSize; ++i) {
            for (int j = 0; j < visibleGridSize; ++j) {
                bool highlight = false;

                // Подсветка выбранного слова
                for (int k = 0; k < placedCount; ++k) {
                    if (placedWords[k].number == selectedNumber) {
                        int r = placedWords[k].row;
                        int c = placedWords[k].col;
                        int len = placedWords[k].word.length();
                        for (int l = 0; l < len; ++l) {
                            int rr = r + (placedWords[k].direction == 'V' ? l : 0);
                            int cc = c + (placedWords[k].direction == 'H' ? l : 0);
                            if (rr == i && cc == j) {
                                highlight = true;
                                break;
                            }
                        }
                    }
                }

                int x = j * CELL_SIZE + GRID_OFFSET_X;
                int y = i * CELL_SIZE + GRID_OFFSET_Y;

                if (grid[i][j] != '.') {
                    setcolor(highlight ? RED : WHITE);
                    rectangle(x, y, x + CELL_SIZE, y + CELL_SIZE);

                    // Показываем буквы только если слово угадано
                    char displayChar = ' ';
                    for (int k = 0; k < placedCount; ++k) {
                        if (guessed[k]) {
                            int r = placedWords[k].row;
                            int c = placedWords[k].col;
                            int len = placedWords[k].word.length();
                            for (int l = 0; l < len; ++l) {
                                int rr = r + (placedWords[k].direction == 'V' ? l : 0);
                                int cc = c + (placedWords[k].direction == 'H' ? l : 0);
                                if (rr == i && cc == j) {
                                    displayChar = grid[i][j];
                                }
                            }
                        }
                    }

                    char ch[2] = { displayChar, '\0' };
                    setcolor(WHITE);
                    outtextxy(x + 10, y + 5, ch);
                }
            }
        }

        // Номера слов
        auto isNumberDrawn = [](int row, int col, int currentIndex) {
            for (int i = 0; i < currentIndex; ++i) {
                if (placedWords[i].row == row && placedWords[i].col == col) {
                    return true;
                }
            }
            return false;
        };

        for (int i = 0; i < placedCount; ++i) {
            int row = placedWords[i].row;
            int col = placedWords[i].col;
            if (row >= visibleGridSize || col >= visibleGridSize) continue;

            if (isNumberDrawn(row, col, i)) continue;

            int x = col * CELL_SIZE + GRID_OFFSET_X;
            int y = row * CELL_SIZE + GRID_OFFSET_Y;

            char numberStr[4];
            sprintf(numberStr, "%d", placedWords[i].number);
            outtextxy(x + 2, y + 2, numberStr);
        }

        // Вопросы (справа от сетки)
        int yOffset = GRID_OFFSET_Y;
        outtextxy(QUESTIONS_X, yOffset - 20, (char*)"Вопросы:");

        for (int i = 0; i < placedCount; ++i) {
            std::string line = std::to_string(placedWords[i].number) + ". ";
            line += (placedWords[i].direction == 'H') ? "-> " : "v  ";
            line += placedWords[i].question;
            
            // Выделяем текущий выбранный вопрос
            if (placedWords[i].number == selectedNumber) {
                setcolor(RED);
            } else if (guessed[i]) {
                setcolor(GREEN);
            } else {
                setcolor(WHITE);
            }
            
            outtextxy(QUESTIONS_X, yOffset + i * 20, line.c_str());
        }

        // Инструкция
        setcolor(WHITE);
        outtextxy(GRID_OFFSET_X, 700, (char*)"TAB - следующее слово, ENTER - ввод, ESC - выход");

        // Ввод слова, если нужно
        if (waitingForInput) {
            for (int i = 0; i < placedCount; ++i) {
                if (placedWords[i].number == selectedNumber && !guessed[i]) {
                    setcolor(WHITE);
                    std::string prompt = "Введите слово для номера " + std::to_string(selectedNumber) + ":";
                    outtextxy(GRID_OFFSET_X, 730, prompt.c_str());

                    delay(100); // Чтобы не глючило после getch()
                    std::string input = inputString(GRID_OFFSET_X, 750);

                    if (input == placedWords[i].word) {
                        guessed[i] = true;
                    }

                    waitingForInput = false;
                    break;
                }
            }
            continue; // Перерисовать поле после ввода
        }

        // Обработка клавиш
        int key = getch();

        if (key == 9) { // TAB
            // Найдём следующий неугаданный номер
            bool foundNext = false;
            for (int i = 1; i <= placedCount; ++i) {
                int nextIndex = (selectedNumber + i - 1) % placedCount;
                if (!guessed[nextIndex]) {
                    selectedNumber = placedWords[nextIndex].number;
                    foundNext = true;
                    break;
                }
            }
            if (!foundNext) selectedNumber = 0;
        }
        else if (key == 13) { // ENTER
            for (int i = 0; i < placedCount; ++i) {
                if (placedWords[i].number == selectedNumber && !guessed[i]) {
                    waitingForInput = true;
                    break;
                }
            }
        }
        else if (key == 27) { // ESC
            break;
        }
    }
}

void cleanupGame() {
    // Очищаем массив слов
    for (int i = 0; i < MAX_WORDS; ++i) {
        memset(words[i], 0, MAX_LEN);
        questions[i].clear();
        used[i] = false;
        guessed[i] = false;
    }

    // Очищаем игровое поле
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = '\0';
            dirGrid[i][j] = '\0';
        }
    }

    // Очищаем массив размещенных слов
    for (int i = 0; i < MAX_WORDS; ++i) {
        placedWords[i] = {0, 0, '\0', 0, "", ""};
    }

    // Сбрасываем счетчики
    wordCount = 0;
    placedCount = 0;
}

void game() {
    srand(time(0));
    loadWords("wordlist.txt");
    loadQuestions("questions.txt");

    initGrid();

    int first = rand() % wordCount;
    const char* firstWord = words[first];
    used[first] = true;

    int len = strlen(firstWord);
    int row = GRID_SIZE / 2;
    int col = (GRID_SIZE - len) / 2;

    for (int i = 0; i < len; ++i) {
        grid[row][col + i] = firstWord[i];
        dirGrid[row][col + i] = 'H';
    }

    placedWords[placedCount++] = {row, col, 'H', 1, firstWord, questions[first]};
    int placed = 1;

    for (int tries = 0; placed < m && tries < 750; ++tries) {
        int index = rand() % wordCount;
        if (used[index]) continue;

       
        if (placeWithIntersection(words[index], placed + 1, index)) {
            used[index] = true;
            ++placed;
        }
    }

    drawGrid();
    
    // Очистка в конце игры
    cleanupGame();
}

