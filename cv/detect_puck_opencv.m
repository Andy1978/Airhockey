run ("~/src/mexopencv/.ci/cvsetup.m")

cam = cv.VideoCapture("2019-02-05-200014.webm");

# Vorspulen...
cam.PosFrames = 50;

i1 = i2 = [];
subplot (2, 1, 1);

src = [134 448;310 8;634 6.4;785 440];
dst = [0 544; 0 7; 400 7; 400 544];
T = cv.getPerspectiveTransform(src, dst);

vid = cv.VideoWriter('myvideo.mp4', [405,544], "FourCC", "X264", "FPS", 15);

while (1)
  frame = cam.read();

  if (! isempty (frame))
 

    frame = cv.warpPerspective(frame, T);
    frame = frame (1:544,1:405,:);
 
    # per ginput aus dem HSV Bild:
    #[x,y]=ginput(1); squeeze(gr(round(y),round(x),:))
    t = [70 180 90];
    gr = cv.cvtColor(frame, 'RGB2HSV');

    lower = [60 100 30];
    upper = [80 255 150];

    # Versuch die Farben darzustellen
    #n = 100;
    #[s, v]=meshgrid (linspace(lower(2),upper(2),n), linspace(lower(3),upper(3),n));
    #img = cat (3, 70*ones(n),s,v);
    #image(hsv2rgb(uint8(img)))

    mask = cv.inRange (gr, lower, upper);
    mask = imopen (mask, strel("disk", 4, 0));
    
    #contours = cv.findContours(mask);
    
    #detector = cv.SimpleBlobDetector();
    #keypoints = detector.detect(mask);
    
    #mask = imdilate (mask, ones(8));
    #rct = cv.boundingRect(mask);

    [labels, N, stats, centroids] = cv.connectedComponents (mask);

    if (isempty (i1))
      subplot (2, 1, 1);
      i1=imshow(frame);
    else
      for k=2:N
        frame = cv.rectangle(frame, stats(k, 1:4), "Thickness", 3, "Color", [0 255 0]);
      endfor
      #for k=1:N
      #  frame = cv.drawMarker(frame, centroids(k, :),
      #                        "Thickness", 5,
      #                        "Color", [0 255 0],
      #                        "MarkerType", "s");
      #endfor
      #frame = cv.drawContours(frame, contours);
      #frame = cv.drawKeypoints(frame, keypoints);
      set(i1, "cdata", frame);
      vid.write(frame);
    endif

    if (isempty (i2))
      subplot (2, 1, 2);
      i2=imshow(mask);
    else
      set(i2, "cdata", mask);
    endif

    #rectangle('Position',  rct, 'EdgeColor', 'g');

    drawnow;
    pause(0.05);

  else
    break;
  endif
endwhile

clear vid

