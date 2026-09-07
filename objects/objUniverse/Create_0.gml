
//universe = new BBMOD_Model("Sky/Sky.bbmod");
//universe.freeze();
frame = 0;
//universe = BBMOD_RESOURCE_MANAGER.load("Sky/Sky.bbmod", function (_error, _model) {
universe = BBMOD_RESOURCE_MANAGER.load("World/World.bbmod", function (_error, _model) {
	if (_error)
	{
		// TODO: Loading failed! Handle error...
		show_message("Error loading model");
	}
	else
	{
		// Here you can for example freeze the model after it's loaded
		_model.freeze();
	}
});