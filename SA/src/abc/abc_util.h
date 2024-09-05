#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "map/mio/mio.h"
#include <fstream>


int ReplaceNotGate( Abc_Frame_t* pAbc );
Abc_Ntk_t* NtkPerformRNG( Abc_Ntk_t* pNtk );

int ConstPropagate( Abc_Frame_t* pAbc );
Abc_Ntk_t* NtkPerformConstPorp( Abc_Ntk_t* pNtk );

void replace_not_with_nand();
void replace_not_with_nor();
void replace_not_func(int TimeOut, int mode, double& best_cost, double& effort_cost, int switch_mode);
void add_const_on_pi();
void generate_abcrc();