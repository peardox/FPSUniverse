enum animate { CAMERA , MODEL }
//universe = new BBMOD_Model("Sky/Sky.bbmod");
//universe.freeze();
rotationAngle = 0;
move = animate.CAMERA; // MODEL; //CAMERA;
var _secondsPerRotation = 12;
rotationSpeed = 360 / _secondsPerRotation;
camera = new BBMOD_BaseCamera();
camera.Fov = 30;
camera.Up = new BBMOD_Vec3(0, 0, 1);
camera.Position = new BBMOD_Vec3(0, 0, 0);
camera.Target = new BBMOD_Vec3(-1, 0, -1);
universe = BBMOD_RESOURCE_MANAGER.load("Sky/Sky.bbmod", function (_error, _model) {
//universe = BBMOD_RESOURCE_MANAGER.load("World/World.bbmod", function (_error, _model) {
	if (_error)
	{
		// TODO: Loading failed! Handle error...
		show_message("Error loading model");
	}
	else
	{
		// Sanity check then Flip the texture
		if(move == animate.MODEL) {
			if(array_length(_model.Materials) == 1) {
				_model.Materials[0].TextureScale = new BBMOD_Vec2(-1, 1);
			}
		}
		_model.freeze();
	}
});