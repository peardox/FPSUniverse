if(freq != display_get_frequency()) {
	freq = display_get_frequency();
	game_set_speed(freq, gamespeed_fps);
}


if (keyboard_check_pressed(vk_f5)) {
	gpu_set_performance(GPU_PERFORMANCE.LOW_POWER);
}

if (keyboard_check_pressed(vk_f6)) {
	gpu_set_performance(GPU_PERFORMANCE.HIGH_POWER);
}

if (keyboard_check_pressed(vk_f11)) {
	global.modelSelected++;
	if(global.modelSelected >= array_length(global.modelList)) {
		global.modelSelected = 0;
	}
	room_restart();
}

if (keyboard_check_pressed(vk_f12)) {
	fullScreenState := !fullScreenState;
	if(fullScreenState) {
		last_x = window_get_x();
		last_y = window_get_y();
	}
	window_set_fullscreen(fullScreenState);
	if(!fullScreenState && last_x != -1 && last_y != -1) {
		window_set_position(last_x, last_y);
		last_x = -1;
		last_y = -1;
	}
}

if (keyboard_check_pressed(vk_escape)) {
	game_end();
}

if (keyboard_check_pressed(vk_space)) {
	if(objUniverse.rotationSpeed == 0) {
		objUniverse.rotationSpeed = last_speed;
		last_speed = 0;
	} else {
		last_speed = objUniverse.rotationSpeed;
		objUniverse.rotationSpeed = 0;
	}
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

