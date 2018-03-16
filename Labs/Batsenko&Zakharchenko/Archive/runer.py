import os
import numpy as np
import json

with open('config.json') as json_data_file:
    config = json.load(json_data_file)

times = []
values = []
abs_error = config["errors"]["abs"]
rel_error = config["errors"]["rel"]
max_iterations_num = config["max_iterations_num"]
threads = config["threads"]
delta = config["delta"]
params_str = ""
threads_arr = []

os.popen("g++ -std=c++14 -Og -pthread using_threads.cpp -o multiple_threads && ./multiple_threads")

for i in range(max_iterations_num):

    if (len(config["params"]["a1"]) > 0) and (len(config["params"]["a2"]) > 0) and (len(config["params"]["c"]) > 0):
        params_str = " ".join(config["params"]["a1"])
        params_str += " " + " ".join(config["params"]["a2"])
        params_str += " " + " ".join(config["params"]["c"])

    output = os.popen("./multiple_threads " +\
                                            threads + " " +\
                                            config["intervals"]["x_start"] + " " +\
                                            config["intervals"]["x_finish"] + " " +\
                                            config["intervals"]["y_start"] + " " +\
                                            config["intervals"]["y_finish"] + " " +\
                                            config["params"]["m"] + " " +\
                                            params_str + " " +\
                                            str(delta)).readlines()

    # Use split to get value - output[0] and time - output[1]
    curr_value, curr_time = [i.split()[-1] for i in output]

    curr_value, curr_time = float(curr_value), int(curr_time)

    values.append(curr_value)
    times.append(curr_time)

    delta *= 2
    threads_arr.append(threads)
    threads = str(int(threads) + 1)

    # Prints of the program
    print("Current value: ", curr_value)
    print("Current absolute error: ", abs(abs(values[i-1]) - abs(values[i])))
    print("Current relative error: ", abs(abs(values[i-1]) - abs(values[i])) / abs(values[i-1]))
    print("Current time: ", curr_time)
    print("\n")



    if i == 0:
        continue

    if abs(abs(values[i-1]) - abs(values[i])) > abs_error:
        # print("Absolute error is bigger then possible, continuing")
        continue

    if abs(abs(values[i-1]) - abs(values[i])) / abs(values[i-1]) > rel_error:
        # print("Relative error is bigger then possible, continuing")
        continue

    break


print("Number of threads: ", threads)
print("Number of iterations: ", i+1)
print("Minimal time: ", np.min(times))
print("Mean value of integration: ", np.mean(values))

# code for writing data (num of threads and time per iteration) to csv format:
# with open('data.csv','w') as file:
#     for threads_num, time in zip(threads_arr, times):
#         file.write(threads_num + ", " + str(time))
#         file.write('\n')
