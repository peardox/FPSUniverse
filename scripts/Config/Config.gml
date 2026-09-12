#macro DELTA_SECONDS (delta_time * 0.000001)
#macro MAX_SCREEN_RES 8192

enum animate { CAMERA , MODEL }

global.modelSelected = 0;
global.modelList = [
	"Sky/Sky.bbmod",
	"World/World.bbmod"
	];

global.__build_id = 3;



