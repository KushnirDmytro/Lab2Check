import os
os.popen("g++ main.cpp -o main")
out = os.popen("./main 4 -100 100 -100 100 0.1")
print(out)