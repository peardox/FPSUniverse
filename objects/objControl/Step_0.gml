if(freq != display_get_frequency()) {
	freq = display_get_frequency();
	game_set_speed(freq, gamespeed_fps);
}

if (keyboard_check_pressed(vk_f12)) {
	fullScreenState := !fullScreenState;
	window_set_fullscreen(fullScreenState);
}

if (keyboard_check_pressed(vk_escape)) {
	game_end();
}

if (keyboard_check(vk_up)) {
	yaw += 0.01;
	if(yaw > 1) {
		yaw = 1;
	}
}

if (keyboard_check(vk_down)) {
	yaw -= 0.01;
	if(yaw < -1) {
		yaw = -1;
	}
}

if(wind.update()) {
	upcount++;
}

