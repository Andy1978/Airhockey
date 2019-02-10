function r = mean_img (img, x, y)

  n = numel (x);
  assert (n, numel (y));
  assert (all (x <= columns (img) & x > 0 & y <= rows (img) & y > 0));
  x = repelem (round (x(:)), 3);
  y = repelem (round (y(:)), 3);
  idx = sub2ind (size (img), y, x, repmat(1:3, 1, n)(:));

  img = reshape (img(idx),3 ,[]);
  
  r = mean (img, 2);

  s = std (img, 0, 2);
  if (any (s > 30))
    warning ("std is very big, please double check...");
  endif

endfunction
