fnt.activate();

if(fullui) {
	draw_text(20, fnt.get_height() *  0,"FPS = " + string(fps) + " / " + string(fps_real) + ", build = " + string(global.__build_id));
} else {
	draw_text(20, fnt.get_height() *  0,"FPS = " + string(fps) + " / " +  string(display_get_frequency()) + " / " + string(fps_real) + ", build = " + string(global.__build_id));
	exit;
}

draw_text(20, fnt.get_height() * 1,"Refresh = " + string(display_get_frequency()));
draw_text(20, fnt.get_height() * 2,"Room = " + string(room_width) + " x " + string(room_height));
draw_text(20, fnt.get_height() * 3,"Window = " + string(window_get_width()) + " x " + string(window_get_height()));
draw_text(20, fnt.get_height() * 4,"Position = " + string(window_get_x()) + " x " + string(window_get_y()));
draw_text(20, fnt.get_height() * 5,"Display = " + string(display_get_width()) + " x " + string(display_get_height()));
draw_text(20, fnt.get_height() * 6,"Rects ... " + string(delta_time));
var _sr = window_get_visible_rects(0,0,0,0);
var _ac = [];
for(var _i=0, _n = array_length(_sr) / 8; _i<_n; _i++) {
	array_copy(_ac, 0, _sr, _i*8, 8);
	draw_text(20, fnt.get_height() * (_i + 7),"      [" + string(_i) + "] "+ string(_ac));
}

draw_text(20, fnt.get_height() * 19,"FS = " + string(wind.isFullscreen));
draw_text(20, fnt.get_height() * 20,"GPU = " + string(gpu));
draw_text(20, fnt.get_height() * 21,"Screens = " + string(screens));
draw_text(20, fnt.get_height() * 22,"Font Time : " + string(fnt.time));
draw_text(20, fnt.get_height() * 23,"Font Size : " + string(fnt.size));
draw_text(20, fnt.get_height() * 24,"Font Height : " + string(fnt.max_char_height));
draw_text(20, fnt.get_height() * 25,"ABC Height  :" + string(string_height("ABC")) );
draw_text(20, fnt.get_height() * 26,"Height of " + string(wind.lines) + " = " + string(wind.lines * fnt.max_char_height));
draw_text(20, fnt.get_height() * 27,"   27");
draw_text(20, fnt.get_height() * 28,"   28");
draw_text(20, fnt.get_height() * 29,"   29");
draw_text(20, fnt.get_height() * 30,"   30");
draw_text(20, fnt.get_height() * 31,"   31");
//draw_text(20, fnt.get_height() * 32,"   32");
