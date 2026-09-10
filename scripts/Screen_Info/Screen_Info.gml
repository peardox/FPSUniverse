// Feather disable GM1041

// GMEX 4CC end of buffer data marker
#macro MAGIC $474D4558

// Don't rely on checking if extension is loaded (yet?)
// Define supported platforms
global.__screen_info_supported = [os_windows];
// Define unsupported platforms - these should move into supported as and when added
global.__screen_info_unsupported = [os_gxgames, os_linux, os_macosx, os_ios, os_tvos, os_android, os_ps4, os_ps5, os_gdk, os_xboxseriesxs, os_switch];

// For use by 
enum SCREEN_INFO_DATA_BUFFER {
    SCREENINFOHEADER,
	SCREENINFOHEADERRESERVED,
    SCREENINFO,
    PHYSICALSCREEN,
    WINDOWCHROME,
    MONITORNAMEBUFFER,
	MONITORDEVICEBUFFER,
    RESOLUTIONHEADER,
    RESOLUTIONHEADERRESERVED,
    RESOLUTIONINFO,
    DISPLAYMODE

};

enum DPI_AWARENESS {
  DPI_AWARENESS_INVALID = -1,
  DPI_AWARENESS_UNAWARE = 0,
  DPI_AWARENESS_SYSTEM_AWARE = 1,
  DPI_AWARENESS_PER_MONITOR_AWARE = 2
};


// Resolution

function bsearch(_array, _value, _compare) {
    var _l = 0;
    var _r = array_length(_array) - 1;
    while(_l <= _r) {
        var _m = _l + (floor(_r - _l) div 2);
        if(_compare(_array[_m], _value) < 0) {
            _l = _m + 1;
        } else if(_compare(_array[_m], _value) > 0) {
            _r = _m - 1;
        } else {
            return _m;
        }
    }
    return -1;
}

function display_variant(_obj = undefined) constructor {
    self.freq = 0;
    self.bpp = 0;
    
    if(!is_undefined(_obj)) {
        self.freq = _obj.freq;
        self.bpp = _obj.bpp;
    }
}

function display_res(_obj = undefined) constructor {
    self.width = 0;
    self.height = 0;
    self.variants = array_create(0);
    
    if(!is_undefined(_obj)) {
        self.width = _obj.width;
        self.height = _obj.height;
    }
    
    static _variant_compare = function(_current, _next) { // current, next
        if(_current.freq < _next.freq) 
            return -1;
        if(_current.freq > _next.freq) 
            return 1;
        // Freq is equal, sort bpp
        if(_current.bpp < _next.bpp) 
            return -1;
        if(_current.bpp > _next.bpp) 
            return 1;
        // Freq + Bpp are equal
        return 0;
    }
    
    static add_variant = function(_obj) {
        var _next = array_length(self.variants);
        array_resize(self.variants, _next + 1);
        self.variants[_next] = new display_variant(_obj);
        if((_next > 0) && (_variant_compare(self.variants[_next-1], self.variants[_next]) > 0)) {
            array_sort(self.variants, _variant_compare);
        }
    }
}

function display_modes() constructor {
    self.res = array_create(0);
    self.tail = 0;
    self.sortCount = 0;

    static _resolution_compare = function(_current, _next) { // current, next
        if(_current.width < _next.width)
            return -1;
        if(_current.width > _next.width)  
            return 1;
        // Widths are equal so sort heights
        if(_current.height < _next.height)
            return -1;
        if(_current.height > _next.height)  
            return 1;
        // Width + Height are equal
        return 0;
    }
    
    static add = function(_obj) {
        var _this = bsearch(self.res, _obj, _resolution_compare);
        
        if(_this == -1) {
            var _next = array_length(self.res);
            self.tail = _next;
            array_resize(self.res, _next + 1);
            self.res[_next] = new display_res(_obj);
            self.res[_next].add_variant(_obj);
            // Does the array need sorting?
            if((_next > 0) && (_resolution_compare(self.res[_next-1], self.res[_next]) > 0)) {
                array_sort(self.res, _resolution_compare);
                self.sortCount++;
            }
            
        } else {
            self.res[_this].add_variant(_obj);
        }
    }
}

