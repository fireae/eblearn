#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh face_mitcbcl_dsprepare.sh"
meta_name=faceds
# emailing results or not
meta_send_email=1
# email to use (use environment variable "myemail")
meta_email=${myemail}

################################################################################
# face dataset compilation
################################################################################

# directories
#dataroot=/data
#dataroot=~/texieradata
dataroot=~/humairadata
#dataroot=~/blakeyadata
root=$dataroot/face/data_mitcbcl
out=$dataroot/face/ds_mitcbcl/

# variables
h=19
w=${h}
max=400 # number of samples in the validation set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 #9
resize=mean #bilinear

# names
id=${resize}${h}x${w}_ker${kernel}
name=mitcbcl_${id}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $root
mkdir -p $out

###############################################################################
# fetch datasets
###############################################################################

###############################################################################
# dataset compilations
###############################################################################

# compile regular dataset
~/eblearn/bin/dscompile $root -precision $precision \
    -outdir ${out} -channels $pp -dname $name \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    $maxdata $maxperclass $ddisplay # debug

# split dataset into training and validation
~/eblearn/bin/dssplit $out ${name} \
    ${name}_val_ ${name}_train_ -maxperclass ${max} -draws $draws

# print out information about extracted datasets to check that their are ok
~/eblearn/bin/dsdisplay $out/${name} -info
