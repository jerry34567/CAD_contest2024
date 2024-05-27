# CAD_contest2024
this is repository of 2024 CAD contest

current we have two method, SA and RL

To execute SA
```
cd SA
make
./bin/cadcontest -library /path/to/lib -cost_function /path/to/cost_function -netlist /path/to/design.v -output /path/to/result.v
```

To execute RL
```
python testReinforce.py -library /path/to/lib -cost_function /path/to/cost_function -netlist /path/to/design.v -output /path/to/result.v
```