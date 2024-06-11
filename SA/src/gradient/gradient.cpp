// #include "gradient.h"

// // 使用中心差分法近似梯度
// map<string, double> approximateGradient(map<string, pair<string, float>> temp_dic, map<string, vector<float>>& timing_dic, const string& lib_file, const string& cost_exe, double h = 1e-5) {
//     map<string, double> grad;
//     double avg = 0;
//     double grad_avg = 0;
//     for (auto& cell : temp_dic) {
//         avg += cell.second.second;
//         double temp = cell.second.second * 0.01;
//         cell.second.second += temp;
//         cout << cell.second.second << endl;
//         write_genlib("./temp.genlib", temp_dic, timing_dic);
//         abccmd("./temp.genlib");
//         double x_plus = cost_cal(lib_file, cost_exe, "temp.v");
//         cell.second.second -= 2*temp;
//         cout << cell.second.second << endl;
//         write_genlib("./temp.genlib", temp_dic, timing_dic);
//         abccmd("./temp.genlib");
//         double x_minus = cost_cal(lib_file, cost_exe, "temp.v");
//         cell.second.second += temp;
//         grad[cell.first] = x_plus - x_minus;
//         grad_avg += grad[cell.first];
//         cout << setprecision(7) << x_plus << " " << setprecision(7) << x_minus << endl;
//         cout << "grad: " << setprecision(7) << grad[cell.first] << endl;
//     }
//     avg /= 8;
//     grad_avg /= 8;
//     for (auto& tmp : grad) {
//         tmp.second *= avg;
//         tmp.second /= grad_avg;
//     }

//     return grad;
// }

// // 梯度下降算法
// void gradientDescent(double learning_rate, int iterations, map<string, pair<string, float>>& temp_dic, map<string, vector<float>>& timing_dic, const string& lib_file, const string& cost_exe, const string& output_file, double& best_cost) {
//     for(int i = 0; i < iterations; i++) {
//         map<string, double> grad = approximateGradient(temp_dic, timing_dic, lib_file, cost_exe, 1e-3);
//         for (auto& cell : temp_dic) {
//             cell.second.second -= learning_rate * grad[cell.first]; // 使用近似梯度更新x
//             cout << cell.first << " diff: " << learning_rate * grad[cell.first] << " now: " << cell.second.second << endl;
//         }
//         write_genlib("./temp.genlib", temp_dic, timing_dic);
//         double cur_cost = cost_cal(lib_file, cost_exe, "temp.v");
//         cout << "cur_cost: " << cur_cost << endl;
//         if (cur_cost < best_cost) {
//             cost_cal(lib_file, cost_exe, output_file);
//             best_cost = cur_cost;
//             write_genlib("./contest.genlib", temp_dic, timing_dic);
//             write_liberty(temp_dic);
//         }
//     }
//     abccmd("read ./contest.genlib");
// }