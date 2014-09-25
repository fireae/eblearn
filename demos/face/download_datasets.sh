mkdir data
cd data
###########################################
# FACES
##########################################
# Download CroppedYale Dataset
wget -c http://vision.ucsd.edu/extyaleb/CroppedYaleBZip/CroppedYale.zip
unzip CroppedYale.zip
mv CroppedYale face
rm -rf CroppedYale.zip


cd face
mkdir lfw
cd lfw
# Download lfw faces (umass)
wget -c http://vis-www.cs.umass.edu/lfw/lfw.tgz
tar xzvf lfw.tgz
mv -f `find . -name "*.jpg"` . 2> /dev/null
# crop pictures to center well on the face
for fname in `find . -name "*.jpg"`
do
    echo "cropping $fname"
    convert -crop 126x126+62+62 $fname $fname
done
cd ../../

#########################################
# BACKGROUND
########################################
mkdir bg
cd bg
# get pascal dataset
wget -c http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2009/VOCtrainval_11-May-2009.tar
tar xvf VOCtrainval_11-May-2009.tar
rm VOCtrainval_11-May-2009.tar

mkdir bgimgs
cd VOCdevkit/VOC2009/Annotations

# remove the images in pascal with faces
for i in `ls *.xml`
do
    count=`cat $i |grep person|wc -l`
    if [ $count -eq 0 ]
    then
	filename=`cat $i |grep filename|cut -f2 -d'>'|cut -f1 -d'<'`
	cp ../JPEGImages/$filename ../../../bgimgs
    fi
done
cd ../../../
rm -rf VOCdevkit
