#ifndef DEF_H_
#define DEF_H_

#ifndef extern_
#define extern_ extern
#endif

/*  
 * if debug_mode == true, there would be additional comments in 
 * generated koopa/assembly containing relative information 
 */
extern_ bool debug_mode_koopa;
extern_ bool debug_mode_riscv;

#endif