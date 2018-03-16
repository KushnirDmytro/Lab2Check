import subprocess
import os
absol=1000
rel=0.01
thr=8
maxx=8
results=[]
times=[]
for _ in range(10):
	p=subprocess.Popen([os.getcwd()+'/tointornottoint',str(absol),str(rel),str(thr),str(-100), str(100) ,str(-100), str(100),str(maxx)],stdout=subprocess.PIPE)
	out, err = p.communicate()
	results.append(float(out.split()[0]))
	times.append(float(out.split()[1]))
print(results)
print(min(times))