// Screen

function screen_info_timer() constructor {
    self.__timer = get_timer();
	
	static elapsed = function() {
		return get_timer() - self.__timer;
	}
	
}

function screen_info_box(_width, _height) constructor {
	self.width = _width;
	self.height = _height;
	
	static aspect = function() {
		if((self.height) == 0) {
			throw("Zero height window");
		}
		return (self.width) / real(self.height);
	}
}

function screen_info_screenrect(_left, _top, _right, _bottom) constructor {
	self.left := _left;
	self.top := _top;
	self.right := _right;
	self.bottom := _bottom;
	
	static width = function() {
		return real(self.right - self.left);
	}
	
	static height = function() {
		return real(self.bottom - self.top);
	}
	
	static aspect = function() {
		if((self.bottom - self.top) == 0) {
			throw("Zero height window");
		}
		return (self.right - self.left) / real(self.bottom - self.top);
	}
}

function screen_info_physical(_width, _height, _diagonal) : screen_info_box(_width, _height) constructor {
	self.diagonal = _diagonal;

	// Diagonal screen size in inches
	static size = function() {
		return self.diagonal / 25.4;
	}
}

function screen_info_screen() constructor {
	self.error    = 0;
	self.refresh  = 0;
	self.primary  = false;
	self.scaleFactor = -1;
	self.virtual  = undefined;
	self.working  = undefined;
	self.box      = undefined;
	self.physical = undefined;
	self.desc     = "";
    self.device   = "";
    self.modes = undefined;

	static override_physical = function(_diagonal_inches) {
		// Similar triangles so just scale
		var _factor = (_diagonal_inches * 25.4) / self.physical.diagonal;
		self.physical.diagonal = self.physical.diagonal * _factor;
		self.physical.width = self.physical.width * _factor;
		self.physical.height = self.physical.height * _factor;
	}
	
    static set_modes = function(_obj) {
        self.modes = _obj;
    }
    
	static set_description = function(_name) {
		self.desc    = _name;
	}
	
	static set_device = function(_name) {
		self.device  = _name;
	}
	
	static set_error = function(_error) {
		self.error    = _error;
	}
	
	static set_refresh = function(_refresh) {
		self.refresh  = real(_refresh);
	}
	
	static set_primary = function(_primary) {
		self.primary  = _primary;
	}

	static set_scaleFactor = function(_scale) {
		self.scaleFactor  = _scale;
	}

	static add_virtual = function(_left, _top, _right, _bottom) {
		self.virtual = new screen_info_screenrect(_left, _top, _right, _bottom);
	}
	
	static add_working = function(_left, _top, _right, _bottom) {
		self.working = new screen_info_screenrect(_left, _top, _right, _bottom);
	}
	
	static add_box = function(_width, _height) {
		self.box = new screen_info_box(_width, _height);
	}
	
	static add_physical = function(_width, _height, _diagonal) {
		self.physical = new screen_info_physical(_width, _height, _diagonal);
	}
	
	// DPI X
	static ppix = function() {
		return (self.virtual.width() / real(self.physical.width)) * 25.4;
	}

	// DPI Y	
	static ppiy = function() {
		return (self.virtual.height() / real(self.physical.height)) * 25.4;
	}

	// DPI
	static ppi = function() {
		return (self.ppix() + self.ppiy()) / 2.0;
	}

	static is_primary = function() {
		if(primary) {
			return true;
		} else {
			return false;
		}
	}
	
	static get_ui_scale = function() {
    	if(self.virtual.width() == 0) {
    		return NaN;
    	} else {
    		return round((self.box.width / real(self.virtual.width())) * 100) / 100;
    	}
    }
    
}

