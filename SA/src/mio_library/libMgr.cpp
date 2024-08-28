#include "libMgr.h"
#include "gvAbcMgr.h"
#include <iostream>

extern AbcMgr* abcMgr;
MioMgr* mioMgr;
SclMgr* sclMgr;

MioMgr::MioMgr() {
    Mio_Library_t* library = (Mio_Library_t *)abcMgr->get_Abc_Frame_t()->pLibGen;
    buf = Mio_LibraryReadBuf(library);
    inv = Mio_LibraryReadInv(library);
    and2 = Mio_LibraryReadAnd2(library);
    nand2 = Mio_LibraryReadNand2(library);
    or2 = Mio_LibraryReadOr2(library);
    nor2 = Mio_LibraryReadNor2(library);
    Mio_Gate_t* temp;
    Mio_LibraryForEachGate(library, temp) {
        if (!strcmp( "A*!B+!A*B" , Mio_GateReadForm(temp)) || !strcmp( "(A & !B) | (!A & B)" , Mio_GateReadForm(temp))) {
            xor2 = temp;
        }
        else if (!strcmp( "A*B+!A*!B",  Mio_GateReadForm(temp)) || !strcmp( "(A & B) | (!A & !B)" , Mio_GateReadForm(temp))) {
            xnor2 = temp;
        }
    }

    name_to_gate["not"] = inv;
    name_to_gate["and"] = and2;
    name_to_gate["nand"] = nand2;
    name_to_gate["or"] = or2;
    name_to_gate["nor"] = nor2;
    name_to_gate["buf"] = buf;
    name_to_gate["xor"] = xor2;
    name_to_gate["xnor"] = xnor2;
}

void MioMgr::revise_genlib(map<string, vector<double>>& timing_dic, map<string, pair<string, double>>& temp_dic) {
    Mio_Library_t* library = (Mio_Library_t *)abcMgr->get_Abc_Frame_t()->pLibGen;
    library->fPinFilter += 1;
    for (auto & it : timing_dic) {
        Mio_Pin_t* pinA = name_to_gate[it.first]->pPins;
        Mio_Pin_t* pinB = pinA->pNext;
        if (it.first != "not" && it.first != "buf") {
            pinB->dDelayBlockFall = it.second[0];
            pinB->dDelayBlockRise = it.second[0];
            pinB->dDelayBlockMax  = it.second[0];
            pinB->dDelayFanoutFall = it.second[1];
            pinB->dDelayFanoutRise = it.second[1];
        }
        pinA->dDelayBlockFall = it.second[0];
        pinA->dDelayBlockRise = it.second[0];
        pinA->dDelayBlockMax  = it.second[0];
        pinA->dDelayFanoutFall = it.second[1];
        pinA->dDelayFanoutRise = it.second[1];
    }
    for (auto & it : temp_dic) {
        Mio_Gate_t* gate = name_to_gate[it.first];
        gate->dArea = it.second.second; 
    }
}

SclMgr::SclMgr() {
    SC_Lib* library = (SC_Lib*)abcMgr->get_Abc_Frame_t()->pLibScl;
    int cell_num = SC_LibCellNum(library);
    for (int i = 0; i < cell_num; ++i) {
        SC_Cell* pCell = SC_LibCell(library, i);
        if (!strcmp( "A & B" , SC_CellPinOutFunc(pCell, 0))) {
            and2 = pCell;
        }
        else if (!strcmp( "A | B" , SC_CellPinOutFunc(pCell, 0))) {
            or2 = pCell;
        }
        else if (!strcmp( "A" , SC_CellPinOutFunc(pCell, 0))) {
            buf = pCell;
        }
        else if (!strcmp( "!A" , SC_CellPinOutFunc(pCell, 0))) {
            inv = pCell;
        }
        else if (!strcmp( "!(A & B)" , SC_CellPinOutFunc(pCell, 0))) {
            nand2 = pCell;
        }
        else if (!strcmp( "!(A | B)" , SC_CellPinOutFunc(pCell, 0))) {
            nor2 = pCell;
        }
        else if (!strcmp( "(A & B) | (!A & !B)" , SC_CellPinOutFunc(pCell, 0))) {
            xnor2 = pCell;
        }
        else if (!strcmp( "(A & !B) | (!A & B)" , SC_CellPinOutFunc(pCell, 0))) {
            xor2 = pCell;
        }
    }
    name_to_cell["not"] = inv;
    name_to_cell["and"] = and2;
    name_to_cell["nand"] = nand2;
    name_to_cell["or"] = or2;
    name_to_cell["nor"] = nor2;
    name_to_cell["buf"] = buf;
    name_to_cell["xor"] = xor2;
    name_to_cell["xnor"] = xnor2;
}

void SclMgr::revise_scllib(map<string, vector<double>>& timing_dic, map<string, pair<string, double>>& temp_dic) {
    SC_Lib* library = (SC_Lib*)abcMgr->get_Abc_Frame_t()->pLibScl;
    for (auto & it : temp_dic) {
        SC_Cell* cell = name_to_cell[it.first];
        cell->area = it.second.second;
        cell->areaI = int(cell->area);
    }
    Abc_SclInstallGenlib( library, 0, 0, 0 );
    Mio_LibraryTransferCellIds();
    mioMgr = new MioMgr;
    mioMgr->revise_genlib(timing_dic, temp_dic);
}
