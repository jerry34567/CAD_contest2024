#include "dc.h"

void dc_gen(bool use_same_lib) {
    const char* command1 = "tcsh -c 'source /project/cad/cad50/synopsys/CIC/synthesis.csh; source /project/cad/cad50/synopsys/CIC/lc.csh; lc_shell -f lc.sh;'";
    const char* command2 = "tcsh -c 'source /project/cad/cad50/synopsys/CIC/synthesis.csh; dc_shell-t -f syn.tcl | tee syn.log'";
    if (!use_same_lib)
       int result1 = system(command1);
    int result2 = system(command2);
}