// --- Draw event ---
var _w = 300;   // full width  of the draw area
var _h = 100;   // full height of the draw area
shader_set(sh_corner);
shader_set_uniform_f(u_resolution,  _w, _h);
shader_set_uniform_f(u_cornerColor, 0.2, 0.5, 0.9);
shader_set_uniform_f(u_thickness,   4.0);
shader_set_uniform_f(u_corner,      1.0);          // top-right
shader_set_uniform_f(u_taper,       0.6);
shader_set_uniform_f(u_size,        _w * 0.5, _h * 0.5); // half-extents
shader_set_uniform_f(u_radius,      20.0);

// Draw a plain white 1x1 stretched to the target rect so v_vTexcoord
// sweeps a clean 0..1 across the whole area.
draw_sprite_stretched(spr_white, 0, x, y, _w, _h);

shader_reset();


