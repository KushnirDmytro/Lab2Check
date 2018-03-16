from os import popen

def set_threads(threads):
    config = open('ackley.cfg', 'r+')   
    lines = config.readlines()
    lines[1] = 'threads='+ str(threads) + '\n'
    config.seek(0)
    config.write(''.join(lines))
    config.close()

table = dict()
results = set()

for i in range(1, 9):

    set_threads(i)
    times = list()

    for j in range(10):
        output = popen('./ackley').read()
        print(output, end='')
        results.add(output.splitlines()[0])
        times.append(int(output.splitlines()[2]))

    times.sort()
    table[i] = times[0]

print()

if len(results) == 1:
    print('all results are the same', results.pop())
else:
    print('not all results are the same', results)    

print()

print('threads: time (ms)')
print(table)
