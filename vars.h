#ifndef VARS_H
#define VARS_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <regex>

struct{
    const std::string ADD = "add";
    const std::string SEL = "sel";
    const std::string REMOVE = "rm";
    const std::string EDIT = "edit";
    const std::string SORT = "sort";
    const std::string RAND = "r";
    const std::string QUIT = "q";
    const std::string RAND1 = "i";
    const std::string RAND2 = "j";
    const std::string SHUFFLE = "shuffle";
    const std::string PRINT = "p";
    const std::string LAST_RANDOM = "k";
    const std::string LAST_RANDOM1 = "a";
    const std::string SAVE = "s";
    const std::string CLS = "clear";
    const std::string SHOW_FILENAME = "w";
    const std::string COLOR = "color";
    const std::string RED = "\e[38;5;196m";
    const std::string GREEN = "\e[38;5;46m";
    const std::string DEFAULT_COLOR = "\e[38;5;7";
    const std::string PERMISSION_ERROR = "Permission denied";
    bool MOTLEY;
    std::string FILENAME;
    std::string text_color;
    std::string curr_color_code;
    std::vector<int> COLORS;
} app;

void SetConsoleColor(std::string color);
void SaveColorToFile();
void PrintMessage(const std::string& color, const std::string& msg);
int GetRandomNumber(int min_val, int max_val);
std::string GetRandomColor();
void FillColorList();
void ReadColorFromFile();


#endif
