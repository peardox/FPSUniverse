#macro DELTA_SECONDS (delta_time * 0.000001)
#macro MAX_SCREEN_RES 8192

enum animate { CAMERA , MODEL }

global.modelSelected = 0;
global.modelList = [
	"Sky/Sky.bbmod",
	"World/World.bbmod"
	]

function display_window() constructor {
	self.width = window_get_width();
	self.height = window_get_height();

	static update = function() {
		if( (self.width = window_get_width()) &&
			(self.height = window_get_height()) ) {
			return false;
		}
			
		if(window_get_width() >= MAX_SCREEN_RES) {
			var _asp = window_get_width() / window_get_height();
			self.width = MAX_SCREEN_RES;
			self.height = self.width * _asp;
		} else {
			self.width = window_get_width();
			self.height = window_get_height();
		}
		
		room_width = self.width;
		room_height = self.height;
		surface_resize(application_surface, room_width, room_height);
		display_set_gui_size(self.width, self.height);
		
		return true;
	}

}