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
draw_text(20, fnt.get_height() * 21,"Font Time : " + string(fnt.time));
draw_text(20, fnt.get_height() * 22,"Font Size : " + string(fnt.size));
draw_text(20, fnt.get_height() * 23,"Font Height : " + string(fnt.max_char_height));
draw_text(20, fnt.get_height() * 24,"ABC Height  :" + string(string_height("ABC")) );
draw_text(20, fnt.get_height() * 25,"Height of " + string(wind.lines) + " = " + string(wind.lines * fnt.max_char_height));
draw_text(20, fnt.get_height() * 26,"Finder : " + string(fnt.finder));
draw_text(20, fnt.get_height() * 27,"Spare Pixels : " + string(wind.spare));
var _scr = screens.screen;
var _sc = 0;
for(var _i=0, _n = array_length(_scr); _i<_n; _i++) {
	var _txt = "Desc[" + string(_i) + "] : " + _scr[_i].desc + ", Refresh : " + string(_scr[_i].refresh) + ", Diagonal : " + string(round(_scr[_i].physical.diagonal / 25.4)) + "\", Width : " + string(_scr[_i].virtual.width()) + ", Height : " + string(_scr[_i].virtual.height()) + ", Top : " + string(_scr[_i].virtual.top) + ", Left : " + string(_scr[_i].virtual.left) + ", Aspect : " + string(_scr[_i].virtual.aspect());
	draw_text(40, fnt.get_height() * (28 + _i), _txt);
	_sc++;
}
for(var _i=28+_sc; fnt.get_height() * _i < display_get_gui_height() - fnt.get_height() - 1; _i++) {
	draw_text(20, fnt.get_height() * _i,"   " + string(_i));
}

