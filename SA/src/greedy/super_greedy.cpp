#include "super_greedy.h"
#include "util.h"

extern CostMgr* costMgr;
extern AbcSuperMgr* abcSuperMgr;
extern my_RandomNumGen Rand;

void super_greedy(double& best_cost, bool buf_flag){
    Map_SuperLib_t* pLib = (Map_SuperLib_t *)Abc_FrameReadLibSuper();
    double orig_cost = costMgr->cost_cal_use_map(0, buf_flag);
    Map_Super_t** super_array = pLib->ppSupers;
    double cur_best_cost = orig_cost;
    int num_super = pLib->nSupersReal;
    for (int i = 0; i < num_super; i++) {
        cout << "i: " << i << endl;
        Map_Super_t* pGate = super_array[i];
        double record_area = pGate->Area;
        double after_cost;

        pGate->Area = 0.5 * record_area;
        after_cost = costMgr->cost_cal_use_map(0, buf_flag);
        if (after_cost < cur_best_cost) {
            cur_best_cost = after_cost;
            costMgr->cost_cal_use_map(1, buf_flag);
            cout << "cost: " << cur_best_cost << endl;
            continue;
        }

        pGate->Area = 2 * record_area;
        after_cost = costMgr->cost_cal_use_map(0, buf_flag);
        if (after_cost < cur_best_cost) {
            cur_best_cost = after_cost;
            costMgr->cost_cal_use_map(1, buf_flag);
            cout << "cost: " << cur_best_cost << endl;
            continue;
        }
        pGate->Area = record_area;
    }
    best_cost = cur_best_cost;
}

