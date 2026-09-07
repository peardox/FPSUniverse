#macro DELTA_SECONDS (delta_time * 0.000001)


function display_window() constructor {
	self.width = window_get_width();
	self.height = window_get_height();

	static update = function() {
		if( (self.width = window_get_width()) &&
			(self.height = window_get_height()) ) {
			return false;
			}
			
		self.width = window_get_width();
		self.height = window_get_height();
		
		room_width = self.width;
		room_height = self.height;
		surface_resize(application_surface, room_width, room_height);
		display_set_gui_size(room_width, room_height);
		
		return true;
	}

}