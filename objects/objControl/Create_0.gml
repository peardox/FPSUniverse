freq = 0;
fullScreenState := false;
fullui = true;
wind = new display_window();
upcount = 0;
yaw = 0;
last_x = -1;
last_y = -1;
last_speed = 0;

window_enable_borderless_fullscreen(true);
instance_create_depth(500,500,0,objUniverse);