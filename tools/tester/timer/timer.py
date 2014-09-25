import generator
import subprocess
import os
import os.path
import random
import sys
import string
import base64
import cPickle
from result import *

def get_random(typ):
    if typ == "ubyte":
        return random.randint(0, 255)
    elif typ == "byte":
        return random.randint(-128, 127)
    elif typ == "uint16":
        return random.randint(0, 65535)
    elif typ == "int16":
        return random.randint(-32768, 32767)
    elif typ == "uint32":
        return random.randint(0L, 4294967295L)
    elif typ == "int32":
        return random.randint(-2147483648L, 2147483647L)
    elif typ == "float32":
        return random.uniform(-1e30, 1e30)
    else:
        raise NotImplementedError

verbose_v = True
def verbose(s):
    if verbose_v == True:
        print s

def parse_timer_output(output):
    out_lines = output.rstrip("\n").split("\n")
    for line in out_lines[:-1]:
        print line
    times_str = out_lines[-1].split(" ")
    if len(times_str) != 4:
        print "parse_time_output : wrong output : %s"%(times_str,)
    times_str = [times_str[0] + times_str[1], times_str[2] + times_str[3]]
    times = [int(a) for a in times_str]
    return float(times[1] - times[0]) / 1000000.0
    

def time_program(dir, eblroot, ipproot, names, typ, dims, code_to_time, args, n_cores):
    if ipproot == None:
        ipproot = ""
    dir = os.path.abspath(dir)
    verbose("Writing cpp file")
    totime_file = open("%s/timefun.cpp"%(dir,), "w")
    totime_file.write(generator.get_program(names, typ, dims, code_to_time,
                                            n_cores))
    totime_file.close()
    verbose("Compiling")
    gpp_str = """export %s EBL_ROOT=%s TMP_DIR=%s IPPROOT=%s TIMEFUN=1; \
cp CMakeLists.txt %s && cd %s && cmake -DCMAKE_BUILD_TYPE=Release . && \
make timefun"""%(args, eblroot, dir, ipproot, dir, dir)
    verbose(gpp_str)
    gpp = subprocess.Popen([gpp_str], shell = True, stderr = subprocess.PIPE,
                           stdout = subprocess.PIPE)
    gpp.wait()
    verbose(gpp.stdout.read())
    print gpp.stderr.read()
    verbose("Timing")
    totime = subprocess.Popen(["""export LD_LIBRARY_PATH=%s/lib:%s/sharedlib:\
$LD_LIBRARY_PATH; cd %s && ./timefun"""%(os.path.abspath(dir), ipproot, dir)],
                              shell = True, stdout = subprocess.PIPE)
    totime.wait()
    time = parse_timer_output(totime.stdout.read())
    verbose("Timed : " + str(time))
    return time

# parameters is a tuple of :
# - letter "r" if idx not to be reused
# - letter "i" if idx
# - letter "s" if scalar
# - letter "o" if out
# - letter "p" if "i" with positive values
# - letter "z" if "i" with non-zero values
def get_code_to_time(n, name, parameters, typ, dims):
    names = []
    for i in xrange(len(parameters)):
        p = parameters[i]
        #if p == "r":
        #    names += ["A%d_%d"%(i, j) for j in xrange(n)]
        if p == "i" or p == "o" or p == "r":
            names += [("A%d"%(i,), [])]
        elif p == "s":
            pass
        elif p == "p":
            names += [("A%d"%(i,), ["positive"])]
        elif p == "z":
            names += [("A%d"%(i,), ["nonzero"])]
    line = "for (int i = %d; i >= 0; --i) { volatile %s a = %s("%(n-1, typ, name)
    for i in xrange(len(parameters)):
        p = parameters[i]
        if p == "i" or p == "o" or p == "r" or p == "p" or p == "z":
            line += "A%d, "%(i,)
        elif p == "s":
            line += "(%s)%s, "%(typ, get_random(typ))
    if parameters != []:
        line = line[:-2]
    line += "); }"
    code = [line]

    """
    code = []
    for j in xrange(n):
        line = name + "("
        for i in xrange(len(parameters)):
            p = parameters[i]
            if p == "i": #TODO
                p = "n"
            if p == "i":
                line += "A%d_%d, "%(i, j)
            elif p == "n":
                line += "A%d, "%(i,)
            elif p == "s":
                line += "(%s)%s, "%(typ, get_random(typ))
            elif p == "o":
                line += "A%d, "%(i,)
        if parameters != []:
            line = line[:-2]
        line += ")"
        code += [line]
        """
    return (names, code)

