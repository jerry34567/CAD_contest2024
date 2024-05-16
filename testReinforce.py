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

import argparse
import parser as ps

class AbcReturn:
    def __init__(self, returns):
        self.returns = returns
        # self.area = float(returns[0])
        # self.delay = float(returns[1])
    def __lt__(self, other):
        return self.returns < other.returns
        return self.area * self.delay < other.area * other.delay
        if (int(self.delay) == int(other.delay)):
            return self.area < other.area
        else:
            return self.delay < other.delay
    def __eq__(self, other):
        return self.returns == other.returns
        return self.area * self.delay == other.area * other.delay
        return int(self.level) == int(other.level) and int(self.numNodes) == int(self.numNodes)

def testReinforce(cost_function_name, filename, libfilename, genlibname, output):
    now = datetime.now()
    dateTime = now.strftime("%m/%d/%Y, %H:%M:%S") + "\n"
    print("Time ", dateTime)
    env = Env(filename, libfilename, cost_function_name, genlibname, output)
    #vApprox = Linear(env.dimState(), env.numActions())
    vApprox = RF.PiApprox(env.dimState(), env.numActions(), 8e-4, RF.FcModelGraph)
    baseline = RF.Baseline(0)
    vbaseline = RF.BaselineVApprox(env.dimState(), 3e-3, RF.FcModel)
    reinforce = RF.Reinforce(env, 0.9, vApprox, vbaseline)

    lastfive = []
    min_value = float('inf')
    for idx in range(200):
        returns = reinforce.episode(phaseTrain=True)
        # print((returns))
        if min_value > reinforce.memTrajectory[0].value :
            actions = reinforce.memTrajectory[0].actions
            min_value = reinforce.memTrajectory[0].value
            record_return = returns
        seqLen = reinforce.lenSeq
        line = "iter " + str(idx) + " returns "+ str(returns) + " seq Length " + str(seqLen) + "\n"
        if idx >= 195:
            lastfive.append(AbcReturn(returns))
        print(line)
        #reinforce.replay()
    resultName = "./results/" + output + ".csv"
    #lastfive.sort(key=lambda x : x.level)
    lastfive = sorted(lastfive)
    with open(resultName, 'a') as andLog:
        line = ""
        line += str(lastfive[0].returns)
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
    argument = argparse.ArgumentParser()
    argument.add_argument("-cost_function", type=str)
    argument.add_argument("-library", type=str)
    argument.add_argument("-netlist", type=str)
    argument.add_argument("-output", type=str)
    args_ = argument.parse_args()
    ps.parser(args_.library)
    testReinforce(args_.cost_function, args_.netlist, args_.library, "./contest.genlib", args_.output)
