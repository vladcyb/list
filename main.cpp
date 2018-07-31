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

using namespace std;

const string ADD = "add";
const string SEL = "sel";
const string REMOVE = "rm";
const string EDIT = "edit";
const string SORT = "sort";
const string RAND = "r";
const string QUIT = "q";
const string RAND1 = "i";
const string RAND2 = "j";
const string SHUFFLE = "shuffle";
const string PRINT = "p";
const string LAST_RANDOM = "k";
const string LAST_RANDOM1 = "a";
const string SAVE = "s";
const string CLS = "clear";
const string SHOW_FILENAME = "w";
const string COLOR = "color";
const string RED = "\e[38;5;196m";
const string GREEN = "\e[38;5;46m";
const string DEFAULT_COLOR = "\e[38;5;7";
const string PERMISSION_ERROR = "Permission denied";
bool MOTLEY = false;
string FILENAME;
string text_color;
string curr_color_code;
vector<int> COLORS;

void SetConsoleColor(string color){
    if (color == "m"){
        MOTLEY = !MOTLEY;
        text_color = "\e[38;5;" + curr_color_code + "m";
        cout << text_color;
    }else{
        MOTLEY = false;
        curr_color_code = color;
        text_color = "\e[38;5;" + color + "m";
        cout << text_color;
    }
}

void SaveColorToFile(){
    ofstream color_file(COLOR);
    color_file << curr_color_code << ' ' << MOTLEY;
}

void PrintMessage(const string& color, const string& msg){
    cout << color << msg << endl;
    if(!MOTLEY){
        cout << text_color;
    }
}

int GetRandomNumber(int min_val, int max_val){
    std::random_device trueRandom;
    std::mt19937 pseudoRandom(trueRandom());
    std::uniform_int_distribution<int> distribution(min_val, max_val);
    return distribution(pseudoRandom);
}

string GetRandomColor(){
    return to_string(COLORS[GetRandomNumber(0, COLORS.size() - 1)]);
}

typedef tuple<string, string> stringTie;

class Person{
public:
    string surname;
    string name;
    bool selected = false;
    Person(string s = "-", string n = "-"){
        surname = s;
        name = n;
    }
    string ToString() const {
        return surname + " " + (name == "-" ? "" : name);
    }
    void Trim(){
        surname = regex_replace(surname, regex("^ +| +$|( ) +"), "$1");
        name = regex_replace(name, regex("^ +| +$|( ) +"), "$1");
    }
};

stringstream& operator>>(stringstream& ss, Person& person){
    string s;
    getline(ss, s);
    stringstream ss1(s);
    if (s.find(',') != string::npos){
        getline(ss1, person.surname, ',');
        getline(ss1, person.name, ',');
        ss1 >> person.selected;
    }else{
        ss1 >> person.surname >> person.name;
    }
    return ss;
}

ostream& operator<<(ostream& out, const Person& person){
    return out << left << person.surname << ' ' <<
    (person.name == "-" ? "" : person.name);
}

class People{
public:
    Person GetLastRandomPerson() const {
        return last_random_person;
    }
    void PrintTable() const {
        int i = 1;
        cout << endl;
        for(const auto& person : people){
            if (MOTLEY){
                cout << "\e[38;5;" + GetRandomColor() + "m";
            }
            if(person.selected){
                cout << RED;
            }
            cout << i++ << ' ' << person << endl;
            cout << "\e[38;5;" + curr_color_code + "m";
        }
    }
    void Add(const string& s){
        stringstream ss(s);
        Person person;
        ss >> person;
        person.Trim();
        people.push_back(person);
    }
    void Erase(int n){
        people.erase(people.begin() + n - 1);
    }
    void Clear(){
        people.clear();
    }
    void Set(int n, const Person& person) {
        people.at(n - 1) = person;
    }
    void RemoveSelected(){
        for(auto it = people.begin(); it != people.end(); ++it){
            if((*it).selected){
                people.erase(it--);
                changed = true;
            }
        }
    }
    void ToggleSelect(size_t n){
        people.at(n - 1).selected = !people.at(n - 1).selected;
    }
    void Sort(const string& by_what){
        if(by_what == "2"){
            sort(people.begin(), people.end(), [&](const Person& p1, const Person& p2){
                return GetRankName(p1) < GetRankName(p2);
            });
        }else{
            sort(people.begin(), people.end(), [&](const Person& p1, const Person& p2){
                return GetRankSurname(p1) < GetRankSurname(p2);
            });
        }
    }
    int GetPeopleSize() const {
        return people.size();
    }
    void Dump(ofstream& stream) const {
        for(const Person& person : people){
            stream << person.surname << ',' << person.name << ',' << person.selected << endl;
        }
    }
    void Shuffle(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (people.begin(), people.end(), std::default_random_engine(seed));
    }
    string GetRandomPerson(const string& q) {
        last_random_person = people.at(GetRandomNumber(0, people.size() - 1));
        return q == RAND ? last_random_person.ToString() : q == RAND1 ? last_random_person.surname : last_random_person.name;
    }
    void PrintRandomPerson(const string& q){
        cout << GetRandomPerson(q) << endl;
    }
    bool isChanged(){
        return changed;
    }
    void SetChanged(bool b){
        changed = b;
    }
private:
    bool changed;
    Person last_random_person;
    vector<Person> people;
    stringTie GetRankSurname(const Person& p){
        return tie(p.surname, p.name);
    }
    stringTie GetRankName(const Person& p){
        return tie(p.name, p.surname);
    }
};