# name : name of the function
# parameters : parameters to call the function with. cf. get_code_to_time
# typ : type of the template
# n : number of times to repeat
# dims : dimensions of the idx's
def time_function_elem(dir, eblroot, ipproot, name, parameters, typ, n, dims, args, n_cores):
    (names, code) = get_code_to_time(n, name, parameters, typ, dims)
    return time_program(dir, eblroot, ipproot, names, typ, dims, code, args, n_cores)

# - tmp_dir   : directory to store the temporary files
# - eblroot   : root eblearn directory
# - ipproot   : root ipp directory
# - patterns  : list of tuples (n, dims)
#               - n : number of times to repeat
#               - dims : dimensions of the idx's
# - functions : list of tuples (test_name, function_name, parameters, types, seed)
#               - test_name : name of the test (must be unique)
#               - function_name : name of the function
#               - parameters : list of parameters (cf. get_code_to_time)
#               - types : list of the types
#               - seed : seed for the random generator
# - filename  : name of the file to write
# - out_mode  : append or erase the out file
# - args      : compilation options
# - n_cores   : number of cores to use with IPP
def time_functions(tmp_dir, eblroot, ipproot, patterns, functions,
                   filename, out_mode, args, n_cores):
    if (out_mode == "erase"):
        f = open(filename, "w") #clear the file

        f.close()
    else:
        assert(out_mode == "append")
    for (test_name, function_name, parameters, types, seed) in functions:
        result = TestResult(test_name, function_name)
        for (n, dims) in patterns:
            for typ in types:
                print "Test " + test_name + " (" + function_name \
                    + ") " + str(n) + " " + str(dims) + " " + typ
                random.seed(seed)
                time = time_function_elem(tmp_dir, eblroot, ipproot,
                                          function_name, parameters,
                                          typ, n, dims, args, n_cores)
                result.add_case(n, seed, typ, time, dims = dims)
        f = open(filename, "a")
        f.write(result.get_encoded())
        f.close()

def time_conv(out_file, out_mode, args, n_cores,
              eblroot, tmp_dir, ipproot, conv_patterns, types, seed):
    #TODO seed
    if (out_mode == "erase"):
        f = open(out_file, "w")
        f.close()
    else:
        assert(out_mode == "append")

    verbose("Compiling")
    gpp_str = """export %s EBL_ROOT=%s TMP_DIR=%s IPPROOT=%s TIMECONV=1; \
cp CMakeLists.txt %s && cp timeconv.cpp %s && cd %s && \
cmake -DCMAKE_BUILD_TYPE=Release . && make timeconv"""%(args, eblroot, tmp_dir,
                                                        ipproot, tmp_dir, tmp_dir,
                                                        tmp_dir)
    verbose(gpp_str)
    gpp = subprocess.Popen([gpp_str], shell = True, stdout = subprocess.PIPE)
    gpp.wait()
    verbose(gpp.stdout.read())
    verbose("Timing")
    result = TestResult("convolution", "convolution")
    for (n, size, n_exs, size_param) in conv_patterns:
        for typ in types:
            print "Test convolution %d %d %s"%(n, size, typ)
            totime = subprocess.Popen(["export LD_LIBRARY_PATH=%s/lib:\
%s/sharedLib:$LB_LIBRARY_PATH; cd %s && ./timeconv %d %d %d %d %d"""\
                                           %(os.path.abspath(tmp_dir),
                                             ipproot, tmp_dir,
                                             size, n, n_exs, size_param, n_cores)],
                                      shell = True, stdout = subprocess.PIPE)
            totime.wait()
            time = parse_timer_output(totime.stdout.read())
            result.add_case(n, seed, typ, time, size = size)
    f = open(out_file, "a")
    f.write(result.get_encoded())
    f.close()
