import os
import subprocess
os.system("make all")
for _ in range(20):
    subprocess.call("./threads.out")
    subprocess.call("./integrate.out")
os.remove("integrate.o")
os.remove("integrate.out")
os.remove("threads.o")
os.remove("threads.out")
file_th = open("results_th.txt", "r")
file_int = open("results_int.txt", "r")
results_th = file_th.readline().split(' ')
results_int = file_int.readline().split(' ')
result_th = results_th[0]
result_int = results_int[0]
abs_er = results_int[2]
rel_er = results_int[3]
times_th = min(results_th[1::4])
times_int = min(results_int[1::4])
output = str()
output += "th_result: " + result_th +  "\nint_result: " + result_int + "\nabs err:" + abs_er + "\nrel err: " +rel_er + "\nthread calculation time: " + times_th + "\nparralel calculation time: "+ times_int 
f = open('output.txt', 'wt', encoding='utf-8')
f.write(output)
file_int.close()
file_th.close()
f.close()
os.remove("results_th.txt")
os.remove("results_int.txt")