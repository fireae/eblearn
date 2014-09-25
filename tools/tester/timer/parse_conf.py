import result
import sys

def parse_type(str_typ):
    i = 0
    types = []
    while i < len(str_typ):
        if str_typ[i:i+2] == "8u":
            types.append("ubyte")
            i += 2
        elif str_typ[i:i+2] == "8s":
            types.append("byte")
            i += 2
        elif str_typ[i:i+3] == "16u":
            types.append("uint16")
            i += 3
        elif str_typ[i:i+3] == "16s":
            types.append("int16")
            i += 3
        elif str_typ[i:i+3] == "32u":
            types.append("uint32")
            i += 3
        elif str_typ[i:i+3] == "32s":
            types.append("int32")
            i += 3
        elif str_typ[i:i+3] == "32f":
            types.append("float32")
            i += 3
        else:
            print "Wrong type : %s"%(str_typ,)
            sys.exit(0)
    return types
    

def parse(conffile):
    blocks = conffile.read().split(")")[:-1]
    conf = {}
    for block in blocks:
        block = block.strip().rstrip()
        if block[0] != "(":
            print "\"(\" missing."
            return
        block = block[1:].strip()
        splitedblock = [line.strip().rstrip() for line in block.split("\n")]
        i = 0
        while i != len(splitedblock):
            if splitedblock[i][0] == "#":
                splitedblock.remove(splitedblock[i])
            else:
                i += 1
        blockname = splitedblock[0]
        splitedblock = splitedblock[1:]
        conf[blockname] = [line.split() for line in splitedblock]

    if "dims" not in conf:
        print "\"dims\" block missing in conf"
        return
    dims = [[int(n) for n in line] for line in conf["dims"]]

    if "n_total" not in conf:
        print "\"n_total\" block missing in conf"
        return
    n_total = int(conf["n_total"][0][0])

    if "n_tests_min" not in conf:
        print "\"n_tests_min\" block missing in conf"
        return
    n_tests_min = int(conf["n_tests_min"][0][0])

    patterns = [(max(n_tests_min,int(n_total/result.size_of_dims(d))), d) for d in dims]

    if "functions" not in conf:
        print "\"functions\" block missing in conf"
        return
    functions = []
    init_rand = 0
    for line in conf["functions"]:
        test_name = line[0]
        fun_name = line[1]
        params = [a for a in line[2]]
        types = parse_type(line[3])
        functions.append([test_name, fun_name, params, types, init_rand])
        init_rand += 1

    if "convolution" in conf:
        conv = conf["convolution"]
        convolution_patterns = [tuple([int(a) for a in line]) for line in conv[:-1]]
        convolution_types = parse_type(conv[-1][0])
        convolution = (convolution_patterns, convolution_types)
    else:
        convolution = None

    return (patterns, functions, convolution)
