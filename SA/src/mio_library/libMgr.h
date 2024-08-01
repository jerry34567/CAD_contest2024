#ifndef LIB_MGR_H
#define LIB_MGR_H

#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "map/mio/mio.h"
#include "map/mio/mioInt.h"
#include "map/scl/sclLib.h"
#include "map/scl/scl.h"

#include <map>
#include <string>
#include <vector>

using namespace std;
class MioMgr;
class SclMgr;

extern MioMgr* mioMgr;
extern SclMgr* sclMgr;


class MioMgr {
public:
    MioMgr();
    ~MioMgr(){};
    void revise_genlib(map<string, vector<double>>& timing_dic, map<string, pair<string, double>>& temp_dic);
private:
    Mio_Gate_t* buf;
    Mio_Gate_t* inv;
    Mio_Gate_t* and2;
    Mio_Gate_t* nand2;
    Mio_Gate_t* or2;
    Mio_Gate_t* nor2;
    Mio_Gate_t* xor2;
    Mio_Gate_t* xnor2;
    map<string, Mio_Gate_t*> name_to_gate;
};

class SclMgr {
public:
    SclMgr();
    ~SclMgr(){};
    void revise_scllib(map<string, vector<double>>& timing_dic, map<string, pair<string, double>>& temp_dic);
private:
    SC_Cell* buf;
    SC_Cell* inv;
    SC_Cell* and2;
    SC_Cell* nand2;
    SC_Cell* or2;
    SC_Cell* nor2;
    SC_Cell* xor2;
    SC_Cell* xnor2;
    map<string, SC_Cell*> name_to_cell;
};


#endif