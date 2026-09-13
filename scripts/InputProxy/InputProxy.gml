enum INPUT_VERB
{
    //Add your own verbs here!
    UP,
    DOWN,
    ACCEPT,
    ACTION,
    SPECIAL,
	EXIT
}
    
function GetRequestedVerb(which) {
	var _opt = -1;
	switch(which) {
		case INPUT_VERB.UP:
			_opt = vk_up;
			break;
		case INPUT_VERB.DOWN:
			_opt = vk_down;
			break;
		case INPUT_VERB.ACTION:
			_opt = vk_f11;
			break;
		case INPUT_VERB.SPECIAL:
			_opt = vk_f12;
			break;
		case INPUT_VERB.EXIT:
			_opt = vk_escape;
			break;
		case INPUT_VERB.ACCEPT:
			_opt = vk_space;
			break;
	}
	
	return _opt;
}

function InputCheck(which) {
	var _opt = GetRequestedVerb(which);
	if(_opt != -1) {
		return keyboard_check(_opt);
	}
}

function InputPressed(which) {
	var _opt = GetRequestedVerb(which);
	if(_opt != -1) {
		return keyboard_check_pressed(_opt);
	}
}
