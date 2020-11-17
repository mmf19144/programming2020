#include <iostream>
#include <fstream>
#include "hash_map.cpp"

template<typename Key, typename Value>
size_t getCntUniqValues(hash_map<Key, Value> *hm){
    hash_map<Value, bool> a(100, 0.75);
    for(const auto& it: *hm){
        a.insert(it.second, true);
    }
    return a.getCapacity();
}

template<typename Key, typename Value>
void run(std::istream &input, std::ostream &output) {
    size_t n;
    input >> n;
    char action;
    hash_map<Key, Value> hash_map(100, 0.75);
    Key key;
    Value val;
    for (size_t i = 0; i < n; i++) {
        input >> action;
        if (action == 'A') {
            input >> key >> val;
            hash_map.insert(key, val);
        }
        if (action == 'R') {
            input >> key;
            hash_map.delKey(key);
        }
    }
    output << hash_map.getCapacity() << " " << getCntUniqValues<Key, Value>(&hash_map);
}

int main() {
    char key_type, val_type;
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    input >> key_type >> val_type;

    if (key_type == 'I' && val_type == 'S') {
        run<int, std::string>(input, output);
    } else if (key_type == 'I' && val_type == 'D') {
        run<int, double>(input, output);
    } else if (key_type == 'D' && val_type == 'S') {
        run<double, std::string>(input, output);
    } else if (key_type == 'D' && val_type == 'I') {
        run<double, int>(input, output);
    } else if (key_type == 'S' && val_type == 'I') {
        run<std::string, int>(input, output);
    } else if (key_type == 'S' && val_type == 'D') {
        run<std::string, double>(input, output);
    } else if (key_type == 'I' && val_type == 'I') {
        run<int, int>(input, output);
    } else if (key_type == 'D' && val_type == 'D') {
        run<double, double>(input, output);
    } else if (key_type == 'S' && val_type == 'S') {
        run<std::string, std::string>(input, output);
    }

    return 0;

}
