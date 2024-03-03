#ifndef DEF_H_
#define DEF_H_

#ifndef extern_
    extern int block_count;
    extern int tmp_var_count;

    /*  
    * if debug_mode == true, there would be additional comments in 
    * generated koopa/assembly containing relative information 
    */
    extern bool debug_mode_koopa_type;
    extern bool debug_mode_koopa_pred_succ;
    extern bool debug_mode_riscv;

#else
    int block_count = 0;
    int tmp_var_count = 0;

    bool debug_mode_koopa_type = false;
    bool debug_mode_koopa_pred_succ = false;
    bool debug_mode_riscv = false;
#endif

#endif