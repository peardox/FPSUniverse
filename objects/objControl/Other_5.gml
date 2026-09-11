fnt.get_info();
show_debug_message("Font Count = " + string(fnt.count)); // 354
show_debug_message("Font Max H = " + string(fnt.max_char_height)); // 36
show_debug_message("Font Max W = " + string(fnt.max_char_width)); // 37
fnt.destroy();
