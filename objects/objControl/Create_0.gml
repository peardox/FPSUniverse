// show_debug_overlay(true);
freq = 0;
fullScreenState := true;
fullui = true;

fnt = new Font("Roboto-Regular.ttf");
fnt.create();

wind = new display_window();
wind.set_font(fnt);
wind.set_lines(32);
wind.set_size();

upcount = 0;
yaw = 0;
last_x = window_get_x();
last_y = window_get_y();

last_speed = 0;
gpu =gpu_get_performance();
screens = _screen_info_load();
show_debug_message(json_stringify(screens, true));
window_enable_borderless_fullscreen(true);
instance_create_depth(500,500,0,objUniverse);

show_debug_message("Font Count = " + string(fnt.count)); // 354
show_debug_message("Font Max H = " + string(fnt.max_char_height)); // 36
show_debug_message("Font Max W = " + string(fnt.max_char_width)); // 37

window_set_fullscreen(fullScreenState);
instance_create_depth(wind.width / 2,wind.height / 2,-100,objCorner);

wind.update();
