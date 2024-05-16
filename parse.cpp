#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class Cell {
public:
    string cell_name;
    string cell_type;
    vector<int> integer_attributes;
    vector<double> float_attributes;
};

int main(int argc, char** argv) {
    string inputFileName = argv[1];
    string outputFileName = argv[2];
    // read JSON file
    ifstream readfile(inputFileName);
    if (!readfile.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    // parse JSON
    json j;
    readfile >> j;

    // close file
    readfile.close();

    // get attribute number
    int attribute_num = stoi(j["information"]["attribute_num"].get<string>());
    int float_attribute_num = 0;
    int int_attribute_num = 0;

    // store all cells
    vector<Cell> cells;

    // dictionary
    unordered_map<string, string> dictionary;
    dictionary["not"] = "O=!a;";
    dictionary["buf"] = "O=a;";
    dictionary["and"] = "O=a*b;";
    dictionary["nand"] = "O=!(a*b);";
    dictionary["or"] = "O=a+b;";
    dictionary["nor"] = "O=!(a+b);";
    dictionary["xor"] = "O=a*!b+!a*b;";
    dictionary["xnor"] = "O=a*b+!a*!b;";

    unordered_map<string, string> dictionary2;
    dictionary2["not"] = "INV";
    dictionary2["buf"] = "NONINV";
    dictionary2["and"] = "NONINV";
    dictionary2["nand"] = "INV";
    dictionary2["or"] = "NONINV";
    dictionary2["nor"] = "INV";
    dictionary2["xor"] = "UNKNOWN";
    dictionary2["xnor"] = "UNKNOWN";

    // information
    cout << "Cell num: " << j["information"]["cell_num"].get<string>() << endl;
    cout << "Attribute num: " << j["information"]["attribute_num"].get<string>() << endl;
    cout << "Attributes: ";
    for (const auto& attribute : j["information"]["attributes"]) {
        cout << attribute.get<string>() << " ";
        if (attribute.get<string>().back() == 'i') int_attribute_num++;
        else if (attribute.get<string>().back() == 'f') float_attribute_num++;
    }
    cout << endl << endl;

    // write Output file
    ofstream writefile(outputFileName);
    if (!writefile.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    // visit every cell, write genlib file
    for (const auto& cell_data : j["cells"]) {
        Cell cell;
        cell.cell_name = cell_data["cell_name"];
        cell.cell_type = cell_data["cell_type"];
        writefile << "GATE " << cell.cell_name << "\t";
        if (cell.cell_type=="buf" || cell.cell_type=="not") writefile << "1\t";
        else writefile << "2\t";
        writefile << dictionary[cell.cell_type] << "\t";
        writefile << "PIN * " << dictionary2[cell.cell_type] << " " << "1 999 0.5 0.5 0.5 0.5";
        writefile << "\n";

        cell.integer_attributes.reserve(int_attribute_num);
        cell.float_attributes.reserve(float_attribute_num);

        for (const auto& attribute : j["information"]["attributes"]) {
            if (attribute.get<string>().back() == 'i') {
                cell.integer_attributes.push_back(stoi((cell_data[attribute]).get<string>()));
            }

            else if (attribute.get<string>().back() == 'f') {
                cell.float_attributes.push_back(stof((cell_data[attribute]).get<string>()));
            }
        }

        cells.push_back(cell);
    }

    // cout
    for (const auto& cell : cells) {
        cout << "Cell Name: " << cell.cell_name << endl;
        cout << "Cell Type: " << cell.cell_type << endl;

        cout << "integer attribute: ";
        for (int i : cell.integer_attributes) {
            cout << i << " ";
        }
        cout << endl;

        cout << "float attribute: ";
        for (double f : cell.float_attributes) {
            cout << f << " ";
        }
        cout << endl;
    }

    writefile.close();

    return 0;
}