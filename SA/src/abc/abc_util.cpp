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

void add_const_on_pi() {
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    Abc_Obj_t* pNode;
    int i;

//   nand_5 g1  (.A(n2),.B(n2),.Y(ap)); // pTemp
//   or_6 g2  (.A(ap),.B(ap),.Y(t4));   pTemp3
//   or_6 g3  (.A(t4),.B(t4),.Y(t5));   pTemp4
//   nand_5 g4  (.A(t5),.B(t5),.Y(t6)); 5
//   nand_5 g5  (.A(t6),.B(t6),.Y(t7)); 6

//   or_6  g6  (.A(n2),.B(n2),.Y(t0));   //  pTemp2
//   or_6 g7  (.A(t0),.B(t0),.Y(t1));    7
//   nand_5 g8  (.A(t1),.B(t1),.Y(t2));     8
//   nand_5 g9  (.A(t2),.B(t2),.Y(t3));     9
//   or_6 g0   (.A(t3),.B(t3),.Y(t04));     10


//   and_6 g67 (.A(t7),.B(t04),.Y(t8));


    Abc_Obj_t* pi = Abc_NtkPi(pNtk, 0);
    Mio_Gate_t* nand2 = Mio_LibraryReadNand2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* and2 = Mio_LibraryReadAnd2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* nor2 = Mio_LibraryReadNor2((Mio_Library_t *)Abc_FrameReadLibGen());
    Mio_Gate_t* or2 = Mio_LibraryReadOr2((Mio_Library_t *)Abc_FrameReadLibGen());

    Abc_Obj_t* pTemp12 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    Abc_ObjTransferFanout(pi, pTemp12);
    pTemp12->pData = or2;

    Abc_Obj_t* pTemp = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp->pData = nand2;
    Abc_ObjAddFanin(pTemp, pi);
    Abc_ObjAddFanin(pTemp, pi);

    Abc_Obj_t* pTemp2 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp2->pData = or2;
    Abc_ObjAddFanin(pTemp2, pi);
    Abc_ObjAddFanin(pTemp2, pi);

    Abc_Obj_t* pTemp3 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp3->pData = or2;
    Abc_ObjAddFanin(pTemp3, pTemp);
    Abc_ObjAddFanin(pTemp3, pTemp);

    Abc_Obj_t* pTemp4 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp4->pData = or2;
    Abc_ObjAddFanin(pTemp4, pTemp3);
    Abc_ObjAddFanin(pTemp4, pTemp3);

    Abc_Obj_t* pTemp5 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp5->pData = nand2;
    Abc_ObjAddFanin(pTemp5, pTemp4);
    Abc_ObjAddFanin(pTemp5, pTemp4);

    Abc_Obj_t* pTemp6 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp6->pData = nand2;
    Abc_ObjAddFanin(pTemp6, pTemp5);
    Abc_ObjAddFanin(pTemp6, pTemp5);

    Abc_Obj_t* pTemp16 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp16->pData = or2;
    Abc_ObjAddFanin(pTemp16, pTemp6);
    Abc_ObjAddFanin(pTemp16, pTemp6);

    Abc_Obj_t* pTemp7 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp7->pData = or2;
    Abc_ObjAddFanin(pTemp7, pTemp2);
    Abc_ObjAddFanin(pTemp7, pTemp2);

    Abc_Obj_t* pTemp8 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp8->pData = nand2;
    Abc_ObjAddFanin(pTemp8, pTemp7);
    Abc_ObjAddFanin(pTemp8, pTemp7);

    Abc_Obj_t* pTemp9 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp9->pData = nand2;
    Abc_ObjAddFanin(pTemp9, pTemp8);
    Abc_ObjAddFanin(pTemp9, pTemp8);

    Abc_Obj_t* pTemp10 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp10->pData = or2;
    Abc_ObjAddFanin(pTemp10, pTemp9);
    Abc_ObjAddFanin(pTemp10, pTemp9); 

    Abc_Obj_t* pTemp14 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp14->pData = or2;
    Abc_ObjAddFanin(pTemp14, pTemp10);
    Abc_ObjAddFanin(pTemp14, pTemp10); 

    Abc_Obj_t* pTemp11 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
    pTemp11->pData = and2;
    Abc_ObjAddFanin(pTemp11, pTemp16);
    Abc_ObjAddFanin(pTemp11, pTemp14); 

    Abc_ObjAddFanin(pTemp12, pTemp11);
    Abc_ObjAddFanin(pTemp12, pi);
    
    Abc_NtkForEachPi(pNtk, pNode, i) {
        if (i == 0) continue;
        Abc_Obj_t* pTemp13 = Abc_NtkCreateObj(pNtk, ABC_OBJ_NODE);
        Abc_ObjTransferFanout(pNode, pTemp13);
        pTemp13->pData = or2;
        Abc_ObjAddFanin(pTemp13, pTemp11);
        Abc_ObjAddFanin(pTemp13, pNode);
    }
}

