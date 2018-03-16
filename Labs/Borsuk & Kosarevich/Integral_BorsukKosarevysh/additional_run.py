from run import *

if __name__ == "__main__":
    configs = list(map(lambda x: "configs/" + x,
                       ["config11.txt", "config12.txt", "config21.txt", "config22.txt", "config23.txt", "config24.txt"]))

    for conf_filename in configs:
        x, y = [], []
        p = CppProgram(simple_integral_filename, conf_filename)
        x.append("simple")
        y.append(p.get_mintime_validate(4, alpha))
        for i in n_threads:
            p = CppProgram(integral_filename, conf_filename, i)
            x.append("{0} threads".format(i))
            y.append(p.get_mintime_validate(4, alpha))
        with open("result{}.txt".format(conf_filename[-6:-4]), 'w') as file:
            file.write(str(y))
        display_graph(x, y, "result{}.png".format(conf_filename[-6:-4]))
