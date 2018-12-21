#include "vars.h"

class Person{
public:
    std::string field1;
    std::string field2;
    bool selected = false;
    Person(std::string s = "-", std::string n = "-"){
        field1 = s;
        field2 = n;
    }
    std::string ToString() const {
        return field1 + " " + (field2 == "-" ? "" : field2);
    }
    void Trim(){
        field1 = regex_replace(field1, std::regex("^ +| +$|( ) +"), "$1");
        field2 = regex_replace(field2, std::regex("^ +| +$|( ) +"), "$1");
    }
};

std::stringstream& operator>>(std::stringstream& ss, Person& person){
    using namespace std;
    string s;
    getline(ss, s);
    stringstream ss1(s);
    if (s.find(',') != string::npos){
        getline(ss1, person.field1, ',');
        getline(ss1, person.field2, ',');
        ss1 >> person.selected;
    }else{
        ss1 >> person.field1 >> person.field2 >> person.selected;
    }
    person.Trim();
    return ss;
}

std::ostream& operator<<(std::ostream& out, const Person& person){
    return out << person.field1 << ' ' <<
    (person.field2 == "-" ? "" : person.field2);
}
