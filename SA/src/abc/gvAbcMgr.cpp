#include "gvAbcMgr.h"
#include "base/abc/abc.h"
#include "bdd/cudd/cudd.h"
#include "gvAbcNtk.h"
#include "sat/cnf/cnf.h"
#include <cstring>
#include <iostream>
#include <string>

AbcMgr* abcMgr;

void
AbcMgr::init() {
    Abc_Start();
    pAbc = Abc_FrameGetGlobalFrame();
}

void
AbcMgr::reset() {
    delete pAbc;
}

void
AbcMgr::abcReadDesign(string& fileName) {
    char pFileName[128];
    strcpy(pFileName, fileName.c_str());
    char Command[1000];
    sprintf(Command, "read %s", pFileName);
    Cmd_CommandExecute(pAbc, Command);
    pNtkMgr = new abcNtkMgr(pAbc->pNtkCur);
}

void abccmd(string command){
    char Command[1024], abcCmd[128];
    strcpy(abcCmd, command.c_str());
    sprintf(Command, "%s", abcCmd);
    Cmd_CommandExecute(abcMgr->get_Abc_Frame_t(), Command);
}