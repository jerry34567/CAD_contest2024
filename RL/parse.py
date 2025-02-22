import json
import sys
import subprocess

class Cell:
    def __init__(self):
        self.cell_name = ""
        self.cell_type = ""
        self.integer_attributes = []
        self.float_attributes = []

def parser(lib_file, verilog_file, cost_exe):
    input_file_name = lib_file
    output_file_name = "./contest.genlib"
    verilog_file_name = verilog_file

    #read verilog file
    try:
        with open(verilog_file_name, 'r') as read_file:
            module_name = read_file.readline().strip()
    except IOError:
        # print("can NOT open file!")
        return 1

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

    temp_dic = {}
    # analyze 
    for cell_data in j["cells"]:
        with open("temp_file.v", 'w') as write_file:
            write_file.write(module_name)
            write_file.write(" (a, b, o);\n")
            write_file.write("input a, b;\n")
            write_file.write("output o;\n")
            write_file.write(cell_data["cell_name"])
            if (cell_data["cell_type"] == "not") | (cell_data["cell_type"] == "buf"):
                write_file.write(" g0 (a, o);\n")
            else:
                write_file.write(" g0 (a, b, o);\n")
            write_file.write("endmodule\n")
        args = ["-library", input_file_name, "-netlist", "temp_file.v", "-output", "temp.out"]
        process = subprocess.Popen([cost_exe] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, _ = process.communicate()
        a = stdout.decode()
        # print(a)
        cost = a.split('=')[1].strip()
        value = temp_dic.get(cell_data["cell_type"])
        # print(cost)
        if value is not None:
            if float(value[1]) > float(cost):
                temp_dic[cell_data["cell_type"]] = [cell_data["cell_name"], float(cost)]
        else:
            temp_dic[cell_data["cell_type"]] = [cell_data["cell_name"], float(cost)]

    # write Output file
    try:
        with open(output_file_name, 'w') as writefile:
            for cell_data in temp_dic:
                # print(cost)

            # visit every cell, write genlib file
                cell = Cell()
                cell.cell_name = temp_dic[cell_data][0]
                cell.cell_type = cell_data
                writefile.write(f"GATE {cell.cell_name}\t")
                writefile.write(f"{temp_dic[cell_data][1]}")
                writefile.write("\t")
                writefile.write(f"{dictionary[cell.cell_type]}\t")
                writefile.write(f"PIN * {dictionary2[cell.cell_type]} 1 999 0 0 0 0\n")

                # for attribute in j["information"]["attributes"]:
                #     if attribute[-1] == 'i':
                #         cell.integer_attributes.append(int(cell_data[attribute]))
                #     elif attribute[-1] == 'f':
                #         cell.float_attributes.append(float(cell_data[attribute]))

                cells.append(cell)
            writefile.write("GATE zero	0	O=CONST0;\nGATE one	0	O=CONST1;\n")
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
    parser("./release/lib/lib1.json", "./release/netlists/design6.v", "./release/cost_estimators/cost_estimator_1")