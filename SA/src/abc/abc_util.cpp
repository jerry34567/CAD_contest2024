#include "abc_util.h"
#include "gvAbcMgr.h"

extern AbcMgr* abcMgr;

int ReplaceNotGate( Abc_Frame_t * pAbc )
{
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(pAbc);

    if ( pNtk == NULL )
    {
        Abc_Print( -1, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsMappedLogic(pNtk) )
    {
        Abc_Print( -1, "This command can only be applied to a mapped logic network.\n" );
        return 1;
    }
    // modify the current network
    Abc_Ntk_t* pNew = NtkPerformRNG( pNtk );
    printf("success!\n");
    Abc_FrameReplaceCurrentNetwork(pAbc, pNew);
    return 0;
}

Abc_Ntk_t* NtkPerformRNG( Abc_Ntk_t* pNtk) {
    Abc_Ntk_t* pDupNtk = Abc_NtkDupDfs(pNtk);
    Abc_Obj_t* pNode;
    Abc_Obj_t* pFanin;
    int i;
    int j;
    Mio_Gate_t* inv = Mio_LibraryReadInv((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* and2 = Mio_LibraryReadAnd2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* nand2 = Mio_LibraryReadNand2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* or2 = Mio_LibraryReadOr2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* nor2 = Mio_LibraryReadNor2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* xor2;
    Mio_Gate_t* xnor2;
    Mio_Gate_t* temp;
    Mio_LibraryForEachGate((Mio_Library_t *)Abc_FrameReadLibGen(), temp) {
        if (!strcmp( "A*!B+!A*B" , Mio_GateReadForm(temp))) {
            xor2 = temp;
        }
        else if (!strcmp( "A*B+!A*!B",  Mio_GateReadForm(temp))) {
            xnor2 = temp;
        }
    }
    Abc_NtkForEachNode(pDupNtk, pNode, i) {
        if (Abc_ObjFanoutNum(pNode) == 1 && Abc_ObjFanout0(pNode)->pData == inv) {
            Abc_Obj_t* pTemp = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
            if (pNode->pData == and2)
                pTemp->pData = nand2;
            else if (pNode->pData == nand2)
                pTemp->pData = and2;
            else if (pNode->pData == or2)
                pTemp->pData = nor2;
            else if (pNode->pData == nor2)
                pTemp->pData = or2;
            else if (pNode->pData == xor2)
                pTemp->pData = xnor2;
            else if (pNode->pData == xnor2)
                pTemp->pData = xor2;

            if (pTemp->pData != NULL) {
                printf("in gatelist\n");
                Abc_ObjForEachFanin(pNode, pFanin, j) {
                    Abc_ObjAddFanin(pTemp, pFanin);
                }
                Abc_ObjReplace(Abc_ObjFanout0(pNode), pTemp);
            }
            else {
                printf("not in gatelist\n");
                Mio_Gate_t * pGate = (Mio_Gate_t *)pNode->pData;
                printf("%s\n", Mio_GateReadForm(pGate));                
            }
        }
    }
    return pDupNtk;
}


int ConstPropagate( Abc_Frame_t* pAbc ) {
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(pAbc);

    if ( pNtk == NULL )
    {
        Abc_Print( -1, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsMappedLogic(pNtk) )
    {
        Abc_Print( -1, "This command can only be applied to a mapped logic network.\n" );
        return 1;
    }
    // modify the current network
    Abc_NtkCleanup( pNtk, 0 );
    Abc_Ntk_t* pNew = NtkPerformConstPorp( pNtk );
    Abc_NtkCleanup( pNew, 0 );
    // printf("success!\n");
    Abc_FrameReplaceCurrentNetwork(pAbc, pNew);
    return 0;
}
Abc_Ntk_t* NtkPerformConstPorp( Abc_Ntk_t* pNtk ) {
    Abc_Ntk_t* pDupNtk = Abc_NtkDupDfs(pNtk);
    Mio_Library_t* library = (Mio_Library_t *)Abc_FrameReadLibGen();
    Abc_Obj_t* pNode;
    Abc_Obj_t* pFanin;
    Abc_Obj_t* pFanout;
    Abc_Obj_t* pFanoutFanout;
    int i;
    int j;
    int k;
    Mio_Gate_t* inv = Mio_LibraryReadInv(library);
    Mio_Gate_t* and2 = Mio_LibraryReadAnd2(library);
    Mio_Gate_t* nand2 = Mio_LibraryReadNand2(library);
    Mio_Gate_t* or2 = Mio_LibraryReadOr2(library);
    Mio_Gate_t* nor2 = Mio_LibraryReadNor2(library);
    Mio_Gate_t* const1 = Mio_LibraryReadConst1(library);
    Mio_Gate_t* const0 = Mio_LibraryReadConst0(library);
    Mio_Gate_t* xor2;
    Mio_Gate_t* xnor2;
    Mio_Gate_t* temp;
    Mio_LibraryForEachGate(library, temp) {
        if (!strcmp( "A*!B+!A*B" , Mio_GateReadForm(temp))) {
            xor2 = temp;
        }
        else if (!strcmp( "A*B+!A*!B",  Mio_GateReadForm(temp))) {
            xnor2 = temp;
        }
    }
    Abc_NtkForEachNode(pDupNtk, pNode, i) {
        if (pNode->pData == const1) {
            printf("const1!!!\n");
            Abc_ObjForEachFanout(pNode, pFanout, j) {
                int fanout_num = Abc_ObjFanoutNum(pFanout);
                if (fanout_num == 0) {
                    printf("fanout num = 0\n");
                    continue;
                }
                printf("fanout num = %d\n", fanout_num);
                Abc_Obj_t* pTemp;
                if (Abc_ObjFanin0(pFanout)->pData == const1) {
                    pTemp = Abc_ObjFanin1(pFanout);
                }
                else {
                    pTemp = Abc_ObjFanin0(pFanout);
                }
                if (pFanout->pData == and2 || pFanout->pData == xnor2) {
                    Abc_ObjReplace(pFanout, pTemp);
                }
                else if (pFanout->pData == nand2 || pFanout->pData == xor2) {
                    Abc_Obj_t* pNot = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pNot->pData = inv;
                    Abc_ObjAddFanin(pNot, pTemp);
                    Abc_ObjReplace(pFanout, pNot);
                }
                else if (pFanout->pData == inv || pFanout->pData == nor2) {
                    Abc_Obj_t* pConst0 = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pConst0->pData = const0;
                    Abc_ObjReplace(pFanout, pConst0);
                }
                else {
                    Abc_Obj_t* pConst1 = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pConst1->pData = const0;
                    Abc_ObjReplace(pFanout, pConst1);
                }
            }
        }
        else if (pNode->pData == const0) {
            printf("const0!!!\n");
            Abc_ObjForEachFanout(pNode, pFanout, j) {
                int fanout_num = Abc_ObjFanoutNum(pFanout);
                if (fanout_num == 0) {
                    printf("fanout num = 0\n");
                    continue;
                }
                printf("fanout num = %d\n", fanout_num);
                Abc_Obj_t* pTemp;
                if (Abc_ObjFanin0(pFanout)->pData == const0) {
                    pTemp = Abc_ObjFanin1(pFanout);
                }
                else {
                    pTemp = Abc_ObjFanin0(pFanout);
                }
                if (pFanout->pData == or2 || pFanout->pData == xor2) {
                    Abc_ObjReplace(pFanout, pTemp);
                }
                else if (pFanout->pData == nor2 || pFanout->pData == xnor2) {
                    Abc_Obj_t* pNot = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pNot->pData = inv;
                    Abc_ObjAddFanin(pNot, pTemp);
                    Abc_ObjReplace(pFanout, pNot);
                }
                else if (pFanout->pData == inv || pFanout->pData == nand2) {
                    Abc_Obj_t* pConst1 = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pConst1->pData = const0;
                    Abc_ObjReplace(pFanout, pConst1);
                }
                else {
                    Abc_Obj_t* pConst0 = Abc_NtkCreateObj(pDupNtk, ABC_OBJ_NODE);
                    pConst0->pData = const0;
                    Abc_ObjReplace(pFanout, pConst0);
                }
            }
        }
    }
    return pDupNtk;
}

void replace_not_with_nand() {
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    Abc_Obj_t* pNode;
    int i;
    Mio_Gate_t* inv = Mio_LibraryReadInv((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* nand2 = Mio_LibraryReadNand2((Mio_Library_t *)Abc_FrameReadLibGen());
    Abc_NtkForEachNode(pNtk, pNode, i) {
        if (pNode->pData == inv) {
            Abc_Obj_t* pTemp = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
            Abc_ObjAddFanin(pTemp, Abc_ObjFanin0(pNode));
            Abc_ObjAddFanin(pTemp, Abc_ObjFanin0(pNode));
            pTemp->pData = nand2;
            Abc_ObjReplace(pNode, pTemp);
        }
    }
}
