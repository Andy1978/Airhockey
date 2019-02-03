## use the manual mm2 mode via udp

pkg load sockets-enh

s = socket(AF_INET, SOCK_DGRAM, 0);

# hard coded in Network.ino
addr = struct("addr", "192.168.4.1", "port", 2222);
c = connect(s, addr)

function set (s, x, y, speed, accel, rx, ry)
  # x [mm], y[mm], speed, accel, robot_x[mm], robot_y[mm] 
  tmp = uint16([x y speed accel rx ry]);
  send (s, ["mm2" typecast(swapbytes(tmp),"char")]);
endfunction

a = 5000;
v = 30000;

set (s, 0, 0, v, a, 0, 0)

r = 100;
for k=1:100

  set (s, 0, 350, v, a, 0, 0)
  pause (0.5)
  set (s, 0, 10, v, a, 0, 0)
  pause (0.5)

endfor
