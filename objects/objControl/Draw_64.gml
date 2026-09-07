if(fullui) {
	draw_text(20,  0,"FPS = " + string(fps) + " / " + string(fps_real));
} else {
	draw_text(20,  0,"FPS = " + string(fps) + " / " +  string(display_get_frequency()) + " / " + string(fps_real));
	exit;
}

draw_text(20, 20,"Refresh = " + string(display_get_frequency()));
draw_text(20, 40,"Room = " + string(room_width) + " x " + string(room_height));
draw_text(20, 60,"Window = " + string(window_get_width()) + " x " + string(window_get_height()));
draw_text(20, 80,"Position = " + string(window_get_x()) + " x " + string(window_get_y()));
draw_text(20,100,"Display = " + string(display_get_width()) + " x " + string(display_get_height()));
draw_text(20,120,"Rects ... " + string(delta_time));
var _sr = window_get_visible_rects(0,0,0,0);
var _ac = [];
for(var _i=0, _n = array_length(_sr) / 8; _i<_n; _i++) {
	array_copy(_ac, 0, _sr, _i*8, 8);
	draw_text(20,140+(_i*20),"      [" + string(_i) + "] "+ string(_ac));
}
draw_text(20,300,"UpCount = " + string(upcount));
