mkdir prepared_data

# compile background dataset
../../bin/dscompile data/bg -type patch -precision float -outdir prepared_data/bg -scales 4,5,5.5,6,7 -dims 32x32x1 -channels Yp -resize mean -kernelsz 7x7 -maxperclass 6 -maxdata 15000 -nopadded -forcelabel bg 
../../bin/dscompile prepared_data/bg -dname backgroundset -precision float -outdir prepared_data/ -forcelabel bg -nopp
# compile face dataset
../../bin/dscompile data/face -dname faceset -precision float -outdir prepared_data -dims 32x32x1 -channels Yp -resize mean -kernelsz 7x7 -maxdata 15000 -forcelabel face

#merge background and face datasets
cd prepared_data
../../../bin/dsmerge . face+bg backgroundset faceset

# create training and validation sets
../../../bin/dssplit . face+bg face+bg_val face+bg_train -maxperclass 500
cd ..

