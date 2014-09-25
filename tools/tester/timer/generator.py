import random

# gives the code to get a number of type <typ>, generated from the parameters
# [i##d for ##d in <dims>]
def str_pseudo_random(typ, dims, positive = False):
    numbers = [random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000),
               random.randint(0, 100000)]
    ret = ""
    if positive:
        pos_str = "abs"
    else:
        pos_str = ""
    if typ == "ubyte":
        ret = "(ubyte)%s(((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += ") % 256))"
    elif typ == "byte":
        ret = "(byte)%s(((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += ") % 256 - 128))"
    elif typ == "uint16":
        ret = "(uint16)%s(((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += ") % 65535))"
    elif typ == "int16":
        ret = "(int16)%s(((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += ") % 65535 - 32768))"
    elif typ == "uint32":
        ret = "(uint32)%s((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += "))"
    elif typ == "int32":
        ret = "(int32)%s((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += "))"
    elif typ == "float32":
        ret = "(float32)%s(((%d*i0"%(pos_str, numbers[0])
        for i in (range(len(dims)))[1:]:
            ret += " + %d*i%d"%(numbers[i], i)
        ret += ") * 1.23512e-4f * (float32)((i0 % 3 + 1) * 2 - 3)))"
    else:
        raise NotImplementedError
    return ret

# returns the code to create the idx's.
# <names> is a list of couples (strings (the names of the idx's), flags)
# <typ> is a string (the type of the idx's)
# <dims> is a list of integers (the dimension of the idx's)
def str_create_idxs(indent, names, typ, dims):
    if dims == []:
        raise NotImplementedError
    if names == []:
        raise NotImplementedError
    dims_str = str(dims[0])
    for d in dims[1:]:
        dims_str += ", " + str(d)
    ret = "%sidx<%s> %s (%s)"%(indent, typ, names[0][0], dims_str);
    for name in names[1:]:
        ret += ", %s (%s)"%(name[0], dims_str)
    ret += ";\n"
    ret += indent + "int i0"
    for i in (range(len(dims)))[1:]:
        ret += ", i%d"%(i,)
    ret += ";\n"
    indent1 = indent
    for i in xrange(len(dims)):
        ret += "%sfor (i%d = %d; i%d >= 0; --i%d)\n"%(indent1, i, dims[i]-1,
                                                          i, i)
        indent1 += "  "
    ret = ret[:-1] + " {\n"
    for name in names:
        indices = "i0"
        for dim in (range(len(dims)))[1:]:
            indices += ", i%d"%(dim,)
        positive = "positive" in name[1]
        nonzero = "nonzero" in name[1]
        ret += "%s%s.set(%s, %s);\n"%(indent1, name[0],
                                      str_pseudo_random(typ, dims,
                                                        positive = positive),
                                      indices)
        if nonzero:
            ret += "%sif (%s.get(%s) == 0)"%(indent1, name[0], indices)
            ret += "%s  %s.set((%s)1, %s);"%(indent1, name[0], typ, indices)
    ret += indent
    ret = ret[:-2] + "}\n"
    return ret

# returns the code to run the commands
# <commands> : list of strings
def str_run_functions(indent, commands):
    ret = ""
    for command in commands:
        ret += indent + command + ";\n"
    return ret

# returns the code to init the timer, named <name>
def str_timer_init(indent, name):
    ret = ""
    ret += "%sstruct timeval %s_init, %s_end;\n"%(indent, name, name)
    ret += "%sgettimeofday(&%s_init, NULL);\n"%(indent, name)
    return ret

# returns the code to end the timer, named <name>
def str_timer_end(indent, name):
    ret = ""
    ret += "%sgettimeofday(&%s_end, NULL);\n"%(indent, name)
    ret += "%sprintf(\"%%lld %%06d %%lld %%06d\\n\", (long long int)%s_init.tv_sec, (int)%s_init.tv_usec, (long long int)%s_end.tv_sec, (int)%s_end.tv_usec);\n"%(indent, name, name, name, name)
    return ret

# returns the timer program
# <names> is a list of the names of the idx's
# <typ> is the type of the idx's
# <dims> is a list, the dimensions of the idx's
# <commands> is a list of commands to run
def get_program(names, typ, dims, commands, n_cores):
    code = ""
    code += "#include <cstdio>\n"
    code += "#include <ctime>\n"
    code += "#include <sys/time.h>\n"
    code += "#include \"eblearn/idxops.h\"\n"
    code += "using namespace ebl;\n"
    code += "\n"
    code += "int main() {\n"
    indent = "  "
    if n_cores != 0:
        code += indent + "ipp_init(%d);\n"%(n_cores,)
    code += indent + "//creating idx's\n"
    code += str_create_idxs(indent, names, typ, dims)
    code += indent + "//timing\n"
    code += str_timer_init(indent, "timer")
    code += str_run_functions(indent, commands);
    code += str_timer_end(indent, "timer")
    code += indent + "return 0;\n"
    code += "}\n"
    indent = ""
    return code
