#include "cmd_sa_new.h"
#include "util.h"
#include "cost.h"

// extern RandomNumGen  rnGen;
extern my_RandomNumGen Rand;
extern CostMgr* costMgr;

INST2 get_inst2(){
    int result = Rand() % 5; // number of action
    return  static_cast<INST2>(result);
}

ACTION2 get_action2(){
    int result = Rand() % 31; // number of action
    return  static_cast<ACTION2>(result);
}

string get_command(ACTION2 action2) {
    string act;
    int lRw = Rand() & 1; 
    int zRw = Rand() & 1; 
    int lRf = Rand() & 1; 
    int zRf = Rand() & 1; 
    int lRs = Rand() & 1; 
    int zRs = Rand() & 1; 
    int kRs = ((Rand() & 3) + 1) << 2;
    int nRs = Rand() & 3;
    int lOrch = Rand() & 1; 
    int zOrch = Rand() & 1; 
    int kOrch = ((Rand() & 3) + 1) << 2;
    int nOrch = Rand() & 3;
    int bDc2 = Rand() & 1; 
    int lDc2 = Rand() & 1; 
    int pDc2 = Rand() & 1;
    int lB = Rand() & 1;
    int sShare = Rand() & 1;
    int fShare = Rand() & 1;
    int FShare = 10 + (Rand() % 20);
    int zShare = Rand() & 1;
    int dABC9B = Rand() & 1;
    int aABC9B = Rand() & 1;
    int mABC9BLUT = Rand() & 1;
    int rABC9BLUT = Rand() & 1;
    int aABC9BLUT = Rand() & 1;
    int lABC9DC2  = Rand () & 1;
    int bDRWSAT = Rand() & 1;
    int fDEEPSYN = Rand() & 1;
    int dDEEPSYN = Rand() & 1;
    int aDEEPSYN = Rand() & 1;
    int eDEEPSYN = Rand() & 1;
    int kDEEPSYN = 2 + (Rand() % 7);
    int LDEEPSYN = Rand() % 100;
    int fxDEEPSYN = Rand() & 1;
    int pDCH = Rand() & 1;
    int xDCH = Rand() & 1;
    int aJF = Rand() & 1;
    int eJF = Rand() & 1;
    int mJF = Rand() & 1;
    int DCH = Rand() & 1;

    switch(action2){
        case ACTION2::REWRITE: {
            act = "rewrite";
            act += lRw ? " -l" : "";
            act += zRw ? " -z" : "";
            break;
        }
        case ACTION2::REFACTOR: {
            act = "refactor";
            act += lRf ? " -l" : "";
            act += zRf ? " -z" : "";
            break;
        }
        case ACTION2::RESUB: {
            act = "resub";
            act += lRs ? " -l" : "";
            act += zRs ? " -z" : "";
            act += " -K " + to_string(kRs);
            act += " -N " + to_string(nRs);
            break;
        }
        case ACTION2::ORCHESTRATE: {
            act = "orchestrate";
            act += lOrch ? " -l" : "";
            act += zOrch ? " -z" : "";
            act += " -K " + to_string(kOrch);
            act += " -N " + to_string(nOrch);
            break;
        }
        case ACTION2::IFRAIG: {
            act = "ifraig";
            break;
        }
        case ACTION2::DC2: {
            act = "dc2";
            act += bDc2 ? " -b" : "";
            act += lDc2 ? " -l" : "";
            act += pDc2 ? " -p" : "";
            break;
        }
        case ACTION2::BALANCE: {
            act = "balance";
            act += lB ? " -l" : "";
            break;
        }
        case ACTION2::SHARE: {
            act = "multi -m";
            act += sShare ? " -s" : "";
            act += fShare ? " -f" : "";
            act += " -F " + to_string(FShare);
            act += ";sop; fx";
            act += zShare ? " -z" : "";
            act += ";st;";
            break;
        }
        case ACTION2::ABC9BALANCE: {
            act = "&get -n;";
            act += "&b";
            act += dABC9B ? " -d": "";
            act += aABC9B ? " -a": "";
            act += ";&put;";
            break;
        }
        case ACTION2::ABC9BLUT: {
            act = "&get -n;";
            act += "&blut";
            act += mABC9BLUT ? " -m": "";
            act += rABC9BLUT ? " -r": "";
            act += aABC9BLUT ? " -a": "";
            act += ";&put;";
            break;
        }
        case ACTION2::ABC9DC2: {
            act = "&get -n;";
            act += "&dc2";
            act += lABC9DC2 ? " -l": "";
            act += ";&put;";
            break;
        }
        case ACTION2::ABC9DSDB: {
            act = "&get -n;";
            act += "&dsdb;";
            act += "&put;";
            break;
        }
        case ACTION2::ABC9SOPB: {
            act = "&get -n;";
            if (DCH) {
                act += "&dch ";
                act += fDEEPSYN ? " -f" : "";
                act += pDCH ? " -p" : "";
                act += xDCH ? " -x" : "";
                act += ";";
            }
            act += "&sopb;";
            act += "&put;";
            break;
        }
        case ACTION2::DRWSAT: {
            act = "drwsat";
            act += bDRWSAT ? " -b": "";
            break;
        }
        case ACTION2::ABC9DEEPSYN: {
            act = "&get -n; &dch";
            act += fDEEPSYN ? " -f;" : ";";
            act += "&jf -a -K " + to_string(kDEEPSYN) + "; &mfs ";
            act += dDEEPSYN ? " -d" : "";
            act += aDEEPSYN ? " -a" : "";
            act += eDEEPSYN ? " -e" : "";
            act += " -W 20 -L " + to_string(LDEEPSYN) + ";";
            act += fxDEEPSYN ? "&fx;" : "";
            act += "&st; &put;";
            break;
        }
        case ACTION2::ABC9JF: {
            act = "&get -n; &dch";
            act += fDEEPSYN ? " -f" : "";
            act += pDCH ? " -p" : "";
            act += xDCH ? " -x" : "";
            act += ";&jf";
            act += aJF ? " -a" : "";
            act += eJF ? " -e" : "";
            act += mJF ? " -m" : "";
            act += " -K " + to_string(kDEEPSYN) + ";";
            act += "&st; &put;";
            break;
        }
        case ACTION2::RESYN: {
            act = "resyn";
            break;
        }
        case ACTION2::RESYN2: {
            act = "resyn2";
            break;
        }
        case ACTION2::RESYN2a: {
            act = "resyn2a";
            break;
        }
        case ACTION2::RESYN3: {
            act = "resyn3";
            break;
        }
        case ACTION2::COMPRESS: {
            act = "compress";
            break;
        }
        case ACTION2::COMPRESS2: {
            act = "compress2";
            break;
        }
        case ACTION2::RWSAT: {
            act = "rwsat";
            break;
        }
        case ACTION2::DRWSAT2: {
            act = "drwsat2";
            break;
        }
        case ACTION2::DC3: {
            act = "&get -n; &dc3; &put";
            break;
        }
        case ACTION2::DC4: {
            act = "&get -n; &dc4; &put";
            break;
        }
        case ACTION2::SRCRW: {
            act = "src_rw";
            break;
        }
        case ACTION2::SRCRS: {
            act = "src_rs";
            break;
        }
        case ACTION2::SRCRWS: {
            act = "src_rws";
            break;
        }
        case ACTION2::R2RS: {
            act = "r2rs";
            break;
        }
        case ACTION2::C2RS: {
            act = "c2rs";
            break;
        }
    }
    return act;
}

