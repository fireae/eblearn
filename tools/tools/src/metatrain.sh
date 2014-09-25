#!/bin/sh

# h=96
# w=48
# chans=1
# traindsname=ped_${name}_mean${h}x${w}_ker7_bg_train
# valdsname=ped_${name}_mean${h}x${w}_ker7_bg_val
# name=toto
# ebl=$HOME/eblearn/
# machine=${HOSTNAME}a
# eblearnbin0=$ebl/bin/
# metaconf_name=${name}_meta.conf
# # max number of false positives to extract per iteration
# save_max=15660
# # max number of false positives to extract per full image
# save_max_per_frame=10
# # number of threads to use duing false positive extraction
# nthreads=5
# # maximum number of retraining iterations
# maxiteration=3
# npasses=3
# max_scale=4
################################################################################
# meta commands
################################################################################

# required variables
meta_command="sh train.sh"

# optional meta variables ######################################################

# directory where to write outputs of all processes
meta_output_dir=${out}
# name of this meta job
meta_name=${name}_${machine}
# emailing results or not
meta_send_email=1
# email to use (use environment variable "myemail")
meta_email=${myemail}
# send email with this frequency (if email_iters not defined)
meta_email_period=1

# interval in seconds to analyze processes output, and to check who is alive.
meta_watch_interval=120
# variables to minimize, process and iteration with lowest value will
# be used to report best weights, or start consequent training
meta_minimize=i
# send n best answers that minimize meta_minimize's value
meta_send_best=5

################################################################################
# variables
################################################################################

# directories
#tstamp=`date +"%Y%m%d.%H%M%S"`
tstamp=20100727.073128
xpname=${meta_name}_${tstamp}
root=~/${machine}data/ped/${name}/
dataroot=$root/ds/
out=$root/out/$xpname/
eblearnbin=${out}/bin/
negatives_root=$root/train/full/
input_max=1000
# variables

metaconf0=$ebl/demos/pedestrians/${name}/${metaconf_name}
metaconf=${out}/${metaconf_name}

precision=float

# threshold will be decremented at each iter until -.95
threshold=-.5

# split ratio of validation over training
ds_split_ratio=".1"
draws=3

###############################################################################
# functions
###############################################################################

# stop if error
check_error() {
    if [ $1 -ne 0 ]
    then
	echo "Error, exiting."
	exit -1
    fi
}

# extract false positives
extract_falsepos() {
    # arguments ###############################################################
    bestconf=$1
    save_maximum=$2
    save_max_per_frame=$3
    bestout=$4 # directory of trained weights
    input_max=$5
    thresh=$6
    weights=$7
    neg_root=$8 # directory where to find negative images
    eblearnbin=$9
    nthreads=${10}
    npasses=${11}
    minsc=${12}
    maxsc=${13}
    tstamp=${14}
    name=${15}
    display=${16}
    mindisplay=${17}
    savevideo=${18}
    scaling=${19}
    echo
    echo -e "Arguments:\nbestconf=${bestconf}\nsave_max=${save_maximum}"
    echo -e "save_max_per_frame=${save_max_per_frame}\nbestout=${bestout}\ninput_max=${input_max}"
    echo -e "threshold=${thresh}\nweights=${weights}\nnegatives_root=${neg_root}"
    echo -e "eblearnbin=${eblearnbin}\nnthreads=${nthreads}\nnpasses=${npasses}"
    echo -e "max_scale=${max_scale}\ntstamp=${tstamp}\nname=${name}\nscaling=${scaling}"
    echo
    # function body ###########################################################

    # add new variables to best conf
    # force saving detections
    echo "save_detections = 1" >> $bestconf
    # do not save video
    echo "save_video = ${savevideo}" >> $bestconf
    echo "display = ${display}" >> $bestconf
    echo "minimal_display = ${mindisplay}" >> $bestconf
    echo "save_max = ${save_maximum}" >> $bestconf
    echo "save_max_per_frame = ${save_max_per_frame}" >> $bestconf
    # add directory where to find trained files
    echo "root2 = ${bestout}" >> $bestconf
    # limit input size 
    echo "input_max = ${input_max}" >> $bestconf
    echo "threshold = ${thresh}" >> $bestconf
    # set weights to retrain: same as this conf
    echo "retrain_weights = \${weights}" >> $bestconf
    # set where to find full images
    echo "input_dir = ${neg_root}/" >> $bestconf
    # send report every 1000 frames processed
    echo "meta_email_iters = " >> $bestconf
    echo "meta_email_period = 1000" >> $bestconf
    # override train command by detect command
    echo >> $bestconf
    echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/detect\"" >> $bestconf
    echo "meta_name = ${name}" >> $bestconf
    # set multi threads for detection
    echo "nthreads = ${nthreads}" >> $bestconf
    # pass n times on the dataset to make sure we reach the desired amount of fp
    echo "input_npasses = ${npasses}" >> $bestconf
    # randomize list of files to extract fp from
    echo "input_random = 1" >> $bestconf
    # no input list
    echo "input_list = " >> $bestconf
    # keep all detected bbox even overlapping ones
    echo "nms = 0" >> $bestconf
    # downsample to n times input (capped by input_min)
    echo "min_scale = ${minsc}" >> $bestconf
    # oversample to n times input (capped by input_max)
    echo "max_scale = ${maxsc}" >> $bestconf
    echo "scaling = ${scaling}" >> $bestconf
    echo "scaling_type = 3" >> $bestconf
    # do not change bbox
    echo "bbhfactor = 1" >> $bestconf
    echo "bbwfactor = 1" >> $bestconf
    # not zpad allowed
    echo "hzpad = 0" >> $bestconf
    echo "wzpad = 0" >> $bestconf
    # detect mode
    echo "run_type = detect" >> $bestconf
    # in scaler mode, use output as confidence
    echo "scaler_answer1_rawconf = 1" >> $bestconf
    # no specific input list
    echo "input_list = " >> $bestconf
    # do not use scale predictions
    echo "scaler_mode = 0" >> $bestconf
    # start parallelized extraction
    echo "executing: ${eblearnbin}/metarun $bestconf -tstamp ${tstamp}"
    ${eblearnbin}/metarun $bestconf -tstamp ${tstamp} -reset_progress
    check_error $? 
}