function window_chrome() constructor {
	self.virtual  = undefined;
	self.working  = undefined;
	self.preambleTime = undefined;
	self.dllTime = undefined;
	self.callTime = undefined;

	static add_virtual = function(_left, _top, _right, _bottom) {
		self.virtual = new screen_info_screenrect(_left, _top, _right, _bottom);
	}
	
	static add_working = function(_left, _top, _right, _bottom) {
		self.working = new screen_info_screenrect(_left, _top, _right, _bottom);
	}
	
	// The actual chrome height is the difference between virtual and working
	// minus the border (from width) which results in caption height
	static height = function() {
		return (self.virtual.height() - self.working.height() - self.width());
	}
	
	// The actual chrome width is 1/2 the difference between virtual and working
	// as this results in the border width being returned which is a meaningful figure
	static width = function() {
		return (self.virtual.width() - self.working.width()) / 2.0;
	}
	
	static overlap = function(_target) {
	    // Compute horizontal overlap
	    var overlap_w = max(0,
	        min(self.virtual.right, _target.virtual.right)
	      - max(self.virtual.left, _target.virtual.left)
	    );
    
	    // Compute vertical overlap
	    var overlap_h = max(0,
	        min(self.virtual.bottom, _target.virtual.bottom)
	      - max(self.virtual.top, _target.virtual.top)
	    );
    
	    // Area is width * height
	    return overlap_w * overlap_h;
	}
	
	static on_screen = function(_screens) {
		var _scount = array_length(_screens);
		var _overlaps = array_create(_scount);
		for(var _i = 0; _i < _scount; _i++) {
			_overlaps[_i] = self.overlap(_screens[_i]);
		}
		var _scr = -1;
		var _largest = 0;
		for(var _i = 0; _i < _scount; _i++) {
			if(_overlaps[_i] > _largest) {
				_scr = _i;
				_largest = _overlaps[_i];
			}
		}
	return _scr;
	}
	
}

function screen_info() constructor {
	self.fallback = true;
	self.screen = [];
	self.count = 0;
	self.active = 0;
	self.preambleTime = undefined;
	self.dllTime = undefined;
	self.callTime = undefined;
	self.dpi_aware = 0;
	
		
	static add_screen = function() {
		var _screen = new screen_info_screen();
		var _screen_index = self.count;
		array_resize(self.screen, _screen_index + 1);
		self.screen[_screen_index] = _screen;
		self.count++;
		return _screen;
	}
	
	static get_primary = function() {
		var _res = 0;
		
		for(var _i = 0; _i < self.count; _i++) {
			if(screen[_i].primary) {
				_res = _i;
				break;
			}
		}
		
		return _res;
	}
	
	static activate = function(_which, _alter_fps = true) {
		if((_which >= 0) && (_which < self.count)) {
			self.active = _which;
			if(_alter_fps) {
				game_set_speed(self.screen[self.active].refresh, gamespeed_fps);
                show_debug_message("Set FPS to " + string(self.screen[self.active].refresh));
			}
			
		}
	}
	static start = function() {
		// See if we have primary in screen list
		var _p = self.get_primary();
		if(_p != -1) {
		// If so activate it
			self.SwitchTo(_p);
		} else {
		// Oops - no primary, just activate first screen
			if(array_length(self.screen) > 0) {
				self.SwitchTo(0);
			} else {
				throw("No screens");
			}
		}
	}


	static AutoCenter = function(_doCenter = true) {
		var _scr = get_chrome();
		if(view_enabled) {
			// Only 1 view ATM
			var _view_scale = self.screen[self.active].get_ui_scale();
			if(_view_scale == -1) {
				window_set_rectangle(
					self.screen[self.active].virtual.left + ((self.screen[self.active].virtual.width() - _scr.virtual.width()) / 2) ,
					self.screen[self.active].virtual.top + ((self.screen[self.active].virtual.height() - _scr.virtual.height()) / 2),
					room_width,
					room_height
					);
			} else {
				var _new_width = round(view_get_wport(0) / _view_scale);
				var _new_height = round(view_get_hport(0) / _view_scale);
				if(_doCenter) {
					var _new_left = round(self.screen[self.active].virtual.left + (((self.screen[self.active].virtual.width() - _new_width) / 2)));
					var _new_top = round(self.screen[self.active].virtual.top + (((self.screen[self.active].virtual.height() - _new_height) / 2)));
				} else {
					var _new_left = self.screen[self.active].virtual.left;
					var _new_top = self.screen[self.active].virtual.top;
				}

				window_set_rectangle(
					_new_left ,
					_new_top,
					_new_width, _new_height
					);
				camera_set_view_size(view_camera[0], _new_width, _new_height);
				surface_resize(application_surface, _new_width, _new_height); // 1280, 800);
				// surface_resize(application_surface, _new_width, _new_height);
				show_debug_message("Reset window View (" + string(_new_left) + ", " + string(_new_top) + ") [" + string(_new_width) + " x " + string(_new_height) + "]");
			}
		} else {
			if(_doCenter) {
				window_set_rectangle(
					self.screen[self.active].virtual.left + ((self.screen[self.active].virtual.width() - _scr.virtual.width()) / 2) ,
					self.screen[self.active].virtual.top + ((self.screen[self.active].virtual.height() - _scr.virtual.height()) / 2),
					room_width,
					room_height
					);
			} else {
				window_set_rectangle(
					self.screen[self.active].virtual.left,
					self.screen[self.active].virtual.top,
					self.screen[self.active].virtual.width(),
					self.screen[self.active].virtual.height()
					);
			}
			show_debug_message("Reset window NoView : (" + string(window_get_x()) + ", " + string(window_get_y()) + ")");
		}
	}
	
	static SwitchTo = function(_which = self.active, _alter_fps = true) {
		if((_which >= 0) && (_which < array_length(self.screen))) {
			self.activate(_which, _alter_fps);
			self.AutoCenter(false);			
		}
	}


	static override_screen = function(_ovr) {
		for(var _i = 0; _i < array_length(self.screen); _i++) {
			if(self.screen[_i].desc == _ovr.desc) {
				self.screen[_i].override_physical(_ovr.size);
				return true;
			}
		}
		return false;
	}
	
	static apply_overrides = function(_ovr) {
		for(var _i = 0; _i < array_length(_ovr); _i++) {
			if(self.override_screen(_ovr[_i])) {
				break;
			}
		}
	}
}

