#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh face_dsprepare.sh"
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
dataroot=/misc/vlgscratch1/LecunGroup/sc3104/datasets/vision/
#dataroot=~/blakeyadata
pascal=$dataroot/pascal
pascalroot=$pascal/VOCdevkit_trainval09/VOCdevkit/VOC2009/
pascalroot_annotations=$pascalroot/Annotations/
root=$dataroot/face/data
out=$dataroot/face/ds/
nopersons_root=$dataroot/nopersons/
nopersons_root_pascal=$nopersons_root/pascal/
detector_name=20100223.002945.face_conf00_eta_.00001
false_positive_root=$dataroot/face/false_positives/$detector_name/

# variables
h=32 #48 64
w=${h}
max=1000 # number of samples in test AND validation set
maxtest=500 # number of samples in the test set
draws=5 # number of train/val sets to draw
precision=float
pp=YnUVn
kernel=7x7 #9
resize=mean #bilinear
nbg=2
bgscales=8,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=all_${id}
namebg=${name}_bg
bgds=pascalbg_${id}
outbg=${out}/${bgds}
partsname=parts${name}
faces=face
fp_name=false_positives_${detector_name}
all_fp=${namebg}_${detector_name}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $pascalroot
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root
mkdir -p $nopersons_root_pascal
mkdir -p $false_positive_root
mkdir -p "$false_positive_root/bg/"

###############################################################################
# fetch datasets
###############################################################################

# # get yale face dataset
# # http://cvc.yale.edu/projects/yalefacesB/yalefacesB.html
# faces_root=$root/${faces}/yale/
# mkdir -p $faces_root
# cd $faces_root
# for i in `seq -f %02.0F 10`
# do
#     for j in `seq -f %02.0F 0 8`
#     do
# 	yale=yaleB${i}_P${j}.tar.gz
# 	wget ftp://plucky.cs.yale.edu/CVC/pub/images/yalefacesB/TarSets/$yale
# 	tar xzvf $yale
#     done
# done

# # get LFW faces (umass)
# # http://vis-www.cs.umass.edu/lfw/
# faces_root=$root/${faces}/lfw/
# mkdir -p $faces_root
# cd $faces_root
# wget http://vis-www.cs.umass.edu/lfw/lfw.tgz
# tar xzvf lfw.tgz
# mv -f `find . -name "*.jpg"` . 2> /dev/null
# # crop pictures to center well on the face
# for fname in `find . -name "*.jpg"`
# do
#     echo "cropping $fname"
#     convert -crop 126x126+62+62 $fname $fname
# done

# # get pascal dataset
# cd $pascal
# wget http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2009/VOCtrainval_11-May-2009.tar
# tar xvf "${pascal}/VOCtrainval_11-May-2009.tar"
# mv -f $pascal/VOCdevkit $pascal/VOCdevkit_trainval09

###############################################################################
# dataset compilations
###############################################################################

# extract background images at different scales from all images parts that
# don't contain persons
~/ebltrunk/bin/dscompile $pascalroot -type pascalbg -precision $precision \
    -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
    -maxperclass $nbg $maxdata -include "person" \
    -channels $pp -resize $resize -kernelsz $kernel \
    $maxdata $ddisplay -annotations $pascalroot_annotations
# debug

# # # extract faces from pascal
# # ~/eblearn/bin/dscompile $pascalroot -type pascal -precision $precision \
# #     -outdir $out -dims ${h}x${w}x3 \
# #     -channels $pp -ignore_difficult -resize $resize -kernelsz $kernel \
# #     -mindims 24x24 -include "head_Frontal" \
# #     -useparts -dname ${namepheads_temp} -usepose \
# #     $maxdata $maxperclass $ddisplay # debug

# # compile background dataset
# ~/eblearn/bin/dscompile ${outbg} -type lush -precision $precision \
#     -outdir ${out} -dname ${bgds}_${nbg} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# # compile regular dataset
# ~/eblearn/bin/dscompile $root -precision $precision \
#     -outdir ${out} -channels $pp -dname $name \
#     -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# # merge normal dataset with background dataset
# ~/eblearn/bin/dsmerge $out ${namebg} ${bgds}_$nbg ${name}

# # # merge pascal faces with regular dataset
# # ~/eblearn/bin/dsmerge $out ${namebgpheads} ${namebg} ${namepheads_temp}

# # split dataset into training and {validation/test}
# ~/eblearn/bin/dssplit $out ${namebg} \
#     ${namebg}_testval_${maxtest}_ \
#     ${namebg}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# # split validation and test
# for i in `seq 1 ${draws}`
# do
# ~/eblearn/bin/dssplit $out ${namebg}_testval_${maxtest}_$i \
#     ${namebg}_test_${maxtest}_$i \
#     ${namebg}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
# done

# # print out information about extracted datasets to check that their are ok
# ~/eblearn/bin/dsdisplay $out/${namebg} -info

###############################################################################
# false positive dataset compilations
###############################################################################

# # extract all pascal full images that do not contain faces
# ~/eblearn/bin/dscompile $pascalroot -type pascalfull -precision $precision \
#     -outdir $nopersons_root_pascal -exclude "person" \
#     $maxdata $ddisplay # debug

# # generate false positives
# cd "${false_positive_root}/bg/" && \
# ~/eblearn/bin/objrec_detect \
#     ~/eblearn/demos/objrec/face/trained/${detector_name}.conf \
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
# cat $0 | mutt $meta_email -s "face dsprepare" -a ${tgz_name}