void generate_abcrc() {
    std::ofstream file("abc.rc");

    file << "# global parameters\n"
         << "set check         # checks intermediate networks\n"
         << "#set checkfio      # prints warnings when fanins/fanouts are duplicated\n"
         << "#unset checkread   # does not check new networks after reading from file\n"
         << "#set backup        # saves backup networks retrived by \"undo\" and \"recall\"\n"
         << "#set savesteps 1   # sets the maximum number of backup networks to save\n"
         << "#set progressbar   # display the progress bar\n\n"

         << "# program names for internal calls\n"
         << "set dotwin dot.exe\n"
         << "set dotunix dot\n"
         << "set gsviewwin gsview32.exe\n"
         << "set gsviewunix gv\n"
         << "set siswin sis.exe\n"
         << "set sisunix sis\n"
         << "set mvsiswin mvsis.exe\n"
         << "set mvsisunix mvsis\n"
         << "set capowin MetaPl-Capo10.1-Win32.exe\n"
         << "set capounix MetaPl-Capo10.1\n"
         << "set gnuplotwin wgnuplot.exe\n"
         << "set gnuplotunix gnuplot\n\n"

         << "# Niklas Een's commands\n"
         << "#load_plugin C:\\_projects\\abc\\lib\\bip_win.exe \"BIP\"\n\n"

         << "# standard aliases\n"
         << "alias hi history\n"
         << "alias b balance\n"
         << "alias cg clockgate\n"
         << "alias cl cleanup\n"
         << "alias clp collapse\n"
         << "alias cs care_set\n"
         << "alias el eliminate\n"
         << "alias esd ext_seq_dcs\n"
         << "alias f fraig\n"
         << "alias fs fraig_sweep\n"
         << "alias fsto fraig_store\n"
         << "alias fres fraig_restore\n"
         << "alias fr fretime\n"
         << "alias ft fraig_trust\n"
         << "alias ic indcut\n"
         << "alias lp lutpack\n"
         << "alias pcon print_cone\n"
         << "alias pd print_dsd\n"
         << "alias pex print_exdc -d\n"
         << "alias pf print_factor\n"
         << "alias pfan print_fanio\n"
         << "alias pg print_gates\n"
         << "alias pl print_level\n"
         << "alias plat print_latch\n"
         << "alias pio print_io\n"
         << "alias pk print_kmap\n"
         << "alias pm print_miter\n"
         << "alias ps print_stats\n"
         << "alias psb print_stats -b\n"
         << "alias psu print_supp\n"
         << "alias psy print_symm\n"
         << "alias pun print_unate\n"
         << "alias q quit\n"
         << "alias r read\n"
         << "alias ra read_aiger\n"
         << "alias r3 retime -M 3\n"
         << "alias r3f retime -M 3 -f\n"
         << "alias r3b retime -M 3 -b\n"
         << "alias ren renode\n"
         << "alias rh read_hie\n"
         << "alias ri read_init\n"
         << "alias rl read_blif\n"
         << "alias rb read_bench\n"
         << "alias ret retime\n"
         << "alias dret dretime\n"
         << "alias rp read_pla\n"
         << "alias rt read_truth\n"
         << "alias rv read_verilog\n"
         << "alias rvl read_verlib\n"
         << "alias rsup read_super mcnc5_old.super\n"
         << "alias rlib read_library\n"
         << "alias rlibc read_library cadence.genlib\n"
         << "alias rty read_liberty\n"
         << "alias rlut read_lut\n"
         << "alias rw rewrite\n"
         << "alias rwz rewrite -z\n"
         << "alias rf refactor\n"
         << "alias rfz refactor -z\n"
         << "alias re restructure\n"
         << "alias rez restructure -z\n"
         << "alias rs resub\n"
         << "alias rsz resub -z\n"
         << "alias sa set autoexec ps\n"
         << "alias scl scleanup\n"
         << "alias sif if -s\n"
         << "alias so source -x\n"
         << "alias st strash\n"
         << "alias sw sweep\n"
         << "alias ssw ssweep\n"
         << "alias tr0 trace_start\n"
         << "alias tr1 trace_check\n"
         << "alias trt \"r c.blif; st; tr0; b; tr1\"\n"
         << "alias u undo\n"
         << "alias w write\n"
         << "alias wa write_aiger\n"
         << "alias wb write_bench\n"
         << "alias wc write_cnf\n"
         << "alias wh write_hie\n"
         << "alias wl write_blif\n"
         << "alias wp write_pla\n"
         << "alias wv write_verilog\n\n"

         << "# standard scripts\n"
         << "alias resyn       \"b; rw; rwz; b; rwz; b\"\n"
         << "alias resyn2      \"b; rw; rf; b; rw; rwz; b; rfz; rwz; b\"\n"
         << "alias resyn2a     \"b; rw; b; rw; rwz; b; rwz; b\"\n"
         << "alias resyn3      \"b; rs; rs -K 6; b; rsz; rsz -K 6; b; rsz -K 5; b\"\n"
         << "alias compress    \"b -l; rw -l; rwz -l; b -l; rwz -l; b -l\"\n"
         << "alias compress2   \"b -l; rw -l; rf -l; b -l; rw -l; rwz -l; b -l; rfz -l; rwz -l; b -l\"\n"
         << "alias choice      \"fraig_store; resyn; fraig_store; resyn2; fraig_store; fraig_restore\"\n"
         << "alias choice2     \"fraig_store; balance; fraig_store; resyn; fraig_store; resyn2; fraig_store; resyn2; fraig_store; fraig_restore\"\n"
         << "alias rwsat       \"st; rw -l; b -l; rw -l; rf -l\"\n"
         << "alias drwsat2     \"st; drw; b -l; drw; drf; ifraig -C 20; drw; b -l; drw; drf\"\n"
         << "alias share       \"st; multi -m; sop; fx; resyn2\"\n"
         << "alias addinit     \"read_init; undc; strash; zero\"\n"
         << "alias blif2aig    \"undc; strash; zero\"\n"
         << "alias v2p         \"&vta_gla; &ps; &gla_derive; &put; w 1.aig; pdr -v\"\n"
         << "alias g2p         \"&ps; &gla_derive; &put; w 2.aig; pdr -v\"\n"
         << "alias &sw_        \"&put; sweep; st; &get\"\n"
         << "alias &fx_        \"&put; sweep; sop; fx; st; &get\"\n"
         << "alias &dc3        \"&b; &jf -K 6; &b; &jf -K 4; &b\"\n"
         << "alias &dc4        \"&b; &jf -K 7; &fx; &b; &jf -K 5; &fx; &b\"\n\n"

         << "# resubstitution scripts for the IWLS paper\n"
         << "alias src_rw      \"st; rw -l; rwz -l; rwz -l\"\n"
         << "alias src_rs      \"st; rs -K 6 -N 2 -l; rs -K 9 -N 2 -l; rs -K 12 -N 2 -l\"\n"
         << "alias src_rws     \"st; rw -l; rs -K 6 -N 2 -l; rwz -l; rs -K 9 -N 2 -l; rwz -l; rs -K 12 -N 2 -l\"\n"
         << "alias resyn2rs    \"b; rs -K 6; rw; rs -K 6 -N 2; rf; rs -K 8; b; rs -K 8 -N 2; rw; rs -K 10; rwz; rs -K 10 -N 2; b; rs -K 12; rfz; rs -K 12 -N 2; rwz; b\"\n"
         << "alias r2rs        \"b; rs -K 6; rw; rs -K 6 -N 2; rf; rs -K 8; b; rs -K 8 -N 2; rw; rs -K 10; rwz; rs -K 10 -N 2; b; rs -K 12; rfz; rs -K 12 -N 2; rwz; b\"\n"
         << "alias compress2rs \"b -l; rs -K 6 -l; rw -l; rs -K 6 -N 2 -l; rf -l; rs -K 8 -l; b -l; rs -K 8 -N 2 -l; rw -l; rs -K 10 -l; rwz -l; rs -K 10 -N 2 -l; b -l; rs -K 12 -l; rfz -l; rs -K 12 -N 2 -l; rwz -l; b -l\"\n"
         << "alias c2rs        \"b -l; rs -K 6 -l; rw -l; rs -K 6 -N 2 -l; rf -l; rs -K 8 -l; b -l; rs -K 8 -N 2 -l; rw -l; rs -K 10 -l; rwz -l; rs -K 10 -N 2 -l; b -l; rs -K 12 -l; rfz -l; rs -K 12 -N 2 -l; rwz -l; b -l\"\n\n"

         << "# use this script to convert 1-valued and DC-valued flops for an AIG\n"
         << "alias fix_aig     \"logic; undc; strash; zero\"\n\n"

         << "# use this script to convert 1-valued and DC-valued flops for a logic network coming from BLIF\n"
         << "alias fix_blif    \"undc; strash; zero\"\n\n"

         << "# lazy man's synthesis\n"
         << "alias recadd3     \"st; rec_add3; b; rec_add3; dc2; rec_add3; if -K 8; bidec; st; rec_add3; dc2; rec_add3; if -g -K 6; st; rec_add3\"\n";

    file.close();
}