#!/usr/bin/python

import random
import os
import sys

################################################################################
# switches

# paths
dsname = 'housenums'
machine = str(os.getenv("HOSTNAME")) + 'a'
root = os.path.join(str(os.getenv("HOME")), machine) + 'data'
root = os.path.join(os.path.join(root, 'housenums'), dsname)

svhnroot = ''
dataroot = svhnroot
out = os.path.join('svhn')
#we're assuming that eblearn is installed in the system with the binaries in the path
bin = ''
ebl = ''
# if not, uncomment the two lines below, filling out eblearn's path
#ebl = 'path to eblearn dir'
#bin = os.path.join(ebl, 'bin') #'../../bin/

dirpostfix = '_32x32'
train = ['train']
test = ['test']
extra = ['extra']
name_prefix = ''
train_validation_samples_perclass = 400 #there are 10 classes
extra_validation_samples_perclass = 200 
h = 32 # target height
w = 32 # target width
chans = 3 # target color channels
dims = str(h) + 'x' + str(w) + 'x' + str(chans)
precision = '-precision float'
resize = 'bilinear' #'mean'

jitter = '' # -jitter 2,2,8,.10,0,0,5 '
mirror = ' '
# names
id = '' #resize + str(h) + 'x' + str(w)
name = dsname + '_' + id

# preprocessing ################################################################
ppresize = ''

pp = 'YnUV' # preprocessing
ppkernel = '7x7' #9
ppresize = ppresize + ' -channels ' + pp 
ppresize = ppresize + ' -resize ' + resize 
ppresize = ppresize + ' -kernelsz ' + ppkernel
ppresize = ppresize + ' -addpp '

name_prefix = name_prefix + 'ynuv7'
################################################################################

# bin commands
dscompile = os.path.join(bin, 'dscompile ')
dsmerge = os.path.join(bin, 'dsmerge ')
dssplit = os.path.join(bin, 'dssplit ')
dsdisplay = os.path.join(bin, 'dsdisplay ')

# debug variables
maxdata = ' -maxdata 200 '
maxperclass = ' -maxperclass 5 '
ddisplay = ' -disp -sleep 10000 '
#debug = ddisplay + ' ' + maxdata # $maxperclass $ddisplay"
#debug = ddisplay + maxdata + maxperclass
#debug = maxdata
debug = ''
save_display = 0

# create directories
print 'eblearn: ' + ebl
print 'dataroot: ' + dataroot
print 'out: ' + out
try: os.makedirs(out)
except OSError, e: print 'warning: ' + str(e)

###############################################################################

all = []
for e in test: all.append(e)
for e in train: all.append(e)
for e in extra: all.append(e)
print 'Extracting data from all sets: ' + str(all)

for e in all:
    # set names for this set
    setroot = os.path.join(dataroot, e + dirpostfix)
    outname  = 'svhn' + '_' + name_prefix + '_' + e 
    tmpout = os.path.join(out, e + '_tmpdir' )
    cmd = dscompile + setroot  \
          + ' ' + precision + ' -outdir ' + out \
          + ' -dname ' + outname + ' ' \
          + ' ' + ' -dims ' + dims \
          + jitter  + ppresize \
          + mirror + debug + ' -tmpout ' + tmpout 
    print cmd + '\n'
    if os.system(cmd) != 0: sys.exit(-1)

###############################################################################
# Split the train and extra sets into train/val sets

# Split train dataset
for e in train:
    inname = 'svhn' + '_' + name_prefix + '_' + e 
    trainname = inname + 't' + '_train'
    valname = inname + 't' + '_val'
    cmd = dssplit + out \
        + ' ' + inname + ' ' + valname + ' ' + trainname \
        + ' -maxperclass ' + str(train_validation_samples_perclass)
    print cmd + '\n'
    if os.system(cmd) != 0: sys.exit(-1)

# # Split extra dataset
for e in extra:
    inname = 'svhn' + '_' + name_prefix + '_' + e 
    trainname = inname + '_train'
    valname = inname + '_val'
    cmd = dssplit + out \
        + ' ' + inname + ' ' + valname + ' ' + trainname \
        + ' -maxperclass ' + str(extra_validation_samples_perclass)
    print cmd + '\n'
    if os.system(cmd) != 0: sys.exit(-1)


# # Merge train and extra
inname_train = 'svhn' + '_' + name_prefix + '_' + 'train' + 't'
inname_extra = 'svhn' + '_' + name_prefix + '_' + 'extra'
trainname_train = inname_train + '_train'
valname_train = inname_train + '_val'
trainname_extra = inname_extra + '_train'
valname_extra = inname_extra + '_val'
outname_train = 'svhn_yuv7_train'
outname_val = 'svhn_yuv7_val'
cmd = dsmerge + out \
    + ' ' + outname_train + ' ' + trainname_train + ' ' + trainname_extra
print cmd + '\n'
if os.system(cmd) != 0: sys.exit(-1)

cmd = dsmerge + out \
    + ' ' + outname_val + ' ' + valname_train + ' ' + valname_extra
print cmd + '\n'
if os.system(cmd) != 0: sys.exit(-1)


cmd = 'rm -rf ' + out + '/*traint*'
if os.system(cmd) != 0:sys.exit(-1)
cmd = 'rm -rf ' + out + '/*extra*'
if os.system(cmd) != 0:sys.exit(-1)
cmd = 'rm -rf ' + out + '/*tmpdir*'
if os.system(cmd) != 0:sys.exit(-1)
