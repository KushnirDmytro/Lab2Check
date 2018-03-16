import subprocess
from sys import argv

if len(argv) > 1:
    itr = int(argv[1])

else:
    """
    set default value of iterations HERE
    """
    itr = 1

if len(argv) > 2:
    abs_er = int(argv[2])

else:
    """
    set default value of absolute error HERE
    """
    abs_er = 1
    


results = list()
times = list()

for i in range(itr):
    #out = subprocess.run("g++ -std=c++14 -O3 -ffast-math single_thread.cpp -o single_thread && ./single_thread -maxiter 5", shell=True, encoding="ascii", stdout=subprocess.PIPE)
    out = subprocess.run("g++ -std=c++14 -O3 -ffast-math -pthread multiple_threads.cpp -o multiple_threads && ./multiple_threads -threads 3 -maxiter 5"
, shell=True, encoding="ascii", stdout=subprocess.PIPE)
    out = out.stdout.split("\n")

    res = float(out[-5])
    results.append(res)

    time = float(out[-2].split(" ")[-1])
    times.append(time)

    # rel_err = float(out[-3].split(" ")[-1])
    # abs_err = float(out[-4].split(" ")[-1])

er = abs(min(results) - max(results))
print("absolute error is OK:", er < abs_er, "; error:", er)
print("min time:", min(times))