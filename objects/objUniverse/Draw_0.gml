if(universe.IsLoaded) {
	rotationAngle += rotationSpeed * DELTA_SECONDS;
	rotationAngle = rotationAngle % 360;
	if(move == animate.CAMERA) {
		var _cosa = dcos(rotationAngle);
		var _sina = dsin(rotationAngle);
		camera.Target = new BBMOD_Vec3(_sina, _cosa, 0);
		camera.update(delta_time);
		camera.apply();
	}
	bbmod_material_reset();
	if(move == animate.MODEL) {
		var _scale = min(room_width / 2, room_height / 2);
		new BBMOD_Matrix()
			.RotateX(90)
			.RotateY(rotationAngle)
			.RotateZ(180)
			.Scale(_scale, _scale, _scale)
			.Translate(room_width / 2, room_height / 2, 0)
			.ApplyWorld();
	}
	universe.submit();
	new BBMOD_Matrix().ApplyWorld();
	bbmod_material_reset();

}
