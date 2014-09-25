% load train/test/extra
filenames = {'test_32x32'; 'train_32x32'; 'extra_32x32'};
for fn_ = 1:length(filenames)
            filename = filenames{fn_};
fprintf('Dumping %s\n', filename);
load(filename);
mkdir(filename);
for i=1:10
        mkdir([filename,'/',int2str(i)]);
    end
    j=0;
length_y=length(y);
for i=1:length(y)
        imwrite(X(:,:,:,i), [filename,'/',int2str(y(i)),'/',int2str(i),'.ppm'],'PPM');
j=j+1;
if(j==4000)
  j=0;
fprintf('Progress: %d of %d images\n',i,length_y);
        end
    end
        movefile([filename,'/',int2str(10)],[filename,'/',int2str(0)]);
end