function __screen_info_create_fallback_data() {
	var _inf = false;
    _inf = new screen_info();
    _inf.dllTime = 0;
    _inf.preambleTime = 0;
    _inf.autoHide = 0;
    var _scr = _inf.add_screen();
    _scr.set_error(0);
    _scr.set_refresh(game_get_speed(gamespeed_fps));
    _scr.set_primary(1);
    _scr.set_scaleFactor(100);
    _scr.add_box(display_get_width(), display_get_height());
    _scr.add_virtual(0, 0, display_get_width(), display_get_height());
    _scr.add_working(0, 0, window_get_width(), window_get_height());
    _scr.add_physical(597, 336, 685);
    _scr.set_description("Dummy 27\" monitor");
    _inf.dpi_aware = 0;
    _inf.fallback = true;
    _inf.callTime = 0;
    _inf.count = 1;
    return _inf;
}

function __screen_info_read_resolution_data(_dev) {
    var _modes = undefined;
	var _res_info_buf_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.RESOLUTIONINFO);
	var _res_info_header_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.RESOLUTIONHEADER);
	var _res_info_header_skip = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.RESOLUTIONHEADERRESERVED);
	var _res_info_display_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.DISPLAYMODE);
	if(_res_info_buf_size > 0) {
		var _buf = buffer_create(_res_info_buf_size, buffer_fixed, 1);
		try {
			var _page = 0;
			var _haveMore = false;
			var _res_width, _res_height, _res_refresh, _res_bpp;
			
			do {
				if(__screen_info_get_monitor_resolutions(string(buffer_get_address(_buf)), _dev, _page) == 0) {
					if(buffer_peek(_buf, _res_info_buf_size - 4, buffer_u32) != MAGIC) {
						throw("Magic didn't match");
					}  else { 
                        _modes = new display_modes();
						var _count          = buffer_read(_buf, buffer_s32);
						var _macCount       = buffer_read(_buf, buffer_s32);
						var _fromResolution = buffer_read(_buf, buffer_s32);
						var _pageNum        = buffer_read(_buf, buffer_s32);
						_haveMore           = buffer_read(_buf, buffer_s32);
						buffer_seek(_buf, buffer_seek_relative, _res_info_header_skip);
					    for(var _i = 0; _i < _count; _i++) {
                            _res_width = buffer_read(_buf, buffer_s32);
                            _res_height = buffer_read(_buf, buffer_s32);
                            _res_refresh = buffer_read(_buf, buffer_s32);
                            _res_bpp = buffer_read(_buf, buffer_s32);
                            _modes.add({width: _res_width, height: _res_height, freq: _res_refresh, bpp: _res_bpp});
                        }
                    }
					_page++;
				} else {
					throw("DLL call didn't work");
                }
			} until(!_haveMore); 
        } finally {
			buffer_delete(_buf);
        }
    }
    
    return _modes;
}
                    
