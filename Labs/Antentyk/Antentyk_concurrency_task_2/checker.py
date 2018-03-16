import sys
import os
import atexit
import shutil
import subprocess

EXIT_FAILURE = 1
EXIT_SUCCESS = 0

RESULT_FILE_NAME = "results.txt"
CONFIG_DIR = "_config_tmp"

def init():
    global result
    result = open(RESULT_FILE_NAME, "w")
    if os.path.exists(CONFIG_DIR):
        shutil.rmtree(CONFIG_DIR)    
    os.mkdir(CONFIG_DIR)

def clean():
    global result
    result.close()

    shutil.rmtree(CONFIG_DIR)

atexit.register(clean)

EPS = 1e-4

STANDARD = 857208.2414
RELATIVE_ERROR = 0.0001
ABSOLUTE_ERROR = STANDARD * RELATIVE_ERROR

RUN_AMOUNT = 20

config_str = "ITERATIONS_NUM=10;\
MIN_X=-100;\
MAX_X=100;\
MIN_Y=-100;\
MAX_Y=100;\
A=20;\
B=0.2;\
C=6.28318530718;\
STANDARD=%s;\
RELATIVE_ERROR=%s;\
ABSOLUTE_ERROR=%s;" % (
    str(STANDARD),
    str(RELATIVE_ERROR),
    str(ABSOLUTE_ERROR)
)

def eq(a, b):
    return abs(a - b) < EPS

def check_output(output):
    tmp = output.split("\n")
    output = []
    for item in tmp:
        output.extend(item.split(" "))
    
    output = list(map(float, filter(lambda item: item != '', output)))

    result = output[0]

    absolute_error_result = output[1]
    relative_error_result = output[2]

    time_spent = output[3]

    if(not eq(abs(STANDARD - result), absolute_error_result)):
        print("Absolute error does not correspond")
        sys.exit(EXIT_FAILURE)
    if(absolute_error_result > ABSOLUTE_ERROR):
        print("Absolute error bigger than allowed")
        sys.exit(EXIT_FAILURE)
    
    if(not eq(abs(STANDARD - result) / STANDARD, relative_error_result)):
        print("Relative error does not correspond")
        sys.exit(EXIT_FAILURE)
    if(relative_error_result > RELATIVE_ERROR):
        print("Relative error bigger than allowed")
        sys.exit(EXIT_FAILURE)
    
    return time_spent


def check_consecutive():
    print("Checking consecutive", end='')
    sys.stdout.flush()

    min_time = float('inf')

    config = open(CONFIG_DIR + "/tmp.txt", "w")
    config.write(config_str)
    config.write("IS_CONSECUTIVE=1;")
    config.close()

    command = ["./bin/main", "-c", CONFIG_DIR + "/tmp.txt"]

    for i in range(RUN_AMOUNT):
        print('.', end='')
        sys.stdout.flush()
        currenttime = check_output(subprocess.check_output(command).decode('ascii'))
        min_time = min(min_time, currenttime)

    min_time = int(min_time)

    result.write("Consecutive: " + str(min_time) + "\n")
    print()
    sys.stdout.flush()


def check_threads(threads_num):
    print("Checking %s thread(s)" % str(threads_num), end='')
    sys.stdout.flush()

    min_time = float('inf')

    config = open(CONFIG_DIR + "/tmp.txt", "w")
    config.write(config_str)
    config.write("THREADS_NUM=%s;" % (str(threads_num)))
    config.close()

    command = ["./bin/main", "-c", CONFIG_DIR + "/tmp.txt"]

    for i in range(RUN_AMOUNT):
        print('.', end='')
        sys.stdout.flush()
        currenttime = check_output(subprocess.check_output(command).decode('ascii'))
        min_time = min(min_time, currenttime)

    min_time = int(min_time)

    result.write(("%s thread(s): " % (str(threads_num))) + str(min_time) + "\n")
    print()
    sys.stdout.flush()


init()

threads_lst = list(range(1, 11))
threads_lst.append(50)
threads_lst.append(100)
threads_lst.append(200)
threads_lst.append(500)
threads_lst.append(1000)
threads_lst.append(1500)
threads_lst.append(2000)

check_consecutive()
for i in threads_lst:
    check_threads(i)


exit(EXIT_SUCCESS)