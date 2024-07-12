#ifndef GV_ABC_SUPER_H
#define GV_ABC_SUPER_H

#include <unordered_map>
#include <string>
#include <vector>
#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "map/mapper/mapperInt.h"

using namespace std;

typedef struct Mio_LibraryStruct_t_ Mio_Library_t;
typedef struct Mio_GateStruct_t_ Mio_Gate_t;

extern "C" {
    Mio_Library_t * Mio_LibraryRead( char * FileName, char * pBuffer, char * ExcludeFile, int fVerbose );
}

class AbcSuperMgr;
class AbcSuperGate;
extern AbcSuperMgr* abcSuperMgr;

void Abc_Super_Precompute();
Vec_Str_t * Abc_Super_PrecomputeStr( Mio_Library_t * pLibGen, int nVarsMax, int nLevels, int nGatesMax, float tDelayMax, float tAreaMax, int TimeLimit, int fSkipInv, int fVerbose );
void Abc_replace_super(string file_name);
void reset_super_area();


class TreeStr {
public:
    TreeStr() {}
    ~TreeStr() {}
    void set_leaf(bool is_leaf) { _is_leaf = is_leaf; }
    bool Is_leaf() { return _is_leaf; }
    void set_content(string content) { _content = content; }
    string get_content() { return _content; }
    void set_left(TreeStr* left) { _left_child = left; }
    TreeStr* get_left() { return _left_child; }
    void set_right(TreeStr* right) { _right_child = right; }
    TreeStr* get_right() { return _right_child; }

private:
    TreeStr* _left_child;
    TreeStr* _right_child;
    bool _is_leaf = false;
    string _content;
};

class AbcSuperMgr {
public:
    AbcSuperMgr() {}
    ~AbcSuperMgr() {}
    int get_gate_num() { return name_to_super.size(); }
    const unordered_map<string, AbcSuperGate*>& get_name_to_super() { return name_to_super; }
    AbcSuperGate* get_super(string name) { return name_to_super[name]; }
    void reset_wire_counter() { wire_counter = 0; }
    void add_wire_counter() { wire_counter++; }
    int get_wire_counter() { return wire_counter; }
    void add_super_gate(string name, AbcSuperGate* super) {name_to_super[name] = super; }
    void add_numId_to_cost(int numId, double cost) {numId_to_cost[numId] = cost; }
    double get_super_cost(int numId) { 
        if (!numId_to_cost.count(numId)) {
            return 0;
        }
        return numId_to_cost[numId]; 
    }

private:
    unordered_map<string, AbcSuperGate*> name_to_super;
    unordered_map<int, double> numId_to_cost;
    int wire_counter;

};

class AbcSuperGate {
public:
    AbcSuperGate() {}
    ~AbcSuperGate() {}
    void set_function(string func) { _function = func; }
    string get_function() { return _function; }
    void set_area(double area) { _area = area; }
    double get_area() { return _area; }
    void set_tree_function(TreeStr* tree_function) { _tree_function = tree_function; }
    TreeStr* get_tree_function() { return _tree_function; }

private:
    TreeStr* _tree_function;
    string _function;
    double _area;
};


#endif