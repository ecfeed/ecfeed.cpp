#include <iostream>
#include <vector>
#include <map>
#include <any>
#include <tuple>
#include <stdexcept>
#include <string>

class TestArguments {
    std::vector<std::tuple<std::string, std::string, std::string>> core;

public:
    friend std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments);

    void add(std::string name, std::string type, std::string value) {
        core.push_back(std::make_tuple(name, type, value));
    }

    template<typename T>
    T get(int index) const {
        std::tuple<std::string, std::string, std::string> argument = core.at(index);

        return parse<T>(std::get<1>(argument), std::get<2>(argument));
    }

    template<typename T>
    T get(std::string name) const {
        for (int i = 0 ; i < core.size() ; i++) {
	        if (std::get<0>(core.at(i)) == name) {
                return get<T>(i);
            }
        }

        throw std::invalid_argument("Invalid index");
    }

    double getDouble(std::string name) const { return get<double>(name); }
    double getDouble(int index) const { return get<double>(index); }
    bool getBool(std::string name) const { return get<bool>(name); }
    bool getBool(int index) const { return get<bool>(index); }
    int getInt(std::string name) const { return get<int>(name); }
    int getInt(int index) const { return get<int>(index); }
    std::string getString(std::string name) const { return get<std::string>(name); }
    std::string getString(int index) const { return get<std::string>(index); }

private:
    template<typename T>
    T parse(std::string type, std::string value) const {

        if (type == "String") return std::any_cast<T>(value);
        else if (type == "char") return std::any_cast<T>(static_cast<char>(value.at(0)));
        else if (type == "short") return std::any_cast<T>(static_cast<short>(std::stoi(value)));
        else if (type == "byte") return std::any_cast<T>(std::stoi(value));
        else if (type == "int") return std::any_cast<T>(std::stoi(value));
        else if (type == "long") return std::any_cast<T>(std::stol(value));
        else if (type == "float") return std::any_cast<T>(std::stof(value));
        else if (type == "double") return std::any_cast<T>(std::stod(value));
        else if (type == "boolean") return std::any_cast<T>(value == "true");
        else {
            std::cerr << "Unknown parameter type: " << type << ". Converting the parameter to String\n";
            return std::any_cast<T>(value);
        }
    }

};

std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments)
{
    for (auto &x : testArguments.core) {
        os << std::get<1>(x) << " " << std::get<0>(x) << " = " << std::get<2>(x) << "; ";
    }

    return os;
}

// int main(int argc, char** argv){
//     TestArguments testArguments;

//     testArguments.add("uno", "long", "1");
//     testArguments.add("dos", "int", "2");
//     testArguments.add("tres", "double", "3");
//     testArguments.add("cuatro", "int", "4");
//     testArguments.add("cinco", "boolean", "true");
//     testArguments.add("seis", "String", "data");
//     auto e1 = testArguments.get<long>("uno");
//     std::cout << typeid(e1).name() << " : " << e1 << std::endl;
//     auto e2 = testArguments.get<int>(1);
//     std::cout << typeid(e2).name() << " : " << e2 << std::endl;
//     auto e3 = testArguments.getDouble("tres");
//     std::cout << typeid(e3).name() << " : " << e3 << std::endl;
//     auto e4 = testArguments.getInt("cuatro");
//     std::cout << typeid(e4).name() << " : " << e4 << std::endl;
//     auto e5 = testArguments.getBool("cinco");
//     std::cout << typeid(e5).name() << " : " << e5 << std::endl;
//     auto e6 = testArguments.getString("seis");
//     std::cout << typeid(e6).name() << " : " << e6 << std::endl;

//     return 0;
// }