import json
import subprocess
import warnings

warnings.simplefilter(action='ignore', category=FutureWarning)
#from ggplot import *
from pandas import DataFrame

"""
rel_prec - relative precision
abs_prec - absolute precision
thread_num - number of threads
integ_int - integration interval
iter_num - iteration number
"""
#name of configuration file
CONF_FILE = 'conf.json'
#threads to try
EXEC_NUM = 8
#name of executable
PROG_NAME = 'mary_program'

def set_conf(config):
    #parse configuration file and create argument list of it
    subproc_exec = [None]*10
    subproc_exec[0] = config["rel_prec"]
    subproc_exec[1] = config["abs_prec"]
    subproc_exec[2] = config["thread_num"]
    subproc_exec[3] = config["deltas"][0]
    subproc_exec[4] = config["deltas"][1]
    subproc_exec[5] = config["x_s"][0]
    subproc_exec[6] = config["x_s"][1]
    subproc_exec[7] = config["y_s"][0]
    subproc_exec[8] = config["y_s"][1]
    subproc_exec[9] = config["iter_num"]
    return subproc_exec

def proc_data(out_arr):
    #iterate over data returned from execution, create dataframe object of it
    dataframe_arr = []
    iter_dict = {}
    for i in out_arr:
        iter_dict["result"] = out_arr[0]
        iter_dict["rel"] = out_arr[1]
        iter_dict["abs"] = out_arr[2]
        iter_dict["time"] = out_arr[3]
        iter_dict["thread_num"] = out_arr[4]
        dataframe_arr.append(iter_dict)
    return DataFrame(dataframe_arr)

def main():
    with open(CONF_FILE, 'r') as conf_file:
        config = json.load(conf_file)
    arg_list = set_conf(config)
    for i in range(EXEC_NUM):
        if i > 0:
            prv_res = out_arr[0]
        arg_list[2] = i
        proc = subprocess.Popen([PROG_NAME] + arg_list, stdout=subprocess.PIPE)
        print(arg_list)
        proc_stdout = list(proc.stdout.readlines())
        print(proc_stdout)
        out_arr.append(proc_stdout[-4:])
        out_arr.append(i)
        #check absolute and relative things
        if abs(abs(prv_res) - abs(out_arr[0])) > config["rel_prec"]:
            # print("Abs error bigger")
            continue
        if abs(abs(prv_res) - abs(out_arr[0])) / abs(prv_res) > config["abs_prec"]:
            # print("Rel error bigger")
            continue
        #print error if provided
        if len(proc_stdout) > 4:
            print(proc_stdout[:-4])
    data = proc_data(out_arr)
    min_time = min(orig_df['Close'])
    '''#draws ggplot graph
    try:
        ggplot(aes(x='thread_num', y='thread_num'), data=data) +\
                    geom_line() +\
                        stat_smooth(colour='blue', span=0.2)
    except Exception as e:
        print("You probably doesn't have ggplot or some dependancies installed. Check error below")
        print(e)'''

if __name__ == "__main__":
    main()
