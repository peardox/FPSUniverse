draw_text(20,  0,"Refresh = " + string(display_get_frequency()));
draw_text(20, 20,"FPS = " + string(fps) + " / " + string(fps_real));
draw_text(20, 40,"Window = " + string(window_get_width()) + " x " + string(window_get_height()));
draw_text(20, 60,"Position = " + string(window_get_x()) + " x " + string(window_get_y()));
draw_text(20, 80,"Display = " + string(display_get_width()) + " x " + string(display_get_height()));
draw_text(20,100,"Rects ... ");
var _sr = window_get_visible_rects(0,0,0,0);
var _ac = [];
for(var _i=0, _n = array_length(_sr) / 8; _i<_n; _i++) {
	array_copy(_ac, 0, _sr, _i*8, 8);
	draw_text(20,120+(_i*20),"      [" + string(_i) + "] "+ string(_ac));
}