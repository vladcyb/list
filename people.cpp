#include "person.cpp"

typedef std::tuple<std::string, std::string> stringTie;

class People{
public:

    Person GetLastRandomPerson() const {
        return last_random_person;
    }

    void PrintTable() const {
        int i = 1;
        std::cout << std::endl;
        for(const auto& person : people){
            if (app.MOTLEY){
                std::cout << "\e[38;5;" + GetRandomColor() + "m";
            }
            if(person.selected){
                std::cout << app.RED;
            }
            std::cout << i++ << ' ' << person << std::endl;
            std::cout << "\e[38;5;" + app.curr_color_code + "m";
        }
    }

    void Add(const std::string& s){
        std::stringstream ss(s);
        Person person;
        ss >> person;
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

    void Sort(const std::string& by_what){
        if(by_what == "2"){
            sort(people.begin(), people.end(), [&](const Person& p1, const Person& p2){
                return GetRankcolumnSecondFirst(p1) < GetRankcolumnSecondFirst(p2);
            });
        }else{
            sort(people.begin(), people.end(), [&](const Person& p1, const Person& p2){
                return GetRankColumnFirstSecond(p1) < GetRankColumnFirstSecond(p2);
            });
        }
    }

    int GetPeopleSize() const {
        return people.size();
    }

    void Dump(std::ofstream& stream) const {
        for(const Person& person : people){
            stream << person.field1 << ',' << person.field2 << ',' << person.selected << std::endl;
        }
    }

    void Shuffle(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (people.begin(), people.end(), std::default_random_engine(seed));
    }

    std::string GetRandomPerson(const std::string& q) {
        last_random_person = people.at(GetRandomNumber(0, people.size() - 1));
        return q == app.RAND ? last_random_person.ToString() : q == app.RAND1 ? last_random_person.field1 : last_random_person.field2;
    }

    void PrintRandomPerson(const std::string& q){
        std::cout << GetRandomPerson(q) << std::endl;
    }

    bool isChanged(){
        return changed;
    }

    void SetChanged(bool b){
        changed = b;
    }

private:
    bool changed = false;
    Person last_random_person;
    std::vector<Person> people;
    stringTie GetRankColumnFirstSecond(const Person& p){
        return tie(p.field1, p.field2);
    }
    stringTie GetRankcolumnSecondFirst(const Person& p){
        return tie(p.field2, p.field1);
    }
};

bool Save(People& people){
    using namespace std;
    if (!people.isChanged()){
        return true;
    }
    while(true){
        cout << "Save changes? (y/n)" << endl;
        string c;
        cin >> c;
        if(c == "y"){
                ofstream output(app.FILENAME);
                if(!output){
                    PrintMessage(app.RED, app.PERMISSION_ERROR);
                return false;
            }
            people.Dump(output);
            people.SetChanged(false);
            PrintMessage(app.GREEN, "Saved");
            return true;
        }else if (c == "n"){
            return true;
        }
    }
}

bool Proc(People& people, std::string& last_query){
    using namespace std;
    string query, q;
    getline(cin, q);
    stringstream query_stream(q);
    query_stream >> query;
    if(!query.empty()){
        last_query = query;
    }
    if(query == app.ADD){
        query = "";
        getline(query_stream, query);
        people.Add(query);
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == app.REMOVE){
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
        }else if(s1 == app.SEL){
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
    }else if(query == app.EDIT){
        getline(query_stream, query);
        stringstream ss(query);
        int n;
        ss >> n;
        if (n > people.GetPeopleSize() || n < 1){
            cout << "ERROR" << endl;
        }else{
            Person person;
            ss >> person;
            people.Set(n, person);
            people.SetChanged(true);
        }
        people.PrintTable();
    }else if(query == app.SEL){

        set<size_t> marks;
        size_t n;
        while(query_stream >> n){
            marks.insert(n);
        }
        if(marks.empty()){
            PrintMessage(app.RED, "Usage: sel n1, n2, n3, ...");
        }
        for(const auto& x : marks){
            people.ToggleSelect(x);
        }
        people.SetChanged(true);
        people.PrintTable();

    }else if(query == app.CLS){
        system("clear");
        people.PrintTable();
    }else if(query == app.LAST_RANDOM || query == app.LAST_RANDOM1){
        cout << people.GetLastRandomPerson() << endl;
    }else if(query == app.SORT){
        string by_what;
        query_stream >> by_what;
        people.Sort(by_what);
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == app.RAND ||
                query == app.RAND1 || query == app.RAND2){
        people.PrintRandomPerson(query);
    }else if(query == app.SHUFFLE){
        people.Shuffle();
        people.SetChanged(true);
        people.PrintTable();
    }else if(query == app.QUIT) {
        if(Save(people)){
            cout << "(quit)" << endl << endl;
            return false;
        }
    }else if (query == app.PRINT){
        people.PrintTable();
    }else if(query == app.SAVE){
        if (people.isChanged()){
            Save(people);
        }else{
            cout << "Nothing to save" << endl;
        }
        query="";
    }else if(query == app.COLOR){
        string color;
        query_stream.clear();
        query_stream >> color;
        SetConsoleColor(color);
        SaveColorToFile();
        query="";
    }else if (query == app.SHOW_FILENAME){
        cout << app.FILENAME << endl;
    }else if (query.empty()){
        if (last_query == app.RAND || last_query == app.RAND1 || last_query == app.RAND2){
            people.PrintRandomPerson(last_query);
        }
    }
    return true;
}
