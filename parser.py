import json
import sys

class Cell:
    def __init__(self):
        self.cell_name = ""
        self.cell_type = ""
        self.integer_attributes = []
        self.float_attributes = []

def parser(input):
    input_file_name = input
    output_file_name = "./contest.genlib"

    # read JSON file
    try:
        with open(input_file_name, 'r') as readfile:
            j = json.load(readfile)
    except IOError:
        # print("can NOT open file!")
        return 1

    # get attribute number
    attribute_num = int(j["information"]["attribute_num"])
    float_attribute_num = 0
    int_attribute_num = 0

    # store all cells
    cells = []

    # dictionary
    dictionary = {
        "not": "O=!a;",
        "buf": "O=a;",
        "and": "O=a*b;",
        "nand": "O=!(a*b);",
        "or": "O=a+b;",
        "nor": "O=!(a+b);",
        "xor": "O=a*!b+!a*b;",
        "xnor": "O=a*b+!a*!b;"
    }

    dictionary2 = {
        "not": "INV",
        "buf": "NONINV",
        "and": "NONINV",
        "nand": "INV",
        "or": "NONINV",
        "nor": "INV",
        "xor": "UNKNOWN",
        "xnor": "UNKNOWN"
    }

    # information
    # print(f"Cell num: {j['information']['cell_num']}")
    # print(f"Attribute num: {j['information']['attribute_num']}")
    # print("Attributes: ", end="")
    for attribute in j["information"]["attributes"]:
        # print(attribute, end=" ")
        if attribute[-1] == 'i':
            int_attribute_num += 1
        elif attribute[-1] == 'f':
            float_attribute_num += 1
    # print("\n")

    # write Output file
    try:
        with open(output_file_name, 'w') as writefile:
            # visit every cell, write genlib file
            for cell_data in j["cells"]:
                cell = Cell()
                cell.cell_name = cell_data["cell_name"]
                cell.cell_type = cell_data["cell_type"]
                writefile.write(f"GATE {cell.cell_name}\t")
                if cell.cell_type in ["buf", "not"]:
                    writefile.write("1\t")
                else:
                    writefile.write("2\t")
                writefile.write(f"{dictionary[cell.cell_type]}\t")
                writefile.write(f"PIN * {dictionary2[cell.cell_type]} 1 999 0.5 0.5 0.5 0.5\n")

                for attribute in j["information"]["attributes"]:
                    if attribute[-1] == 'i':
                        cell.integer_attributes.append(int(cell_data[attribute]))
                    elif attribute[-1] == 'f':
                        cell.float_attributes.append(float(cell_data[attribute]))

                cells.append(cell)
    except IOError:
        print("can NOT open file!")
        return 1

    # cout
    # for cell in cells:
        # print(f"Cell Name: {cell.cell_name}")
        # print(f"Cell Type: {cell.cell_type}")

        # print("integer attribute: ", end="")
        # for i in cell.integer_attributes:
        #     # print(i, end=" ")
        # # print()

        # # print("float attribute: ", end="")
        # for f in cell.float_attributes:
        #     # print(f, end=" ")
        # # print()

if __name__ == "__main__":
    parser()