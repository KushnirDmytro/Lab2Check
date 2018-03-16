#!/usr/bin/env python
import subprocess
import os
import io


n = 5
results = []
checked_results = []
maple =  float("4.545447652E6")
min_time = float('inf')

for i in range(0, n, 1):
    result = []
    print(i)
    proc = subprocess.Popen([os.getcwd() + "/a.out"], stdout=subprocess.PIPE)
    for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
        result.append(line)
    res = result[0].strip()[8:]
    time = result[2].strip()[6:]
    results.append((res, time))

for el in results:
    if (abs(maple - float(el[0])) / maple) < 0.01:
        checked_results.append(el)
        time = float(el[1])
        if time < min_time:
            min_time = time

#print("cheked results = ", checked_results)
print("min_time = ", min_time)
