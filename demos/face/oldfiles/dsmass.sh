# # get LFW faces (umass)
# # http://vis-www.cs.umass.edu/lfw/
# faces_root=$root/${faces}/lfw/
# mkdir -p $faces_root
# cd $faces_root
wget http://vis-www.cs.umass.edu/lfw/lfw.tgz
tar xzvf lfw.tgz
mv -f `find . -name "*.jpg"` . 2> /dev/null
# crop pictures to center well on the face
for fname in `find . -name "*.jpg"`
do
    echo "cropping $fname"
    convert -crop 126x126+62+62 $fname $fname
done
