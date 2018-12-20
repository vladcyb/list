#include "people.cpp"

using namespace std;

void SetConsoleColor(string color){
    if (color == "m"){
        app.MOTLEY = !app.MOTLEY;
        app.text_color = "\e[38;5;" + app.curr_color_code + "m";
        cout << app.text_color;
    }else{
        app.MOTLEY = false;
        app.curr_color_code = color;
        app.text_color = "\e[38;5;" + color + "m";
        cout << app.text_color;
    }
}

void SaveColorToFile(){
    ofstream color_file(app.COLOR);
    color_file << app.curr_color_code << ' ' << app.MOTLEY;
}

void PrintMessage(const string& color, const string& msg){
    cout << color << msg << endl;
    if(!app.MOTLEY){
        cout << app.text_color;
    }
}

int GetRandomNumber(int min_val, int max_val){
    std::random_device trueRandom;
    std::mt19937 pseudoRandom(trueRandom());
    std::uniform_int_distribution<int> distribution(min_val, max_val);
    return distribution(pseudoRandom);
}

string GetRandomColor(){
    return to_string(app.COLORS[GetRandomNumber(0, app.COLORS.size() - 1)]);
}

void FillColorList(){
    int N = 231;
    app.COLORS.resize(N);
    for(int i = 0; i < N; ++i){
        app.COLORS.at(i) = i + 1;
    }
    app.COLORS.erase(app.COLORS.begin() + 15);
}

void ReadColorFromFile(){
    ifstream color_file(app.COLOR);
    if(color_file){
        color_file >> app.curr_color_code;
        color_file >> app.MOTLEY;
        if(!app.MOTLEY){
            SetConsoleColor(app.curr_color_code);
        }
    }else{
        app.curr_color_code = "7";
        app.text_color = app.DEFAULT_COLOR;
    }
}

int main() {
    FillColorList();
    ReadColorFromFile();
    while(true){
        cin >> app.FILENAME;
        if (app.FILENAME == app.QUIT){
            cout << app.DEFAULT_COLOR + "m";
            return 0;
        }
        if(app.FILENAME == app.COLOR){
            string color;
            cin.clear();
            cin >> color;
            SetConsoleColor(color);
            SaveColorToFile();
            continue;
        }
        app.FILENAME += ".csv";
        People people;
        ifstream input(app.FILENAME);
        if(input){
            string s;
            while(getline(input, s)){
                people.Add(s);
            }
            input.close();
        }
        people.PrintTable();

        string last_query;
        while(Proc(people, last_query));

    }
    return 0;
}
