#include <iostream>
#include <vector>
#include <map>
#include <any>
#include <tuple>
#include <stdexcept>
#include <string>

class TestArguments {
    std::map<std::string, std::tuple<int, std::string, std::string>> core;
    int index = 0;

public:

    friend std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments);

    void add(std::string name, std::string type, std::string value) {
        core.insert({ name, std::make_tuple(index++, type, value) });
        // std::cout << "name:" << name << ", index:" << std::get<0>(core[name]) << ", type:" << std::get<1>(core[name]) << ", value:" << std::get<2>(core[name]) << std::endl;
    }

    template<typename T>
    T get(std::string name) const {
        std::tuple<int, std::string, std::string> argument = core.at(name);

        return parse<T>(std::get<1>(argument), std::get<2>(argument));
    }

    template<typename T>
    T get(int index) const {
        for (auto &x : core) {
	        if (std::get<0>(x.second) == index) {
                return get<T>(x.first);
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

        if (type == "String") return cast<T>(value);
        else if (type == "char") return cast<T>(static_cast<char>(value.at(0)));
        else if (type == "short") return cast<T>(static_cast<short>(std::stoi(value)));
        else if (type == "byte") return cast<T>(std::stoi(value));
        else if (type == "int") return cast<T>(std::stoi(value));
        else if (type == "long") return cast<T>(std::stol(value));
        else if (type == "float") return cast<T>(std::stof(value));
        else if (type == "double") return cast<T>(std::stod(value));
        else if (type == "boolean") return cast<T>(value == "true");
        else {
            std::cerr << "Unknown parameter type: " << type << ". Converting the parameter to String\n";
            return cast<T>(value);
        }
    }

    template<typename T>
    T cast(std::any element) const {
        return std::any_cast<T>(element);
    }

};

std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments)
{
    // std::stringstream output;
    std::string output;
    for (auto &x : testArguments.core) {
        output += x.first + "|" + std::get<1>(x.second) + "|" + std::get<2>(x.second) + " : ";
    }
    output.resize(output.size() - 3);
    os << output;

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