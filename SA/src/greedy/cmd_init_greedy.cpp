#include "cmd_init_greedy.h"


extern CostMgr* costMgr;
extern AbcMgr* abcMgr;

vector<string> init_cmd_greedy() {
    vector<string> actions;
    Abc_Ntk_t* pRecord;
    abccmd("strash");
    for (int i = 0; i < 5; ++i) {
        string best_cmd = "strash";
        double best_score = MAXFLOAT;
        // pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        for (int j = 0; j < 31; ++j) {
            pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
            string act = get_command(static_cast<ACTION2>(j));
            if (abccmd(act)) {
                Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
                continue;
            }
            double temp_cost = costMgr->cost_cal(0);

            // cout << "actions: " << act << "  score: " << temp_cost << endl;
            if (temp_cost < best_score) {
                best_cmd = act;
                best_score = temp_cost;
            }
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        }
        // cout << best_cmd << " " << best_score << endl;
        actions.push_back(best_cmd);
        abccmd(best_cmd);
    }
    return actions;
}

vector<string> init_cmd_greedy_turtle() {
    vector<string> actions;
    Abc_Ntk_t* pRecord;
    abccmd("strash");
    for (int i = 0; i < 5; ++i) {
        string best_cmd = "strash";
        double best_score = MAXFLOAT;
        // pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        for (int j = 0; j < 31; ++j) {
            pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
            string act = get_command(static_cast<ACTION2>(j));
            if (abccmd(act)) {
                Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
                continue;
            }
            double temp_cost = costMgr->cost_cal_use_turtle(0,0);

            // cout << "actions: " << act << "  score: " << temp_cost << endl;
            if (temp_cost < best_score) {
                best_cmd = act;
                best_score = temp_cost;
            }
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        }
        // cout << best_cmd << " " << best_score << endl;
        actions.push_back(best_cmd);
        abccmd(best_cmd);
    }
    return actions;
}