# add new data to existing sets
compile_data() {
    # arguments ###############################################################
    eblearnbin=$1
    falsepos_dir=$2
    precision=$3
    dataroot=$4
    h=$5
    w=$6
    chans=$7
    draws=$8
    traindsname=$9
    valdsname=${10}
    ds_split_ratio=${11}
    step=${12} # current step number
    compile_step=${13} # previous data compilation step
    # function body ###########################################################

    if [ $compile_step -gt 0 ] # first dataset doesn't have a prefix
    then
	prefix="${compile_step}_"
    else
	prefix=""
    fi
    
    # recompile data from last output directory which should contain 
    # all false positives
    # note: no need to preprocess, .mat should already be preprocessed
    ${eblearnbin}/dscompile ${falsepos_dir} -precision ${precision} \
	-outdir ${dataroot} -forcelabel bg -dname ${step}_allfp \
	-dims ${h}x${w}x${chans} \
	-image_pattern ".*[.]mat" -mindims ${h}x${w}x${chans}
    check_error $? 

    # get dataset size
    dssize=`${eblearnbin}/dsdisplay ${dataroot}/${step}_allfp -size`
    echo "false_positives = ${dssize}"
    echo "ds_split_ratio = ${ds_split_ratio}"
    valsize=`echo "(${dssize} * ${ds_split_ratio})/1" | bc`
    echo "valsize = ${valsize}"
    
    # print out information about extracted dataset to check it is ok
    ${eblearnbin}/dsdisplay $dataroot/${step}_allfp -info
    check_error $? 
    
    # split dataset into training and validation
    ${eblearnbin}/dssplit ${dataroot} ${step}_allfp \
	${step}_allfp_val_ ${step}_allfp_train_ \
	-maxperclass ${valsize} -draws $draws
    check_error $? 

    # assume only 1 draw
    ${eblearnbin}/dsmerge ${dataroot} ${step}_${traindsname} \
	${prefix}${traindsname} ${step}_allfp_train_1
    check_error $? 
    ${eblearnbin}/dsmerge ${dataroot} ${step}_${valdsname} \
	${prefix}${valdsname} ${step}_allfp_val_1
    check_error $? 

#     # merge new datasets into previous datasets: training
#     for i in `seq 1 $draws`
#     do
# 	${eblearnbin}/dsmerge ${dataroot} ${step}_${traindsname}_${i} \
# 	    ${prefix}${traindsname}_${i} ${step}_allfp_train_${i}
# 	check_error $? 
#     done

#     # merge new datasets into previous datasets: validation
#     for i in `seq 1 $draws`
#     do
# 	${eblearnbin}/dsmerge ${dataroot} ${step}_${valdsname}_${i} \
# 	    ${prefix}${valdsname}_${i} ${step}_allfp_val_${i} 
# 	check_error $? 
#     done
}

