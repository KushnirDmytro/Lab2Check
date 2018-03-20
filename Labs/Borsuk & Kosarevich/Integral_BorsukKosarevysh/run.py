from subprocess import Popen, PIPE
import matplotlib.pyplot as plt
from platform import platform


alpha = 0.0001
simple_integral_filename = "cmake-build-release/simple_integral.exe" if platform().startswith("Windows") else "cmake-build-release/simple_integral"
integral_filename = "cmake-build-release/integral.exe" if platform().startswith("Windows") else "cmake-build-release/integral"
config_filename = "configs/config.txt"
n_threads = list(range(1, 6))


class CppException(Exception):
    pass


class CppProgram(object):
    def __init__(self, filename, *args):
        self.args = filename + " " + " ".join(map(str, args))

    def run(self):
        p = Popen(self.args, shell=True, stdout=PIPE, stderr=PIPE)
        if p.wait():
            raise CppException(p.stderr.readline().decode("UTF-8").strip())
        result = p.stdout.readline().decode("UTF-8").strip()
        return result

    def get_mintime_validate(self, n, alpha):
        values = []
        min_time = None
        for i in range(n):
            result = self.run().split()
            # add value to list
            values.append(float(result[0]))
            # find min time
            result[1] = int(result[1])
            if (min_time is None or result[1] < min_time):
                min_time = result[1]

        if abs(max(values) - min(values)) > alpha:
            raise Exception("Values are NOT in error bound")
        return min_time


def display_graph(x,y, filename):
    plt.figure(filename[:-4])
    plt.bar(range(len(x)), y, width=0.75)
    plt.xticks(range(len(x)), x)
    plt.ylabel("microseconds")
    plt.savefig(filename)


if __name__ == "__main__":
    x, y = [], []
    p = CppProgram(simple_integral_filename, config_filename)
    x.append("simple")
    y.append(p.get_mintime_validate(20, alpha))
    for i in n_threads:
        p = CppProgram(integral_filename, config_filename, i)
        x.append("{0} threads".format(i))
        y.append(p.get_mintime_validate(20, alpha))
    display_graph(x, y, "result.png")
