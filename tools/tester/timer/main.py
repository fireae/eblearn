import timer
import os
import os.path
import optparse
import result
import parse_conf
import subprocess

def main():
    optparser = optparse.OptionParser(add_help_option = True)
    optparser.add_option("-o", "--output", dest = "out_file", default = "out",
                         type = "str", help = "Output file")
    optparser.add_option("-n", "--ncores", dest = "n_cores", default = 0,
                         type = "int", help = "Number of cores used by IPP")
    optparser.add_option("--openmp", dest = "use_openmp", action = "store_true",
                         help = "Use OpenMP")
    optparser.add_option("--sse", dest = "use_sse", action = "store_true",
                         help = "Use SSE")
    optparser.add_option("--ipp", dest = "use_ipp", action = "store_true",
                         help = "Use IPP's")
    optparser.add_option("--fast", dest = "ipp_fast", action = "store_true",
                         help = "Use __IPP_FAST__ flag")
    optparser.add_option("--checks", dest = "ipp_checks", action = "store_true",
                         help = "Use __IPP_CHECKS__ flag")
    optparser.add_option("-c", "--conffile", dest="conf_file", default = "conf",
                         type = "str",
                         help = 'Name of the configuration file. default is "conf"')
    optparser.add_option("-t", "--tmpdir", dest="tmp_dir", default="tmp",
                         type="str", help="Temporary directory for compilation files")
    optparser.add_option("--ipproot", dest="ipproot", default="", type="str",
                         help="IPP root directory")
    optparser.add_option("--eblroot", dest="eblroot", default=os.path.abspath("../.."),
                         type="str", help="eblearn root directory")
    options, args = optparser.parse_args()

    # find ebl root directory
    eblroot = os.path.abspath(options.eblroot)

    # find ipp root directory
    if options.use_ipp:
        if (options.ipproot != ""):
            ipproot = options.ipproot
        else:
            candidates = os.listdir("/opt/intel/ipp")
            if candidates == []:
                print "Cannot find IPP. Please manually set it with --ipproot option"
                return
            else:
                numeric_candidates = []
                for candidate in candidates:
                    if candidate[0].isdigit():
                        numeric_candidates.append(candidate)
                if numeric_candidates != []:
                    numeric_candidates.sort()
                    ippdir1 = os.path.join("/opt/intel/ipp", numeric_candidates[-1])
                else:
                    ippdir1 = os.path.join("/opt/intel/ipp", candidates[0])
                if os.listdir(ippdir1) == []:
                    print "Cannot find IPP. Please manually set it with --ipproot option"
                    return
                if "em64t" in os.listdir(ippdir1):
                    ipproot = os.path.join(ippdir1, "em64t")
                if "ia32" in os.listdir(ippdir1):
                    ipproot = os.path.join(ippdir1, "ia32")
                else:
                    ipproot = os.path.join(ippdir1, os.listdir(ippdir1)[0])
        if not os.path.exists(ipproot):
            print "ipp directory not found : %s"%(ipproot,)
            return
    else:
        ipproot = None

    # find temporary directory
    if os.path.exists(options.tmp_dir):
        if not os.path.isdir(options.tmp_dir):
            print "%s exists, but is not a directory."%(options.tmp_dir)
            print "You can remove it to solve the problem."
            return
    else:
        os.mkdir(options.tmp_dir)
    tmpdir = os.path.abspath(options.tmp_dir)

    # find output file
    out_mode = "erase"
    if os.path.exists(options.out_file):
        if os.path.isfile(options.out_file):
            print "Warning : %s file already exists."%(options.out_file)
            ans = ""
            while ans != "a" and ans != "c" and ans != "e":
                ans = raw_input("Type (a) to abord, (c) to append, or (e) to erase.\n")
            if ans == "a":
                return
            elif ans == "c":
                out_mode = "append"
            elif ans == "e":
                out_mode = "erase"
        else:
            print "%s exists, but is not a file."%(options.out_file,)
            return

    # find configuration file
    if not os.path.isfile(options.conf_file):
        print "%s is not a file."%(options.conf_file,)
        return
    conffile = open(options.conf_file)
    (patterns, functions, convolution) = parse_conf.parse(conffile)
    conffile.close()

    # set flags
    flags = ""
    if options.use_openmp == True:
        flags += "USEOPENMP=1"
    else:
        flags += "USEOPENMP=0"
    flags += " "
    if options.use_sse == True:
        flags += "USESSE=1"
    else:
        flags += "USESSE=0"
    flags += " "
    if options.use_ipp == True:
        flags += "NOIPP=0"
    else:
        flags += "NOIPP=1"
    flags += " "
    if options.ipp_fast == True:
        flags += "IPPFAST=1"
    else:
        flags += "IPPACC=1"
    flags += " "
    if options.ipp_checks == True:
        flags += "IPPCHECKS=1"
    else:
        flags += "IPPCHECKS=0"

    # benchmark functions
    if options.use_ipp:
        force_ipp = "FORCE_IPP_DIR=%s"%(ipproot)
    else:
        force_ipp = ""
    compiler = subprocess.Popen(["""export %s %s; cd %s && \
make release prj=eblearn && make install DIR=%s"""%(flags, force_ipp,
                                                    eblroot, tmpdir)],
                                shell = True) #TODO
    compiler.wait()
    timer.time_functions(tmpdir, eblroot, ipproot, patterns, functions,
                         options.out_file, out_mode, flags, options.n_cores)

    # benckmark convolution
    if convolution != None:
        (conv_patterns, conv_types) = convolution
        timer.time_conv(options.out_file, out_mode, flags, options.n_cores,
                        eblroot, tmpdir, ipproot, conv_patterns,
                        conv_types, 1)

if __name__ == "__main__":
    main()
