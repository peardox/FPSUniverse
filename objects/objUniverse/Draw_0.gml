if(universe.IsLoaded) {
	var _scale = min(room_width / 2, room_height / 2);
	frame++;
	rotationAngle += rotationSpeed * DELTA_SECONDS;
	bbmod_material_reset();
	new BBMOD_Matrix()
		.RotateX(90)
		.RotateY(rotationAngle)
		.RotateZ(180)
		.Scale(_scale, _scale, 1000)
		.Translate(room_width / 2, room_height / 2, 0)
		.ApplyWorld();
	// Same as:
	//matrix_set(matrix_world, matrix_multiply(
	//	matrix_build(0, 0, 0, 0, 0, -45, 100, 100, 100),
	//	matrix_build(x, y, 0, 45, 0, 0, 1, 1, 1)));
	universe.submit();
	new BBMOD_Matrix().ApplyWorld();
	// Same as:
	//matrix_set(matrix_world, matrix_build_identity());
	bbmod_material_reset();
}
