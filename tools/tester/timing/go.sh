#!/bin/sh

# directories
face_cmumit=/data/faces/cmumit/
eb=${HOME}/eblearn/

# run timing
$eb/bin/metarun $eb/tester/timing/go.sh
# stop here, rest is configuration for metarun
exit

# meta configuration
meta_name = 2buffers_and_forward
meta_command = "${eb}/bin/detect ${eb}/demos/face/trained/best.conf ${face_cmumit}"
meta_gnuplot_terminal=png
meta_output_dir = ${HOME}/timing/
meta_gnuplot_params ="set xlabel \"frame\"; set ylabel \"time in ms\";"
meta_send_email = 1 # send report by email when done
meta_email = ${myemail} # use env variable "myemail"
