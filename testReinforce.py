##
# @file testReinforce.py
# @author Keren Zhu
# @date 10/31/2019
# @brief The main for test REINFORCE
#

from datetime import datetime
import os

import reinforce as RF
from env import EnvGraph as Env

import numpy as np
import statistics


class AbcReturn:
    def __init__(self, returns):
        self.area = float(returns[0])
        self.delay = float(returns[1])
    def __lt__(self, other):
        return self.area * self.delay < other.area * other.delay
        if (int(self.delay) == int(other.delay)):
            return self.area < other.area
        else:
            return self.delay < other.delay
    def __eq__(self, other):
        return self.area * self.delay == other.area * other.delay
        return int(self.level) == int(other.level) and int(self.numNodes) == int(self.numNodes)

def testReinforce(filename, libfilename, ben):
    now = datetime.now()
    dateTime = now.strftime("%m/%d/%Y, %H:%M:%S") + "\n"
    print("Time ", dateTime)
    env = Env(filename, libfilename)
    #vApprox = Linear(env.dimState(), env.numActions())
    vApprox = RF.PiApprox(env.dimState(), env.numActions(), 8e-4, RF.FcModelGraph)
    baseline = RF.Baseline(0)
    vbaseline = RF.BaselineVApprox(env.dimState(), 3e-3, RF.FcModel)
    reinforce = RF.Reinforce(env, 0.9, vApprox, vbaseline)

    lastfive = []
    min_value = float('inf')
    for idx in range(200):
        returns = reinforce.episode(phaseTrain=True)
        if min_value > reinforce.memTrajectory[0].value :
            actions = reinforce.memTrajectory[0].actions
            min_value = reinforce.memTrajectory[0].value
            record_return = returns
        seqLen = reinforce.lenSeq
        line = "iter " + str(idx) + " returns "+ str(returns) + " seq Length " + str(seqLen) + "\n"
        if idx >= 195:
            lastfive.append(AbcReturn(returns))
        print(line)
        # 2014 * 12.2
        #reinforce.replay()
    resultName = "./results/" + ben + ".csv"
    print(actions)
    print(record_return)
    print(min_value)
    print(reinforce.memTrajectory[0].value)
    print(reinforce.memTrajectory[1].value)
    print(reinforce.memTrajectory[2].value)
    print(reinforce.memTrajectory[3].value)
    print(reinforce.memTrajectory[4].value)
    #lastfive.sort(key=lambda x : x.level)
    lastfive = sorted(lastfive)
    with open(resultName, 'a') as andLog:
        line = ""
        line += str(lastfive[0].area)
        line += " "
        line += str(lastfive[0].delay)
        line += "\n"
        andLog.write(line)
    rewards = reinforce.sumRewards
    """
    with open('./results/sum_rewards.csv', 'a') as rewardLog:
        line = ""
        for idx in range(len(rewards)):
            line += str(rewards[idx]) 
            if idx != len(rewards) - 1:
                line += ","
        line += "\n"
        rewardLog.write(line)
    with open ('./results/converge.csv', 'a') as convergeLog:
        line = ""
        returns = reinforce.episode(phaseTrain=False)
        line += str(returns[0])
        line += ","
        line += str(returns[1])
        line += "\n"
        convergeLog.write(line)
    """




if __name__ == "__main__":
    """
    env = Env("./bench/i10.aig")
    vbaseline = RF.BaselineVApprox(4, 3e-3, RF.FcModel)
    for i in range(10000000):
        with open('log', 'a', 0) as outLog:
            line = "iter  "+ str(i) + "\n"
            outLog.write(line)
        vbaseline.update(np.array([2675.0 / 2675, 50.0 / 50, 2675. / 2675, 50.0 / 50]), 422.5518 / 2675)
        vbaseline.update(np.array([2282. / 2675,   47. / 50, 2675. / 2675,   47. / 50]), 29.8503 / 2675)
        vbaseline.update(np.array([2264. / 2675,   45. / 50, 2282. / 2675,   45. / 50]), 11.97 / 2675)
        vbaseline.update(np.array([2255. / 2675,   44. / 50, 2264. / 2675,   44. / 50]), 3 / 2675)
    """
    testReinforce("./benchmarks/random_control/i2c.blif", "./library/mcnc.genlib", "i2c")
    # testReinforce("./bench/MCNC/Combinational/blif/prom1.blif", "prom1")
    # testReinforce("./bench/MCNC/Combinational/blif/mainpla.blif", "mainpla")
    # testReinforce("./bench/MCNC/Combinational/blif/k2.blif", "k2")
    # testReinforce("./bench/ISCAS/blif/c5315.blif", "c5315")
    # testReinforce("./bench/ISCAS/blif/c6288.blif", "c6288")
    # testReinforce("./bench/MCNC/Combinational/blif/apex1.blif", "apex1")
    # testReinforce("./bench/MCNC/Combinational/blif/bc0.blif", "bc0")
    #testReinforce("./bench/i10.aig", "i10")
    #testReinforce("./bench/ISCAS/blif/c1355.blif", "c1355")
    #testReinforce("./bench/ISCAS/blif/c7552.blif", "c7552")
