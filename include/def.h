#ifndef DEF_H_
#define DEF_H_

extern int block_count;
extern int tmp_var_count;

/*  
 * if debug_mode == true, there would be additional comments in 
 * generated koopa/assembly containing relative information 
 */
extern bool debug_mode_koopa_type;
extern bool debug_mode_koopa_pred_succ;
extern bool debug_mode_riscv;

#endif