void revise_cur_actions(vector<string>& cur_action) {
    string act;
    int length = cur_action.size();
    INST2 action = get_inst2();
    if (length == 0) {
        action = INST2::add_action;
    }  
    ACTION2 action2 = get_action2();
    switch(action){
        case INST2::add_action: {
            vector<string>::iterator it;
            if (length == 0) {
                it = cur_action.begin();
            }
            else {
                it = cur_action.begin() + (Rand() % length);
            }
            act = get_command(action2);
            cur_action.insert(it, act);
            break;
        }
        case INST2::delete_action: {
            vector<string>::iterator it = cur_action.begin() + (Rand() % length);
            cur_action.erase(it);
            break;
        }
        case INST2::swap_action: {
            string temp;
            int pos1 = Rand() % length;
            int pos2 = Rand() % length;
            temp = cur_action[pos1];
            cur_action[pos1] = cur_action[pos2];
            cur_action[pos2] = temp;
            break;
        }
        case INST2::move_action: {
            string temp;
            int pos1 = Rand() % length;
            temp = cur_action[pos1];
            vector<string>::iterator it = cur_action.begin() + pos1;
            cur_action.erase(it);
            it = cur_action.begin() + (Rand() % length);
            cur_action.insert(it, temp);
            break;
        }
        case INST2::change_action: {
            act = get_command(action2);
            cur_action[Rand() % length] = act;
            break;
        }
    }
}

vector<string> new_cmd_simulated_annealing(double& low_effort_best_cost, double& best_cost, bool buf_flag, vector<string>& init_actions, map<string, pair<string, double>>& gate_cost_dic){
    Abc_Ntk_t* pRecord;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        if (abccmd(init_actions[i])) {
            vector<string>::iterator it = init_actions.begin() + i;
            init_actions.erase(it);
        }
    }

    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    vector<string> best_action;
    double init_cost = costMgr->cost_cal(0);

    cout << "init: " << init_cost << endl;

    if (init_cost < low_effort_best_cost) {
        low_effort_best_cost = init_cost;
        best_action = cur_action;
        vector<string>::iterator it = best_action.begin();
        best_action.insert(it, "strash");
    }
    if (low_effort_best_cost < best_cost) {
        best_cost = low_effort_best_cost;
        costMgr->change_name();
        if (buf_flag)
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        costMgr->set_best_dic(gate_cost_dic);
    }

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));

        int round = 1 + (Rand() % 4);
        for (int j = 0; j < round; j++) {
            revise_cur_actions(cur_action);
        }

        // cout << "T: " << T << endl;
        bool success = true;
        for (int i = 0; i < cur_action.size(); i++) {
            // cout << cur_action[i] << endl;
            if (abccmd(cur_action[i])) {
                cout << "error!";
                success = false;
                break;
            }
        }
        if (!success) {
            cur_action = temp_action;
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
            continue;
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal(0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX))) / 2;
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < low_effort_best_cost) {
                low_effort_best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }
            if (low_effort_best_cost < best_cost) {
                best_cost = low_effort_best_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
            }

        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}