function __screen_info_read_screen_data() {
	var _t = new screen_info_timer();
	var _inf = false;
	var _screen_info_buf_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.SCREENINFO);
	var _screen_info_header_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.SCREENINFOHEADER);
	var _screen_info_header_skip = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.SCREENINFOHEADERRESERVED);
	var _screen_info_physical_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.PHYSICALSCREEN);
	var _screen_info_physical_name = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.MONITORNAMEBUFFER);
	if(_screen_info_buf_size > 0) {
		var _buf = buffer_create(_screen_info_buf_size, buffer_fixed, 1);
		try {
			var _page = 0;
			var _dll = 0;
			var _preamble  = _t.elapsed();
			var _haveMore = false;
			var _a1, _a2, _a3, _a4;
			
			do {
				if(__screen_info_get_screen_info_paged(string(buffer_get_address(_buf)), _page) == 0) {
					_dll = _dll + (_t.elapsed() - _preamble);
					if(buffer_peek(_buf, _screen_info_buf_size - 4, buffer_u32) != MAGIC) {
						throw("Magic didn't match");
					}  else {
						// Seek start of buffer as we're reusing it
						buffer_seek(_buf, buffer_seek_start, 0);
						// Process buffer
						var _count        = buffer_read(_buf, buffer_s32);
						var _macCount     = buffer_read(_buf, buffer_s32);
						var _fromScreen   = buffer_read(_buf, buffer_s32);
						var _pageNum      = buffer_read(_buf, buffer_s32);
						var _autoHide     = buffer_read(_buf, buffer_s32);
						_haveMore         = buffer_read(_buf, buffer_s32);
						var _versionMajor = buffer_read(_buf, buffer_s32);
						var _versionMinor = buffer_read(_buf, buffer_s32);
						var _versionBuild = buffer_read(_buf, buffer_s32);
						buffer_seek(_buf, buffer_seek_relative, _screen_info_header_skip);
						var _bpos = buffer_tell(_buf);
						if(_bpos != _screen_info_header_size) {
							_haveMore = 0;
							throw("Wrong buffer position");
						} else {
							// Only construct ScreenInfo if we have some screens and construct as late as possible
							if((_inf == false) && (_count > 0)) {
								_inf = new screen_info();
							}
							_inf.dllTime = _dll;
							_inf.preambleTime = _preamble;
							_inf.autoHide = _autoHide;
							for(var _i = 0; _i < _count; _i++) {
								buffer_seek(_buf, buffer_seek_start, _screen_info_header_size + (_screen_info_physical_size * _i));
								var _scr = _inf.add_screen();
								_scr.set_error(buffer_read(_buf, buffer_s32));
								_scr.set_refresh(buffer_read(_buf, buffer_s32));
								_scr.set_primary(buffer_read(_buf, buffer_s32));
								_scr.set_scaleFactor(buffer_read(_buf, buffer_s32));
								_a1 = buffer_read(_buf, buffer_s32);
								_a2 = buffer_read(_buf, buffer_s32);
								_scr.add_box(_a1, _a2);
								_a1 = buffer_read(_buf, buffer_s32);
								_a2 = buffer_read(_buf, buffer_s32);
								_a3 = buffer_read(_buf, buffer_s32);
								_a4 = buffer_read(_buf, buffer_s32);
								_scr.add_virtual(_a1, _a2, _a3, _a4);
								_a1 = buffer_read(_buf, buffer_s32);
								_a2 = buffer_read(_buf, buffer_s32);
								_a3 = buffer_read(_buf, buffer_s32);
								_a4 = buffer_read(_buf, buffer_s32);
								_scr.add_working(_a1, _a2, _a3, _a4);
								_a1 = buffer_read(_buf, buffer_s32);
								_a2 = buffer_read(_buf, buffer_s32);
								_a3 = buffer_read(_buf, buffer_s32);
								_scr.add_physical(_a1, _a2, _a3);
                                // The following are fixed length but GML has no way to read this easily so save the buffer pos
                                _bpos = buffer_tell(_buf);
								_scr.set_description(buffer_read(_buf, buffer_string));
								buffer_seek(_buf, buffer_seek_start, _bpos + _screen_info_physical_name);
								_scr.set_device(buffer_read(_buf, buffer_string));
                                _scr.set_modes(__screen_info_read_resolution_data(_scr.device));
							}
						}
						_page++;
					}
				} else {
					throw("DLL call didn't work");
				}
			} until(!_haveMore);
		} finally {
			buffer_delete(_buf);
			if(_inf) {
				_inf.dpi_aware = __screen_info_get_dpi_awareness();
				_inf.fallback = false;
//				_inf.start();
	    		_inf.callTime = _t.elapsed();
			}
		}
	}
	return _inf;
}

