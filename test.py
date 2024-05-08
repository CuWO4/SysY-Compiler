#!/usr/bin/env python3

# script for testing in docker `maxxing/compiler-dev`

import sys
import os

class UnknownArgument(Exception):
    def __init__(self, argument):
        self.argument = argument
        
    def __str__(self):
        return f"unknown argument `{self.argument}`"

def call_command(stage, target_lang, debug_flag):
    if target_lang == "":
        call_command(stage, "koopa", debug_flag)
        call_command(stage, "riscv", debug_flag)
        return
    
    if stage == "hello":
        print("make")
        os.system("make")
        target_lang_ext = "koopa" if target_lang == "koopa" else "S"
        debug_flag_string = " ".join(debug_flag)

        command = f"./build/compiler -{target_lang} ./testcases/hello/hello.c " \
            f"-o ./testcases/hello/hello.{target_lang_ext} {debug_flag_string}"
    else:
        stage_string = f"-s {stage}" if stage else ""
        command = f"autotest -t /root/compiler/testcases/testcases " \
            f"-{target_lang} {stage_string} /root/compiler"
        
    print(command)
    os.system(command)
        
def parse_arguments(args):
    stage = ""
    target_lang = ""
    debug_flag = []
    
    for arg in args:
        if arg.startswith("lv") or arg == "perf" or arg == "hello" or arg == "all":
            stage = arg
        elif arg == "koopa" or arg == "riscv" or arg == "test":
            target_lang = arg
        elif arg == "-dbg-k" or arg == "-dbg-r":
            debug_flag.append(arg)
        else:
            raise UnknownArgument(arg)
    
    return stage, target_lang, debug_flag

if __name__ == "__main__":
    try:
        stage, target_lang, debug_flag = parse_arguments(sys.argv[1:])
        call_command(stage, target_lang, debug_flag)
    except UnknownArgument as e:
        print(e)
    
    