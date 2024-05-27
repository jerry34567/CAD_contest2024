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
import parse as ps

import abc_py as abcPy
import subprocess

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
    
def call_abc(abc, action):
    if action == 0:
        abc.balance(l=False) # b
        # print("balance")
    elif action == 1:
        abc.rewrite(l=False) # rw
        # print("rewrite")
    elif action == 2:
        abc.refactor(l=False) # rf
        # print("rf")
    elif action == 3:
        abc.rewrite(l=False, z=True) #rw -z
        # print("rw -z")
    elif action == 4:
        abc.refactor(l=False, z=True) #rf -z
    elif action == 5:
        abc.resub(l=False) # rs
        # print("rs")
    elif action == 6:
        abc.orchestrate(n=3)
        # print("orchestrate")
    elif action == 7:
        abc.ifraig()
        # print("ifraig")
    elif action == 8:
        abc.dc2()
        # print("dc2")
    return

def greedy(cost_function_name, filename, libfilename, genlibname, output):
    abc = abcPy.AbcInterface()
    abc.start()
    abc.read_lib(genlibname)
    abc.read(filename)
    total_min = float('inf')
    for i in range(20):
        min_value = float('inf')
        record = 0
        for action in range(9):
            abc.backup()
            call_abc(abc,action)
            abc.abccmd("&get -n")
            abc.abccmd("&nf")
            abc.abccmd("&put")
            abc.abccmd("mfs3 -ae -I 4 -O 2")
            abc.write_verilog(output)
            abc.restore()
            exe_path = cost_function_name
            args = ["-library", libfilename, "-netlist", output, "-output" , "temp"]
            process = subprocess.Popen([exe_path] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            stdout, _ = process.communicate()
            a = stdout.decode()
            cost = a.split('=')[1].strip()
            if (min_value > float(cost)):
                min_value = float(cost)
                record = action
        call_abc(abc, record)
        print(record)
        if (total_min == min_value):
            break
        record_min = min_value
        if (total_min > record_min):
            total_min = record_min
    print(total_min)
    abc.end()
    return

            

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

    resultName = "./results/" + "result.txt"
    lastfive = []
    min_value = float('inf')
    for idx in range(200):
        returns = reinforce.episode(phaseTrain=True)
        # print((returns))
        if min_value > reinforce.memTrajectory[0].value :
            actions = reinforce.memTrajectory[0].actions
            min_value = reinforce.memTrajectory[0].value

            with open(resultName, 'w') as andLog:
                andLog.write("best result: ")
                andLog.write(str(min_value))
                andLog.write("\n")
                for action in actions:
                    act = "temp"
                    if action == 0:
                        act = "b" 
                    elif action == 1:
                        act = "rw" 
                    elif action == 2:
                        act = "rf" 
                    elif action == 3:
                        act = "rwz" 
                    elif action == 4:
                        act = "rfz" 
                    elif action == 5:
                        act = "rs"
                    elif action == 6:
                        act = "orchestrate" 
                    elif action == 7:
                        act = "ifraig" 
                    elif action == 8:
                        act = "dc2" 
                    andLog.write(act)
                    andLog.write("\n")
            # record_return = returns
        seqLen = reinforce.lenSeq
        line = "iter " + str(idx) + " returns "+ str(returns) + " seq Length " + str(seqLen) + "\n"
        if idx >= 195:
            lastfive.append(AbcReturn(returns))
        print(line)
        #reinforce.replay()
    print("best result")
    print(min_value)
    #lastfive.sort(key=lambda x : x.level)
    lastfive = sorted(lastfive)
    
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
    ps.parser(args_.library, args_.netlist, args_.cost_function)
    testReinforce(args_.cost_function, args_.netlist, args_.library, "./contest.genlib", args_.output)
    # greedy(args_.cost_function, args_.netlist, args_.library, "./contest.genlib", args_.output)
