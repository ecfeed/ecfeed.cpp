#include <iostream>
#include <vector>
#include <any>
#include <tuple>

class TestArguments 
{
    std::vector<std::tuple<std::string, std::string, std::string>> core;

    friend std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments);

public:

    void add(std::string name, std::string type, std::string value) 
    {
        core.push_back(std::make_tuple(name, type, value));
    }

    template<typename T>
    T get(int index) const 
    {
        std::tuple<std::string, std::string, std::string> argument = core.at(index);

        return parse<T>(std::get<1>(argument), std::get<2>(argument));
    }

    std::string getString(int index) const 
    { 
        return get<std::string>(index); 
    }

    bool getBool(int index) const 
    { 
        return get<bool>(index); 
    }

    double getDouble(int index) const 
    { 
        return get<double>(index); 
    }

    double getFloat(int index) const 
    { 
        return get<float>(index); 
    }

    int getLong(int index) const 
    { 
        return get<long>(index); 
    }

    int getInt(int index) const 
    { 
        return get<int>(index); 
    }

private:

    template<typename T>
    T parse(std::string type, std::string value) const 
    {

        if (type == "String") return std::any_cast<T>(value);
        else if (type == "char") return std::any_cast<T>(static_cast<char>(value.at(0)));
        else if (type == "short") return std::any_cast<T>(static_cast<short>(std::stoi(value)));
        else if (type == "byte") return std::any_cast<T>(std::stoi(value));
        else if (type == "int") return std::any_cast<T>(std::stoi(value));
        else if (type == "long") return std::any_cast<T>(std::stol(value));
        else if (type == "float") return std::any_cast<T>(std::stof(value));
        else if (type == "double") return std::any_cast<T>(std::stod(value));
        else if (type == "boolean") return std::any_cast<T>(value == "true");
        else 
        {
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