Eblearn Project
--------------------------------------------------------------------------------

For detailed installation, please refer to doc/install.shtml.

** Quick install **

1. Install additional external libraries you need (most of them are optional)
   as described in doc/install.shtml.

2. Go to either 'core' or 'tools' directory to build the project,
   depending on what libraries you are interested in:

   * core:
     * libidx: tensor library
     * libeblearn: learning library
   * tools:
     * libidxgui: GUI library for libidx
     * libeblearngui: GUI library for libeblearn
     * libeblearntools: Data compilation and other tools
     * dscompiler: dataset compiler
     * dssplit: dataset split
     * dsmerge: datasets merge
     * demos

3. Compile the project. Under unix/mac, one can call 'make' in either
   core or tools directory. Calling 'make' in 'tools' directory will make
   the libraries in 'core'.

4. To test if the library works properly, call 'make test' from 'tools'
   directory (run ./configure before to set your MNIST path and test
   learning functionalities).
