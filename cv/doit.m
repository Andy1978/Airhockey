graphics_toolkit fltk

addpath('~/src/mexopencv');

warning ("off", "Octave:shadowed-function")

img = imread ("./png/0013.png");

pkg load image

#kern = rand (5);
#out = cv.filter2D(img, kern);  % with package name 'cv'

out = cv.cvtColor(img, 'RGB2HSV');

# grüner puck
gp = out (218, 354, :)
#gp = out (173, 384, :)

#image(hsv2rgb (out))

#image (img);

#hold on
#plot (354, 218, "ro")
#plot (384, 173, "ro")
#hold off

#image (out);

lowerb = squeeze (gp - 20);
upperb = squeeze (gp + 20);
    
mask = cv.inRange(out, lowerb, upperb);

% apply mask
out2 = cv.copyTo(out, 'Mask',mask);

image(hsv2rgb (out2))
#imagesc (mask)



