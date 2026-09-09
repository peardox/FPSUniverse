fullScreenState := false;
window_enable_borderless_fullscreen(true);
/*
if(window_get_fullscreen()) {
	room_width = display_get_width();
	room_height = display_get_height();
	fullScreenState := true;
} else {
	room_width = window_get_width();
	room_height = window_get_height();
}
*/
	room_width = display_get_width();
	room_height = display_get_height();
	fullScreenState := true;
	window_set_fullscreen(fullScreenState);
surface_resize(application_surface, room_width, room_height);
display_set_gui_size(room_width, room_height);