bool Save(People& people){
    if (!people.isChanged()){
        return true;
    }
    while(true){
        cout << "Save changes? (y/n)" << endl;
        string c;
        cin >> c;
        if(c == "y"){
                ofstream output(FILENAME);
                if(!output){
                    PrintMessage(RED, PERMISSION_ERROR);
                return false;
            }
            people.Dump(output);
            people.SetChanged(false);
            PrintMessage(GREEN, "Saved");
            return true;
        }else if (c == "n"){
            return true;
        }
    }
}

bool Proc(People& people, string& last_query){
    string query, q;
    getline(cin, q);
    stringstream query_stream(q);
    query_stream >> query;
    if(!query.empty()){
        last_query = query;
    }
    if(query == ADD){
        query = "";
        getline(query_stream, query);
        people.Add(query);
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == REMOVE){
        string s;
        getline(query_stream, query);
        stringstream ss(query);
        getline(ss, s);
        stringstream ss1(s);
        string s1;
        ss1 >> s1;
        if(s1 == "*"){
            people.Clear();
            people.SetChanged(true);
        }else if(s1 == SEL){
            people.RemoveSelected();
        }else{
            stringstream ss1(s);
            int n;
            set<size_t> numbers;
            ss1.clear();
            while(ss1 >> n){
                numbers.insert(n);
            }
            int i = 0;
            for(int n : numbers){
                n -= i;
                if (n > people.GetPeopleSize() || n < 1){
                    cout << "ERROR" << endl;
                }else{
                    people.Erase(n);
                    people.SetChanged(true);
                    ++i;
                }
            }
        }
        people.PrintTable();
    }else if(query == EDIT){
        getline(query_stream, query);
        stringstream ss(query);
        int n;
        ss >> n;
        if (n > people.GetPeopleSize() || n < 1){
            cout << "ERROR" << endl;
        }else{
            Person person;
            ss >> person;
            person.Trim();
            people.Set(n, person);
            people.SetChanged(true);
        }
        people.PrintTable();
    }else if(query == SEL){

        set<size_t> marks;
        size_t n;
        while(query_stream >> n){
            marks.insert(n);
        }
        if(marks.empty()){
            PrintMessage(RED, "Usage: sel n1, n2, n3, ...");
        }
        for(const auto& x : marks){
            people.ToggleSelect(x);
        }
        people.SetChanged(true);
        people.PrintTable();

    }else if(query == CLS){
        system("clear");
        people.PrintTable();
    }else if(query == LAST_RANDOM || query == LAST_RANDOM1){
        cout << people.GetLastRandomPerson() << endl;
    }else if(query == SORT){
        string by_what;
        query_stream >> by_what;
        people.Sort(by_what);
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == RAND ||
                query == RAND1 || query == RAND2){
        people.PrintRandomPerson(query);
    }else if(query == SHUFFLE){
        people.Shuffle();
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == QUIT) {
        if(Save(people)){
            return false;
        }
    }else if (query == PRINT){
        people.PrintTable();
    }else if(query == SAVE){
        if (people.isChanged()){
            Save(people);
        }else{
            cout << "Nothing to save" << endl;
        }
        query="";
    }else if(query == COLOR){
        string color;
        query_stream.clear();
        query_stream >> color;
        SetConsoleColor(color);
        SaveColorToFile();
        query="";
    }else if (query == SHOW_FILENAME){
        cout << FILENAME << endl;
    }else if (query.empty()){
        if (last_query == RAND || last_query == RAND1 || last_query == RAND2){
            people.PrintRandomPerson(last_query);
        }
    }
    return true;
}

void FillColorList(){
    int N = 231;
    COLORS.resize(N);
    for(int i = 0; i < N; ++i){
        COLORS.at(i) = i + 1;
    }
    COLORS.erase(COLORS.begin() + 15);
}

void ReadColorFromFile(){
    ifstream color_file(COLOR);
    if(color_file){
        color_file >> curr_color_code;
        color_file >> MOTLEY;
        if(!MOTLEY){
            SetConsoleColor(curr_color_code);
        }
    }else{
        curr_color_code = "7";
        text_color = DEFAULT_COLOR;
    }
}

int main() {
    FillColorList();
    ReadColorFromFile();
    while(true){
        cin >> FILENAME;
        if (FILENAME == QUIT){
            cout << DEFAULT_COLOR;
            return 0;
        }
        if(FILENAME == COLOR){
            string color;
            cin.clear();
            cin >> color;
            SetConsoleColor(color);
            SaveColorToFile();
            continue;
        }
        FILENAME += ".csv";
        People people;
        ifstream input(FILENAME);
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