# retrain a trained network
retrain() {
    # arguments ###############################################################
    bestweights=$1
    eblearnbin=$2
    metaconf=$3
    name=$4
    tstamp=$5
    bestconf=$6
    compile_step=$7 # last data compilation step id
    traindsname=$8
    valdsname=$9
    # function body ###########################################################
    # retrain on old + new data
    echo "Retraining from best previous weights: ${bestweights}"
    # add last weights and activate retraining from those
    echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/train\"" >> $metaconf
    echo "retrain = 1" >> $metaconf
    echo "retrain_weights = ${bestweights}" >> $metaconf
    echo "train = ${compile_step}_${traindsname}" >> $metaconf
    echo "val = ${compile_step}_${valdsname}" >> $metaconf
    echo "run_type = train" >> $metaconf
#     echo "train = ${compile_step}_${traindsname}_\${ds}" >> $metaconf
#     echo "val = ${compile_step}_${valdsname}_\${ds}" >> $metaconf
    echo "meta_name = ${name}" >> $metaconf
    # send report at specific training iterations
    echo "meta_email_iters = 0,1,2,3,4,5,7,10,15,20,30,50,75,100,200" >> \
	$metaconf
    ${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
    check_error $? 
}

print_step() {
    # arguments ###############################################################
    step=$1
    conf=$2
    lastname=$3
    lastdir=$4
    type=$5
    iter=$6
    maxiteration=$7
    # function body ###########################################################
    echo "_________________________________________________________________"
    echo "step ${step}: ${type} with metaconf ${conf}"
    echo "lastname: ${lastname}"
    echo "lastdir: ${lastdir}"
    echo "maxiteration: ${maxiteration} iter: ${iter}"
    echo "i=`expr ${maxiteration} - ${iter}`"
}   

metatrain() {
    # arguments ###############################################################
    minstep=$1
    maxiteration=$2
    out=$3
    eblearnbin=$4
    negatives_root=$5
    metaconf=$6
    metaconf0=$7
    meta_name=$8
    tstamp=$9
    save_max=${10}
    save_max_per_frame=${11}
    input_max=${12}
    threshold=${13}
    nthreads=${14}
    npasses=${15}
    min_scale=${16}
    max_scale=${17}
    precision=${18}
    dataroot=${19}
    h=${20}
    w=${21}
    chans=${22}
    draws=${23}
    traindsname=${24}
    valdsname=${25}
    ds_split_ratio=${26}
    display=${27}
    mindisplay=${28}
    savevideo=${29}
    min_threshold=${30}
    step_threshold=${31}
    scaling=${32}
    echo
    echo -e "Arguments:\nminstep=${minstep}\nmaxiteration=${maxiteration}\nout=${out}"
    echo -e "eblearnbin=${eblearnbin}\nnegatives_root=${negatives_root}\nmetaconf=${metaconf}"
    echo -e "metaconf0=${metaconf0}\nmeta_name=${meta_name}\ntstamp=${tstamp}"
    echo -e "save_max=${save_max}\nsave_max_per_frame=${save_max_per_frame}"
    echo -e "input_max=${input_max}\nthreshold=${threshold}\nnthreads=${nthreads}"
    echo -e "npasses=${npasses}\nmin_scale=${min_scale}\nmax_scale=${max_scale}\nprecision=${precision}"
    echo -e "dataroot=${dataroot}\nh=${h}\nw=${w}\nchans=${chans}\ndraws=${draws}"
    echo -e "traindsname=${traindsname}\nvaldsname=${valdsname}"
    echo -e "ds_split_ratio=${ds_split_ratio}"
    echo -e "scaling=${scaling}"
    echo
    # function body ###########################################################
    
    # create directories
    mkdir -p $out
    mkdir -p $eblearnbin
    mkdir -p $negatives_root

    # copy binaries
    cp $eblearnbin0/* $eblearnbin/
    # set path to libraries locally
    export LD_LIBRARY_PATH=${eblearnbin}

    # make a copy of meta conf and override its output dir
    cp $metaconf0 $metaconf
    echo "" >> $metaconf # new line
    echo "meta_output_dir = ${out}" >> $metaconf

    step=0
    compile_step=0
    stepstr=`printf "%02d" ${step}`
    # initial training
    name=${meta_name}_${stepstr}_training
    lastname=${tstamp}.${name}
    lastdir=${out}/${lastname}
    if [ $step -ge $minstep ]; then
	print_step $step $metaconf $lastname $lastdir "training" \
	    0 $maxiteration
	echo -n "meta_command = \"export LD_LIBRARY_PATH=" >> $metaconf
	echo "${eblearnbin} && ${eblearnbin}/train\"" >> $metaconf
	echo "meta_name = ${name}" >> $metaconf
	${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
    fi
    step=`expr ${step} + 1` # increment step
    stepstr=`printf "%02d" ${step}`
    
    # looping on retraining on false positives
    for iter in `seq 1 ${maxiteration}`
      do	
        echo "_________________________________________________________________"
        echo "iteration ${iter}"
        # find path to latest metarun output: get directory with latest date
	bestout=${lastdir}/best/01/
        # find path to best conf (there should be only 1 conf per folder)
	bestconf=`ls ${bestout}/*.conf`
        # find path to best weights (there should be only 1 weights per folder)
	bestweights=`find ${bestout} -name "*_net[0-9]*[0-9].mat"`

        # false positives
	name=${meta_name}_${stepstr}_falsepos
	lastname=${tstamp}.${name}
	lastdir=${out}/${lastname}
	if [ $step -ge $minstep ]; then
            # decrement threshold, capping at 0.1
	    threshold=`echo "thr=${threshold} - ${step_threshold}; if (thr < ${min_threshold}){ thr = ${min_threshold};}; print thr" | bc`

	    print_step $step $bestconf $lastname $lastdir "false positives" \
		$iter $maxiteration
            # zoomed in positives (forbid zooming out)
	    if [ -d $negatives_root/zoomed_in ] ; then
		save_max_tmp=`expr $save_max / 4` # limit zoomed in to 1/4th
		extract_falsepos $bestconf $save_max_tmp $save_max_per_frame $bestout \
		    $input_max $threshold $bestweights $negatives_root/zoomed_in $eblearnbin \
		    $nthreads $npasses $min_scale $max_scale $tstamp $name \
		    $display $mindisplay $savevideo $scaling
	    fi
            # zoomed out positives (forbid zooming in)
	    if [ -d $negatives_root/zoomed_out ] ; then
		save_max_tmp=`expr $save_max / 4` # limit zoomed in to 1/4th
		extract_falsepos $bestconf $save_max_tmp $save_max_per_frame $bestout \
		    $input_max $threshold $bestweights $negatives_root/zoomed_out $eblearnbin \
		    $nthreads $npasses $min_scale $max_scale $tstamp $name $display $mindisplay $savevideo \
		    $scaling
	    fi
            # full size negatives (zooming in/out allowed)
	    saved=`ls -R $lastdir | grep -e ".mat$" | wc -l` # saved so far
	    save_max_tmp=`expr $save_max - $saved` # limit to remaining max
	    extract_falsepos $bestconf $save_max_tmp $save_max_per_frame $bestout \
		$input_max $threshold $bestweights $negatives_root/full $eblearnbin \
		$nthreads $npasses $min_scale $max_scale $tstamp $name $display \
		$mindisplay $savevideo $scaling
	fi
	step=`expr ${step} + 1` # increment step
	stepstr=`printf "%02d" ${step}`
	
        # recompile data
	if [ $step -ge $minstep ]; then
	    print_step $step $metaconf $lastname $lastdir "data compilation" \
		$iter $maxiteration
	    compile_data $eblearnbin $lastdir $precision $dataroot \
		$h $w $chans $draws $traindsname $valdsname $ds_split_ratio \
		$step $compile_step
	fi
	compile_step=$step # remember last data compilation step
	step=`expr ${step} + 1` # increment step
	stepstr=`printf "%02d" ${step}`
    
        # retrain
	name=${meta_name}_${stepstr}_retraining
	lastname=${tstamp}.${name}
	lastdir=${out}/${lastname}
	if [ $step -ge $minstep ]; then
	    print_step $step $metaconf $lastname $lastdir "retraining" \
		$iter $maxiteration
	    retrain $bestweights $eblearnbin $metaconf $name $tstamp \
		$bestconf $compile_step $traindsname $valdsname
	fi
	step=`expr ${step} + 1` # increment step
	stepstr=`printf "%02d" ${step}`
    done
}

###############################################################################
# training
###############################################################################

# metatrain 4 $maxiteration $out $eblearnbin $negatives_root $metaconf $metaconf0 \
#     $meta_name $tstamp $save_max $save_max_per_frame $input_max $threshold \
#     $nthreads $npasses $max_scale $precision $dataroot $h $w $chans $draws \
#     $traindsname $valdsname $ds_split_ratio
