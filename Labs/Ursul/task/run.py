import matplotlib.pyplot as plt
from subprocess import PIPE, Popen 
from platform import platform


alpha = 0.0001

base_dir = "cmake-build-release/"
thread_fname = base_dir + "integral"

if platform().startswith("Windows"):
    thread_fname = thread_fname.replace("/","\\") + ".exe"
    
config_fname = "cfgs\config.ini"


class CppException(Exception):
    pass
def mintime(n, alpha, fname, *args):
    max_val = min_val = min_time = None
    min_time_ind = None
    for i in range(n):
        p = Popen(fname + " " + " ".join(map(str, args)), shell=True, stdout=PIPE, stderr=PIPE)

        cpp_res = p.stdout.readline().decode("UTF-8").strip().split()

        if p.wait():
            raise CppException(p.stderr.readline().decode("UTF-8").strip())
            
        if (max_val is None or max_val < float(cpp_res[0])):
            max_val = float(cpp_res[0])
        if (min_val is None or min_val > float(cpp_res[0])):
            min_val = float(cpp_res[0])
        if (min_time is None or int(cpp_res[1]) < min_time):
            min_time = int(cpp_res[1])
            min_time_ind = cpp_res
    
    if abs(max_val - min_val) > alpha:
        raise Exception("Out of error bounds")
    return min_time, cpp_res
    

if __name__ == "__main__":
    
    labels, val = [], []
    
    for i in range(1,6):
        labels.append(str(i) + " потоків")
        res = mintime(20, alpha, thread_fname, config_fname, i)
        val.append(res[0])
        print(res[1])
    
    img_name, opt = "result.png", range(len(labels))
    
    plt.figure(img_name)
    plt.ylabel("Мікросекунди")
    plt.xticks(opt, labels)
    plt.bar(opt, val, width=0.5)
    plt.savefig(img_name)
