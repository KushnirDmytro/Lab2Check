import subprocess
import os
import sys

if(len(sys.argv)!=2):
    print("python3 script.py (int)number_of_runs")
    exit()

a = subprocess.run(["make"])

absolute_error = 80
relative_error = 0.001

results = []
times = []
for iter in range(int(sys.argv[1])):
    print(str(iter+1)+"/"+sys.argv[1])
    b = subprocess.run(["./paralel", "-100", "100", "-100", "100", "4", str(absolute_error),  str(relative_error)])
    #start = time.time()

    while not os.path.isfile("./program_result.csv"):
        pass
    with open("program_result.csv") as file:
        line = list(map(float,file.readline().split(",")))
    results.append(line[0])
    times.append(line[3])
match=False
for i in range(0, len(results)-1):
    for j in range(i+1, len(results)):
        if abs(results[i] - results[j]) > absolute_error:
            print("Results don't match!")
            match=True
            break
    if match:
        break
else:
    print("Results are correct!")
print(min(times))