function _screen_info_load() {
    var _rez = false;
    if(array_contains(global.__screen_info_supported, os_type)) {
        _rez = __screen_info_read_screen_data();
    } else {
        _rez = __screen_info_create_fallback_data();
    }
    if(!_rez ) {
        throw("Can't create a screen object - check ___screen_info_supported + ___screen_info_unsupported");
    }
    return _rez;
}

function get_chrome() {
	var _t = new screen_info_timer();
	var _ret = false;
	var _screen_chrome_buf_size = __screen_info_get_buffer_size(SCREEN_INFO_DATA_BUFFER.WINDOWCHROME);
	if(_screen_chrome_buf_size > 0) {
		var _buf = buffer_create(_screen_chrome_buf_size, buffer_fixed, 1);
		try {
			var _preamble = _t.elapsed();
			var _rv = __screen_info_get_window_chrome(string(buffer_get_address(_buf)), string(window_handle()));
			var _dll = _t.elapsed() - _preamble;
			if(_rv == 0) {
				if(buffer_peek(_buf, _screen_chrome_buf_size - 4, buffer_u32) != MAGIC) {
					throw("Magic didn't match");
				}  else {
					var _a1, _a2, _a3, _a4;
					_ret = new window_chrome();
					_ret.preambleTime = _preamble;
					_ret.dllTime = _dll;
					_a1 = buffer_read(_buf, buffer_s32);
					_a2 = buffer_read(_buf, buffer_s32);
					_a3 = buffer_read(_buf, buffer_s32);
					_a4 = buffer_read(_buf, buffer_s32);
					_ret.add_virtual(_a1, _a2, _a3, _a4);
					_a1 = buffer_read(_buf, buffer_s32);
					_a2 = buffer_read(_buf, buffer_s32);
					_a3 = buffer_read(_buf, buffer_s32);
					_a4 = buffer_read(_buf, buffer_s32);
					_ret.add_working(_a1, _a2, _a3, _a4);
					_ret.callTime = _t.elapsed();
				}
			} else {
				throw("DLL call didn't work");
			}
		} finally {
			buffer_delete(_buf);
        }
	}
  return _ret;				
}

