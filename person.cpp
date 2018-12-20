#include "vars.h"

class Person{
public:
    std::string surname;
    std::string name;
    bool selected = false;
    Person(std::string s = "-", std::string n = "-"){
        surname = s;
        name = n;
    }
    std::string ToString() const {
        return surname + " " + (name == "-" ? "" : name);
    }
    void Trim(){
        surname = regex_replace(surname, std::regex("^ +| +$|( ) +"), "$1");
        name = regex_replace(name, std::regex("^ +| +$|( ) +"), "$1");
    }
};

std::stringstream& operator>>(std::stringstream& ss, Person& person){
    using namespace std;
    string s;
    getline(ss, s);
    stringstream ss1(s);
    if (s.find(',') != string::npos){
        getline(ss1, person.surname, ',');
        getline(ss1, person.name, ',');
        ss1 >> person.selected;
    }else{
        ss1 >> person.surname >> person.name >> person.selected;
    }
    person.Trim();
    return ss;
}

std::ostream& operator<<(std::ostream& out, const Person& person){
    return out << person.surname << ' ' <<
    (person.name == "-" ? "" : person.name);
}
