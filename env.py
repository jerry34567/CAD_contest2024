##
# @file env.py
# @author Keren Zhu
# @date 10/25/2019
# @brief The environment classes
#

import abc_py as abcPy
import numpy as np
import graphExtractor as GE
import torch
from dgl.nn.pytorch import GraphConv
import dgl

class EnvGraph(object):
    """
    @brief the overall concept of environment, the different. use the compress2rs as target
    """
    def __init__(self, aigfile, libfile):
        self._abc = abcPy.AbcInterface()
        self._aigfile = aigfile
        self._libfile = libfile
        self._abc.start()
        self.lenSeq = 0
        self._abc.read_lib(self._libfile)
        self._abc.read(self._aigfile)
        initAigStats = self._abc.aigStats() # The initial AIG statistics
        self._abc.backup()
        self._abc.map()
        initNetListStats = self._abc.netlistStats()
        self.initArea = float(initNetListStats.area)
        self.initDelay = float(initNetListStats.delay)
        self.initNumAnd = float(initAigStats.numAnd)
        self.initAigLev = float(initAigStats.lev)
        self.initNumNode = float(initNetListStats.node)
        self.initNumEdge = float(initNetListStats.edge)
        self.initNetLev = float(initNetListStats.lev)
        self._abc.restore()
        self.compress2rs() # run a compress2rs as target
        targetAigStats = self._abc.aigStats()
        self._abc.map()
        targetNetListStats = self._abc.netlistStats()
        totalReward = self.statValue(initNetListStats) - self.statValue(targetNetListStats)
        self._rewardBaseline = totalReward / 18.0 # 18 is the length of compress2rs sequence
        print("baseline area ", targetNetListStats.area, "baseline delay ", targetNetListStats.delay, " total reward ", totalReward )
    def resyn2(self):
        self._abc.balance(l=False)
        self._abc.rewrite(l=False)
        self._abc.refactor(l=False)
        self._abc.balance(l=False)
        self._abc.rewrite(l=False)
        self._abc.rewrite(l=False, z=True)
        self._abc.balance(l=False)
        self._abc.refactor(l=False, z=True)
        self._abc.rewrite(l=False, z=True)
        self._abc.balance(l=False)
    def reset(self):
        self.lenSeq = 0
        self._abc.end()
        self._abc.start()
        self._abc.read_lib(self._libfile)
        self._abc.read(self._aigfile)
        self._abc.backup()
        self._lastAigStats = self._abc.aigStats() # The initial AIG statistics
        self._curAigStats = self._lastAigStats # the current AIG statistics
        self._abc.map()
        self._lastNetStats = self._abc.netlistStats() # The initial NetList statistics
        self._curNetStats = self._lastNetStats # the current NetList statistics
        self._abc.restore()
        self.lastAct = self.numActions() - 1
        self.lastAct2 = self.numActions() - 1
        self.lastAct3 = self.numActions() - 1
        self.lastAct4 = self.numActions() - 1
        self.actsTaken = np.zeros(self.numActions())
        return self.state()
    def close(self):
        self.reset()
    def step(self, actionIdx):
        self.takeAction(actionIdx)
        nextState = self.state()
        reward = self.reward()
        done = False
        if (self.lenSeq >= 20):
            done = True
        return nextState,reward,done,0
    def takeAction(self, actionIdx):
        """
        @return true: episode is end
        """
        # "b -l; rs -K 6 -l; rw -l; rs -K 6 -N 2 -l; rf -l; rs -K 8 -l; b -l; rs -K 8 -N 2 -l; rw -l; rs -K 10 -l; rwz -l; rs -K      10 -N 2 -l; b -l; rs -K 12 -l; rfz -l; rs -K 12 -N 2 -l; rwz -l; b -l
        self.lastAct4 = self.lastAct3
        self.lastAct3 = self.lastAct2
        self.lastAct2 = self.lastAct
        self.lastAct = actionIdx
        #self.actsTaken[actionIdx] += 1
        self.lenSeq += 1
        """
        # Compress2rs actions
        if actionIdx == 0:
            self._abc.balance(l=True) # b -l
        elif actionIdx == 1:
            self._abc.resub(k=6, l=True) # rs -K 6 -l
        #elif actionIdx == 2:
        #    self._abc.resub(k=6, n=2, l=True) # rs -K 6 -N 2 -l
        #elif actionIdx == 3:
        #    self._abc.resub(k=8, l=True) # rs -K 8 -l
        #elif actionIdx == 4:
        #    self._abc.resub(k=10, l=True) # rs -K 10 -l
        #elif actionIdx == 5:
        #    self._abc.resub(k=12, l=True) # rs -K 12 -l
        #elif actionIdx == 6:
        #    self._abc.resub(k=10, n=2, l=True) # rs -K 10 -N 2 -l
        elif actionIdx == 2:
            self._abc.resub(k=12, n=2, l=True) # rs - K 12 -N 2 -l
        elif actionIdx == 3:
            self._abc.rewrite(l=True) # rw -l
        #elif actionIdx == 3:
        #    self._abc.rewrite(l=True, z=True) # rwz -l
        elif actionIdx == 4:
            self._abc.refactor(l=True) # rf -l
        #elif actionIdx == 4:
        #    self._abc.refactor(l=True, z=True) # rfz -l
        elif actionIdx == 5: # terminal
            self._abc.end()
            return True
        else:
            assert(False)
        """
        if actionIdx == 0:
            self._abc.balance(l=False) # b
        elif actionIdx == 1:
            self._abc.rewrite(l=False) # rw
        elif actionIdx == 2:
            self._abc.refactor(l=False) # rf
        elif actionIdx == 3:
            self._abc.rewrite(l=False, z=True) #rw -z
        elif actionIdx == 4:
            self._abc.refactor(l=False, z=True) #rs
        elif actionIdx == 5:
            self._abc.end()
            return True
        else:
            assert(False)
        """
        elif actionIdx == 3:
            self._abc.rewrite(z=True) #rwz
        elif actionIdx == 4:
            self._abc.refactor(z=True) #rfz
        """


        # update the statitics
        self._lastAigStats = self._curAigStats
        self._curAigStats = self._abc.aigStats()
        self._abc.backup()
        self._abc.map()
        self._lastNetStats = self._curNetStats
        self._curNetStats = self._abc.netlistStats()
        self._abc.restore()
        return False
    def state(self):
        """
        @brief current state
        """
        oneHotAct = np.zeros(self.numActions())
        np.put(oneHotAct, self.lastAct, 1)
        lastOneHotActs  = np.zeros(self.numActions())
        lastOneHotActs[self.lastAct2] += 1/3
        lastOneHotActs[self.lastAct3] += 1/3
        lastOneHotActs[self.lastAct] += 1/3
        stateArray = np.array([self._curAigStats.numAnd / self.initNumAnd, self._curAigStats.lev / self.initAigLev,
            self._curNetStats.area / self.initArea, self._curNetStats.delay / self.initDelay,
            self._lastAigStats.numAnd / self.initNumAnd, self._lastAigStats.lev / self.initAigLev, 
            self._lastNetStats.area / self.initArea, self._lastNetStats.delay / self.initDelay])
        stepArray = np.array([float(self.lenSeq) / 20.0])
        combined = np.concatenate((stateArray, oneHotAct, lastOneHotActs, stepArray), axis=-1)
        #combined = np.expand_dims(combined, axis=0)
        #return stateArray.astype(np.float32)
        combined_torch =  torch.from_numpy(combined.astype(np.float32)).float()
        graph = GE.extract_dgl_graph(self._abc)
        return (combined_torch, graph)
    def reward(self):
        if self.lastAct == 5: #term
            return 0
        return self.statValue(self._lastNetStats) - self.statValue(self._curNetStats) - self._rewardBaseline
        #return -self._lastStats.numAnd + self._curStats.numAnd - 1
        if (self._curStats.numAnd < self._lastStats.numAnd and self._curStats.lev < self._lastStats.lev):
            return 2
        elif (self._curStats.numAnd < self._lastStats.numAnd and self._curStats.lev == self._lastStats.lev):
            return 0
        elif (self._curStats.numAnd == self._lastStats.numAnd and self._curStats.lev < self._lastStats.lev):
            return 1
        else:
            return -2
    def numActions(self):
        return 5 #can revise
    def dimState(self):
        return 8 + self.numActions() * 2 + 1
    def returns(self):
        return [self._curNetStats.area , self._curNetStats.delay]
    def statValue(self, stat):
        return float(stat.area)  / float(self.initArea) + float(stat.delay) / float(self.initDelay) # to be finetune
        return float(stat.numAnd)  / float(self.initNumAnd) #  + float(stat.lev)  / float(self.initLev)
        #return stat.numAnd + stat.lev * 10
    def curStatsValue(self):
        return self.statValue(self._curNetStats)
    def seed(self, sd):
        pass
    def compress2rs(self):
        self._abc.compress2rs()

