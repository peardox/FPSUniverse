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

if(InputPressed(INPUT_VERB.ACTION)) {
	global.modelSelected++;
	if(global.modelSelected >= array_length(global.modelList)) {
		global.modelSelected = 0;
	}
	room_restart();
}

if(InputPressed(INPUT_VERB.SPECIAL)) {
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

if(InputPressed(INPUT_VERB.EXIT)) {
	game_end();
}

if(InputPressed(INPUT_VERB.ACCEPT)) {
	if(objUniverse.rotationSpeed == 0) {
		objUniverse.rotationSpeed = last_speed;
		last_speed = 0;
	} else {
		last_speed = objUniverse.rotationSpeed;
		objUniverse.rotationSpeed = 0;
	}
}

if(InputPressed(INPUT_VERB.UP)) {
	yaw += 0.01;
	if(yaw > 1) {
		yaw = 1;
	}
}

if(InputPressed(INPUT_VERB.DOWN)) {
	yaw -= 0.01;
	if(yaw < -1) {
		yaw = -1;
	}
}

if(wind.update()) {
	upcount++;
}