vector<string> new_cmd_simulated_annealing_using_map(double& best_cost, bool buf_flag, vector<string>& init_actions){
    Abc_Ntk_t* pRecord;
    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        abccmd(init_actions[i]);
    }

    double init_cost = costMgr->cost_cal_use_map(0, buf_flag);
    cout << "init: " << init_cost << endl;

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    vector<string> best_action;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        int length = cur_action.size();
        INST2 action = get_inst2();
        string act;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST2::add_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                int lRw = Rand() & 1; 
                int zRw = Rand() & 1; 
                int lRf = Rand() & 1; 
                int zRf = Rand() & 1; 
                int lRs = Rand() & 1; 
                int zRs = Rand() & 1; 
                int kRs = ((Rand() & 3) + 1) << 2;
                int nRs = Rand() & 3;
                int lOrch = Rand() & 1; 
                int zOrch = Rand() & 1; 
                int kOrch = ((Rand() & 3) + 1) << 2;
                int nOrch = Rand() & 3;
                int bDc2 = Rand() & 1; 
                int lDc2 = Rand() & 1; 
                int lB = Rand() & 1; 
                ACTION2 action2 = get_action2();
                switch(action2){
                    case ACTION2::REWRITE: {
                        act = "rewrite";
                        act += lRw ? " -l" : "";
                        act += zRw ? " -z" : "";
                        break;
                    }
                    case ACTION2::REFACTOR: {
                        act = "refactor";
                        act += lRf ? " -l" : "";
                        act += zRf ? " -z" : "";
                        break;
                    }
                    case ACTION2::RESUB: {
                        act = "resub";
                        act += lRs ? " -l" : "";
                        act += zRs ? " -z" : "";
                        act += " -K " + to_string(kRs);
                        act += " -N " + to_string(nRs);
                        break;
                    }
                    case ACTION2::ORCHESTRATE: {
                        act = "orchestrate";
                        act += lOrch ? " -l" : "";
                        act += zOrch ? " -z" : "";
                        act += " -K " + to_string(kOrch);
                        act += " -N " + to_string(nOrch);
                        break;
                    }
                    case ACTION2::IFRAIG: {
                        act = "ifraig";
                        break;
                    }
                    case ACTION2::DC2: {
                        act = "dc2";
                        act += bDc2 ? " -b" : "";
                        act += lDc2 ? " -l" : "";
                        break;
                    }
                    case ACTION2::BALANCE: {
                        act = "balance";
                        act += lB ? " -l" : "";
                        break;
                    }
                    case ACTION2::SHARE: {
                        act = "multi -m; sop; fx; st;";
                        break;
                    }
                }
                cur_action.insert(it, act);
                break;
            }
            case INST2::delete_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                cur_action.erase(it);
                break;
            }
            case INST2::swap_action: {
                string temp;
                int pos1 = Rand() % length;
                int pos2 = Rand() % length;
                temp = cur_action[pos1];
                cur_action[pos1] = cur_action[pos2];
                cur_action[pos2] = temp;
                break;
            }
            case INST2::move_action: {
                string temp;
                int pos1 = Rand() % length;
                temp = cur_action[pos1];
                vector<string>::iterator it = cur_action.begin() + pos1;
                cur_action.erase(it);
                it = cur_action.begin() + (Rand() % length);
                cur_action.insert(it, temp);
                break;
            }
        }
        for (int i = 0; i < cur_action.size(); i++) {
            abccmd(cur_action[i]);
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_map(0, buf_flag);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < best_cost) {
                costMgr->cost_cal_use_map(1, buf_flag);
                best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }

        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}

vector<string> new_cmd_simulated_annealing_using_turtle(double& low_effort_best_cost, double& best_cost, bool buf_flag, vector<string>& init_actions, map<string, pair<string, double>>& gate_cost_dic){
    Abc_Ntk_t* pRecord;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        if (abccmd(init_actions[i])) {
            vector<string>::iterator it = init_actions.begin() + i;
            init_actions.erase(it);
        }
    }

    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    double init_cost = costMgr->cost_cal_use_turtle(0, 0);
    cout << "init: " << init_cost << endl;

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    vector<string> best_action;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        
        int round = 1 + (Rand() % 4);
        for (int j = 0; j < round; j++) {
            revise_cur_actions(cur_action);
        }

        bool success = true;
        for (int i = 0; i < cur_action.size(); i++) {
            if (abccmd(cur_action[i])) {
                cout << "error!";
                success = false;
                break;
            }
        }
        if (!success) {
            cur_action = temp_action;
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
            continue;
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_turtle(0, 0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX))) / 2;
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < low_effort_best_cost) {
                low_effort_best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }
            if (low_effort_best_cost < best_cost) {
                best_cost = low_effort_best_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
            }
        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}