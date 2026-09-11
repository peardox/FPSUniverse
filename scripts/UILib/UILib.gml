function DynamicFont(_name, _from = 20, _to = 126) constructor {
	name = _name;
	bold = false;
	italic = false;
	size = 7;
	from = _from;
	to = _to;
	font = undefined;
	count = 0;
	max_char_height = 0;
	max_char_width = 0;
	__timer = 0;
	time = 0;
	
	static activate = function() {
		if(!font_exists(self.font)) {
			exit;
		}
		draw_set_font(self.font);
	}
	
	static create = function() {
		self.timer();
		if(font_exists(self.font)) {
			font_delete(self.font);
		}
		self.font = font_add(self.name, self.size, self.bold, self.italic, self.from, self.to);
		self.time = self.elapsed();
		self.get_info();
	}
	
	static destroy = function() {
		if(!font_exists(self.font)) {
			exit;
		}
		font_delete(self.font);
	}
	
	static exists = function() {
		return font_exists(self.font);
	}
	
	static get_height = function() {
		return self.max_char_height;
	}
	
	static get_info = function() {
		if(!font_exists(self.font)) {
			exit;
		}
		var _fi = font_get_info(self.font);
//		show_debug_message(string(json_stringify(_fi, true)));
		count = 0;
		self.max_char_height = 0;
		self.max_char_width = 0;
		struct_foreach(_fi.glyphs,function(_name, _value) {
			if((_value.char >= self.from) && (_value.char <= self.to)) {
				if(_value.h > self.max_char_height) {
					self.max_char_height = _value.h;
				}
				if(_value.w > self.max_char_width) {
					self.max_char_width = _value.w;
				}
			}
			self.count++;
		});
		
	}
	
	static set_size = function(_size) {
		self.size = _size;
	}

	static find_size = function(_lines, _height) {
		var _test = floor(_height / _lines);
		var _finished = false;
		var _loop = 0;
		do {
			_loop++;
			// show_debug_message("Test #" + string(_loop) + " size = " + string(_test));
			if(_test <= 8) {
				break;
			}
			self.size = _test;
			self.create();
			self.get_info();
			if((self.max_char_height * _lines) > _height) {
				_test--;
			} else {
				var _diff = _height - (self.max_char_height * _lines);
				if(_diff > self.max_char_height) {
					_test++;
				} else {
					_finished = true;
				}
			}

		} until(_finished || (_loop > 100));	
		
		
		
	}
	
	static timer = function()  {
		self.__timer = get_timer();
	}
	
	static elapsed = function() {
		return get_timer() - self.__timer;
	}
}

function display_window() constructor {
	width = -1;
	height = -1;
	isFullscreen = false;
	font = undefined;
	lines = 0;
	
	static update = function() {
		if( (self.width == window_get_width()) &&
			(self.height == window_get_height()) ) {
			return false;
		}
		self.set_size();
	}
			
	static set_size = function() {
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
		
		if( (self.width == display_get_gui_width()) &&
			(self.height == display_get_gui_height()) ) {
			self.isFullscreen = true;
		} else {
			self.isFullscreen = false;
		}

		if(!is_undefined(self.font)) {
			if(self.font.exists()) {
				if(self.lines > 0) {
					self.font.find_size(self.lines, self.height);
				}
			}
		}
		return true;
	}
	
	static set_font = function(_font) {
		if(is_undefined(_font) || !_font.exists()) {
			exit;
		}
		self.font = _font;
	}
	
	static set_lines = function(_lines) {
		self.lines = _lines;
	}

}

