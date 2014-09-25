#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh text_dsprepare.sh"
meta_name=textds
# email to use (use environment variable "myemail")
meta_email=${myemail}

################################################################################
# text dataset compilation
################################################################################

# directories
#dataroot=/data
dataroot=~/budadata/
#dataroot=~/texieradata
#dataroot=~/humairadata
#dataroot=~/blakeyadata
root=$dataroot/text/data
out=$dataroot/text/ds/
nopersons_root=$dataroot/nopersons/
false_positive_root=$dataroot/text/false_positives/

# variables
h=32 #48 64
w=${h}
max=300 # number of samples in the val set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 #9
resize=mean #bilinear
nbg=2
bgscales=8,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=text_${id}
namebg=${name}_bg
#bgds=pascalbg_${id}
bgds=all_mean32x32_ker7_bg
outbg=${out}/${bgds}
partsname=parts${name}
texts=text
fp_name=false_positives_${detector_name}
all_fp=${namebg}_${detector_name}

# debug variables
#maxdata="-maxdata 50"
#maxperclass="-maxperclass 25"
#ddisplay="-disp" # -sleep 50"

# create directories
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root
mkdir -p $false_positive_root
mkdir -p "$false_positive_root/bg/"

###############################################################################
# fetch datasets
###############################################################################

###############################################################################
# dataset compilations
###############################################################################

# # extract background images at different scales from all images parts that
# # don't contain persons
# ~/eblearn/bin/dscompile $pascalroot -type pascalbg -precision $precision \
#     -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
#     -maxperclass $nbg $maxdata -include "person" \
#     -channels $pp -resize $resize -kernelsz $kernel \
#     $maxdata $ddisplay # debug

# # extract texts from pascal
# ~/eblearn/bin/dscompile $pascalroot -type pascal -precision $precision \
#     -outdir $out -dims ${h}x${w}x3 \
#     -channels $pp -ignore_difficult -resize $resize -kernelsz $kernel \
#     -mindims 24x24 -include "head_Frontal" \
#     -useparts -dname ${namepheads_temp} -usepose \
#     $maxdata $maxperclass $ddisplay # debug

# # compile background dataset
# ~/eblearn/bin/dscompile ${outbg} -type lush -precision $precision \
#     -outdir ${out} -dname ${bgds}_${nbg} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
~/eblearn/bin/dscompile $root -precision $precision -type grid\
    -outdir ${out} -channels $pp -dname ${name}_128 -gridsz 128x128 \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    $maxdata $maxperclass $ddisplay # debug

# # compile regular dataset
# ~/eblearn/bin/dscompile $root -precision $precision -type grid\
#     -outdir ${out} -channels $pp -dname ${name}_32 -gridsz 32x32 \
#     -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
~/eblearn/bin/dscompile $root -precision $precision -type grid\
    -outdir ${out} -channels $pp -dname $name -gridsz 64x64 \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    $maxdata $maxperclass $ddisplay # debug

# merge 128 into 64
~/eblearn/bin/dsmerge $out ${name} ${name}_128 ${name}

# # merge 32 into 64
# ~/eblearn/bin/dsmerge $out ${name} ${name}_32 ${name}

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $out ${namebg} ${bgds} ${name}

# # # merge pascal texts with regular dataset
# # ~/eblearn/bin/dsmerge $out ${namebgpheads} ${namebg} ${namepheads_temp}

# split dataset into training and validation
~/eblearn/bin/dssplit $out ${namebg} \
    ${namebg}_val_ \
    ${namebg}_train_ -maxperclass ${max} -draws $draws

# print out information about extracted datasets to check that their are ok
~/eblearn/bin/dsdisplay $out/${namebg} -info

###############################################################################
# false positive dataset compilations
###############################################################################

# # extract all pascal full images that do not contain texts
# ~/eblearn/bin/dscompile $pascalroot -type pascalfull -precision $precision \
#     -outdir $nopersons_root_pascal -exclude "person" \
#     $maxdata $ddisplay # debug

# # generate false positives
# cd "${false_positive_root}/bg/" && \
# ~/eblearn/bin/objrec_detect \
#     ~/eblearn/demos/objrec/text/trained/${detector_name}.conf \
#     ${nopersons_root} \
# && cd -

# # compile false positive dataset
# ~/eblearn/bin/dscompile ${false_positive_root} -type lush \
#     -precision ${precision} -input_precision ${precision} -outdir ${out} \
#     -dname ${fp_name} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# # merge normal dataset with background dataset
# ~/eblearn/bin/dsmerge $out ${all_fp} ${fp_name} ${namebg}

# # split dataset into training and {validation/test}
# ~/eblearn/bin/dssplit $out ${all_fp} \
#     ${all_fp}_testval_${maxtest}_ \
#     ${all_fp}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# # split validation and test
# for i in `seq 1 ${draws}`
# do
# ~/eblearn/bin/dssplit $out ${all_fp}_testval_${maxtest}_$i \
#     ${all_fp}_test_${maxtest}_$i \
#     ${all_fp}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
# done

# # print out information about extracted datasets to check that their are ok
# ~/eblearn/bin/dsdisplay $out/${all_fp} -info

# ###############################################################################
# # reporting
# ###############################################################################

# # email yourself the results
# here=`pwd`
# base="`basename ${here}`"
# tgz_name="logs_${base}.tgz"
# tar czvf ${tgz_name} out*.log
# cat $0 | mutt $meta_email -s "text dsprepare" -a ${tgz_name}

