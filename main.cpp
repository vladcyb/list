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
const string PERMISSION_ERROR = "Permission denied";
string FILENAME;
string text_color = "";

void PrintMessage(const string& color, const string& error){
    cout << color << error << text_color << endl;
}

typedef tuple<string, string> stringTie;

class Person{
public:
    string surname;
    string name;
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
        getline(ss1, person.name);
    }else{
        ss1 >> person.surname >> person.name;
    }
    return ss;
}

ostream& operator<<(ostream& stream, const Person& person){
    stream << left << person.surname << ' ' <<
    (person.name == "-" ? "" : person.name);
    return stream;
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
            cout << i++ << ' ' << person << endl;
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
    size_t GetPeopleSize() const {
        return people.size();
    }
    void Dump(ofstream& stream) const {
        for(const Person& person : people){
            stream << person.surname << ',' << person.name << endl;
        }
    }
    void Shuffle(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (people.begin(), people.end(), std::default_random_engine(seed));
    }
    string GetRandomPerson(const string& q) {
        std::random_device trueRandom;
        std::mt19937 pseudoRandom(trueRandom());
        std::uniform_int_distribution<int> distribution(0, people.size() - 1);
        last_random_person = people.at(distribution(pseudoRandom));
        return q == RAND ? last_random_person.ToString() : q == RAND1 ? last_random_person.surname : last_random_person.name;
    }
    void PrintRandomPerson(const string& q){
        cout << GetRandomPerson(q) << endl;
    }
private:
    Person last_random_person;
    vector<Person> people;
    stringTie GetRankSurname(const Person& p){
        return tie(p.surname, p.name);
    }
    stringTie GetRankName(const Person& p){
        return tie(p.name, p.surname);
    }
};

bool Save(People& people, bool& changes){
	if (!changes){
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
            changes = false;
            PrintMessage(GREEN, "Saved");
            return true;
        }else if (c == "n"){
            return true;
        }
    }
}

void SetConsoleColor(string color){
    text_color = "\e[38;5;" + color + "m";
    cout << text_color;
    ofstream fout(COLOR);
    fout << color;
}

bool Proc(People& people, bool& changes, string& last_query){
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
        changes = true;
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
            changes = true;
        }else{
            stringstream ss1(s);
            int n;
            set<int> numbers;
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
                    changes = true;
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
            changes = true;
        }
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
        changes = true;
        people.PrintTable();
    }else if(query == RAND ||
                query == RAND1 || query == RAND2){
        people.PrintRandomPerson(query);
    }else if(query == SHUFFLE){
        people.Shuffle();
        changes = true;
        people.PrintTable();
    }else if(query == QUIT) {
        if(Save(people, changes)){
            return false;
        }
    }else if (query == PRINT){
        people.PrintTable();
    }else if(query == SAVE){
        if (changes){
            Save(people, changes);
        }else{
            cout << "Nothing to save" << endl;
        }
        query="";
    }else if(query == COLOR){
        string color;
        query_stream >> color;
        SetConsoleColor(color);
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

int main() {
    ifstream color_file(COLOR);
    if(color_file){
        string color;
        color_file >> color;
        SetConsoleColor(color);
    }else{
        text_color = "\033[m";
    }
    while(true){
        bool changes = false;
        cin >> FILENAME;
        if (FILENAME == QUIT){
            cout << "\e[39m";
            return 0;
        }
        if(FILENAME == COLOR){
            string color;
            cin >> color;
            SetConsoleColor(color);
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
        while(Proc(people, changes, last_query));

	}
    return 0;
}
