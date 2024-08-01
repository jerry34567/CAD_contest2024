#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "ext/ext.h"
#include "map/mio/mio.h"


int ReplaceNotGate( Abc_Frame_t* pAbc );
Abc_Ntk_t* NtkPerformRNG( Abc_Ntk_t* pNtk );

int ConstPropagate( Abc_Frame_t* pAbc );
Abc_Ntk_t* NtkPerformConstPorp( Abc_Ntk_t* pNtk );

void replace_not_with_nand();