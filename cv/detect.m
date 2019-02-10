pkg load image
pkg load video

fns = glob ("png/*.png");

k = 1;

img = imread (fns{k});

#printf ("Farbe grüner Puck bestimmen und mit RETURN abschließen...\n");
#[puck_x, puck_y] = ginput ();
#printf ("Farbe rote Kreise bestimmen und mit RETURN abschließen...\n");
#[circle_x, circle_y] = ginput ();

load ("cache.log");

#mean_img (img, puck_x, puck_y)

figure (1)
imshow (img)
hold on
plot(puck_x,   puck_y, "ro");
plot(circle_x, circle_y, "go");
hold off

pc = mean_img (img, puck_x, puck_y);     # puck color
cc = mean_img (img, circle_x, circle_y); # circle color

## radius etwa 68px

#hold on
#centers = imfindcircles (img, [45 72])
#plot (centers(:,1), centers(:,2), "mo")
#hold off


incp = [121 444; 316 9; 634 6; 788 441];

hold on
plot (incp(:,1), incp(:,2), 'or')
hold off

udata = [min(incp(:,1)) max(incp(:,1))];
vdata = [min(incp(:,2)) max(incp(:,2))];

# Der Tisch hat innen 89 x 43cm
h = 890;
w = 430;
outcp = [0 h; 0 1; w 1; w h];
 
T = maketform ('projective', incp, outcp);


fn="test_avifile1.avi";
m = avifile(fn, "codec", "mpeg4");


for k=1:numel(fns)

  img = imread (fns{k});

  #[im2 xdata ydata] = imtransform (img, T, 'udata', udata,
  #                                'vdata', vdata, 'fillvalues', 1 );

  [im2 xdata ydata] = imtransform (img, T, 'udata', udata,
                                  'vdata', vdata);


  im2 = im2 (30:1049, 400:929, :);
  #addframe(m, im2);

  addframe(m, im2double(im2));

  #figure (2)
  #imh = imshow (im2); set (imh, 'xdata', xdata, 'ydata', ydata) 
  #set (gca, 'xlim', xdata, 'ylim', ydata)
  #axis on, hold on, xlabel ('Projection');
  #plot (outcp(:,1), outcp(:,2), 'or')
  #hold off
endfor

clear m
