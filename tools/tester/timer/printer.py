import pylab
import optparse
import result

class FunctionBenchmark(object):
    # results : result.TestResult list dict (index : case)
    def __init__(self, results):
        self.title = results.values()[0][0].test_name
        # self.result : (x, y, n) list dict (index : case) dict (index : typ)
        self.results = {}
        for case in results:
            for res in results[case]:
                for typ in res.get_types():
                    if typ not in self.results:
                        self.results[typ] = {}
                    if i not in self.results[typ]:
                        self.results[typ][case] = []
                    self.results[typ][case].append(res.get_sorted_sample(typ))

    def display(self):
        n_types = len(self.results)
        n_col = int(float(n_types) / 2 + 0.5)
        if n_types == 1:
            n_row = 1
        else:
            n_row = 2
        colors = ["blue", "red", "green", "cyan", "magenta"]
        i = 1
        for typ in self.results:
            pylab.subplot(n_col, n_row, i)
            i_color = 0
            for case_name in sorted(self.results[typ].keys()):
                fst = True
                for res in self.results[typ][case_name]:
                    xtab = [a[0] for a in res]
                    ytab = [a[1] / a[2] for a in res]
                    if fst == True:
                        pylab.loglog(xtab, ytab, label = case_name,
                                     color = colors[i_color])
                        fst = False
                    else:
                        pylab.loglog(xtab, ytab, color = colors[i_color])
                i_color += 1
            pylab.legend(loc="upper left")
            pylab.title("%s %s"%(self.title, typ), verticalalignment = "top")
            i += 1
        pylab.subplot(n_col, n_row, 1)
        pylab.show()

if __name__ == "__main__":
    optparser = optparse.OptionParser(add_help_option = False)
    optparser.add_option("-1", dest = "in_files_1",
                         action = "append", type = "str")
    optparser.add_option("--l1", dest = "label1", default = "1", type = "str")
    optparser.add_option("-2", dest = "in_files_2",
                         action = "append", type = "str")
    optparser.add_option("--l2", dest = "label2", default = "2", type = "str")
    optparser.add_option("-3", dest = "in_files_3",
                         action = "append", type = "str")
    optparser.add_option("--l3", dest = "label3", default = "3", type = "str")
    optparser.add_option("-4", dest = "in_files_4",
                         action = "append", type = "str")
    optparser.add_option("--l4", dest = "label4", default = "4", type = "str")
    optparser.add_option("-5", dest = "in_files_5",
                         action = "append", type = "str")
    optparser.add_option("--l5", dest = "label5", default = "5", type = "str")
    options, args = optparser.parse_args()
    results_dict = {}
    for i in range(1, 6):
        file_contents = []
        if getattr(options, "in_files_" + str(i)) == None:
            continue
        for filename in getattr(options, "in_files_%d"%(i,)):
            f = open(filename)
            file_contents.append(f.read().rstrip("\n").split("\n"))
            f.close()
        results = [[result.decode_result(a) for a in f] for f in file_contents]
        for test_set in results:
            for res in test_set:
                if res.test_name not in results_dict:
                    results_dict[res.test_name] = {}
                label_name = getattr(options, "label%d"%(i,))
                if label_name not in results_dict[res.test_name]:
                    results_dict[res.test_name][label_name] = []
                results_dict[res.test_name][label_name].append(res)
    benchmarks = []
    for key in results_dict:
        benchmarks.append(FunctionBenchmark(results_dict[key]))
    for b in benchmarks:
        b.display()
