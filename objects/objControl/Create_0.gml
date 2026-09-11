freq = 0;
fullScreenState := false;
fullui = true;
fnt = new DynamicFont("Roboto-Regular.ttf");
fnt.create();
wind = new display_window();
wind.set_font(fnt);
wind.set_rows(32);
wind.set_size();

upcount = 0;
yaw = 0;
last_x = -1;
last_y = -1;
last_speed = 0;
gpu =gpu_get_performance();
screens = _screen_info_load();
window_enable_borderless_fullscreen(true);
instance_create_depth(500,500,0,objUniverse);

show_debug_message("Font Count = " + string(fnt.count)); // 354
show_debug_message("Font Max H = " + string(fnt.max_char_height)); // 36
show_debug_message("Font Max W = " + string(fnt.max_char_width)); // 37
