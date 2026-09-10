enum GPU_PERFORMANCE {
	WINDOWS_DEFAULT,
	LOW_POWER,
	HIGH_POWER
}

function normalized_exename() {
	if((os_type == os_windows) || (os_type == os_xboxseriesxs) || (os_type == os_gdk)) {
		return string_replace_all(parameter_string(0), "/", "\\");
	} else {
		return string_replace_all(parameter_string(0), "\\", "/");
	}
}

function gpu_set_performance(_perf) {
	if(os_type == os_windows) {
		var _res = false;
		switch(_perf) {
			case GPU_PERFORMANCE.WINDOWS_DEFAULT:
				_res = __gpu_set_performance(normalized_exename(), GPU_PERFORMANCE.WINDOWS_DEFAULT);
				show_debug_message("=======> GPU Set to WINDOWS_DEFAULT <=======");
				break;
			case GPU_PERFORMANCE.LOW_POWER:
				_res = __gpu_set_performance(normalized_exename(), GPU_PERFORMANCE.LOW_POWER);
				show_debug_message("=======> GPU Set to LOW_POWER <=======");
				break;
			case GPU_PERFORMANCE.HIGH_POWER:
				_res = __gpu_set_performance(normalized_exename(), GPU_PERFORMANCE.HIGH_POWER);
				show_debug_message("=======> GPU Set to HIGH_POWER <=======");
				break;
			default:
				throw("Invalid Performance Parameter passed to gpu_set_performance");
				break;
		}
	
		return _res;
	} else {
		return false;
	}
}

function gpu_get_performance() {
	if(os_type == os_windows) {
		return __gpu_get_performance(normalized_exename());
	} else {
		return -1;
	}
	
}
//show_debug_message("=======> Game Start <======= <===> GPU Set <===>");