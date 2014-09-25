# # get yale face dataset
# # http://cvc.yale.edu/projects/yalefacesB/yalefacesB.html
# faces_root=$root/${faces}/yale/
# mkdir -p $faces_root
# cd $faces_root
for i in `seq -f %02.0F 10`
do
    for j in `seq -f %02.0F 0 8`
    do
	yale=yaleB${i}_P${j}.tar.gz
	wget ftp://plucky.cs.yale.edu/CVC/pub/images/yalefacesB/TarSets/$yale
	tar xzvf $yale
    done
done
