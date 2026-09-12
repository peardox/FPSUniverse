// Fragment Shader — sh_corners_all
varying vec2 v_vTexcoord;
varying vec4 v_vColour;

// ---- Uniforms ----
uniform vec2  u_resolution;   // draw area size in pixels
uniform vec3  u_cornerColor;  // stroke colour (0..1 per channel)
uniform float u_thickness;    // base stroke thickness in pixels
uniform float u_taper;        // 0.0 = uniform width, 1.0 = strong taper
uniform vec2  u_size;         // box half-extents in pixels
uniform float u_radius;       // corner radius in pixels

// ---- Signed distance to a rounded rectangle ----
float roundedBox(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    // Remap 0..1 texcoords to pixel-space, centred on the box.
    vec2 uv = v_vTexcoord * u_resolution - u_resolution * 0.5;

    float dist = roundedBox(uv, u_size, u_radius);

    // ---- Taper: vary thickness along each corner arc ----
    vec2  local = abs(uv) - (u_size - u_radius);
    float a     = clamp(atan(max(local.y, 0.0), max(local.x, 0.0)) / 1.5708,
                        0.0, 1.0);
    float tip   = 1.0 - abs(a - 0.5) * 2.0;
    float taper = mix(1.0, mix(0.35, 1.0, tip), u_taper);

    float t = u_thickness * taper;

    // ---- Stroke the outline, centred on the boundary ----
    float edgeDist = abs(dist) - t * 0.5;
    float aa       = max(fwidth(edgeDist), 0.001);
    float ring     = 1.0 - smoothstep(-aa, aa, edgeDist);

    // ---- Keep only corner regions (all four) ----
    vec2  q      = abs(uv) - (u_size - u_radius);
    float qm     = min(q.x, q.y);
    float aaq    = max(fwidth(qm), 0.001);
    float corner = smoothstep(-aaq, aaq, qm);

    // ---- Compose (no quadrant selection → all corners) ----
    float alpha = ring * corner;

    gl_FragColor = vec4(u_cornerColor, alpha) * v_vColour;
}