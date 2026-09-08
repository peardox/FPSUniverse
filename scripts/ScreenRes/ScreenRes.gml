global.primeList = [
	2,  3,  5,  7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 
   79, 83, 89, 97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,
   191,193,197,199,211,223,227,229,233,239,241,251];
global.primeCount = array_length(global.primeList);
global.maxPrimeFactorList = 6;

// pfp - primeFactorPower
function basePower(_base, _power) constructor {
	self.base = _base;
	self.power = _power;
	
	static getPower = function(_array) {
		var _res = 0;
		var _n = array_length(_array);
		for(var _i=0; _i < _n; _i++) {
			if(_array[_i].base > self.base) {
				break
			}
			if(_array[_i].base == self.base) {
				_res = _array[_i].power;
			}
		}
		return _res;
	}
	
}

function primeFactorize(num) {
	var _base = int64(num);
	if(_base != num) {
		return [];
	}
	if(num > 65536) {
		return [];
	}
	var _res = array_create(global.maxPrimeFactorList, undefined);
	var _p = 0;
	var _idx = 0;
	while(_base > 1) {
		if(_p >= global.primeCount) {
			_res[_idx] = new basePower(_base, 1);
			_idx++;
			break;
		}
		var _candidatePrime = global.primeList[_p];
		var _divCount = 0;
		// while(isMultipleOf(_base, _candidatePrime)) {
		while((_base % _candidatePrime) == 0) {
			_divCount++;
			_base /= _candidatePrime;
		}
		if(_divCount > 0) {
			_res[_idx] = new basePower(_candidatePrime, _divCount);
			_idx++;
		}
		_p++;
		
	}
	/* This is only used to discover the correct value for maxPrimeFactorList
	if(_idx > global.maxPrimeFactorList) {
		global.maxPrimeFactorList = _idx;
	}
	*/
	array_resize(_res, _idx);
	/*
	var _txt = "";
	for(var _i=0, _n=array_length(_res); _i < _n; _i++) {
		if(_i > 0) {
			_txt += ", ";
		}
		_txt += string(_res[_i].base) + "^" + string(_res[_i].power)
	}
	show_debug_message("======> " + string(num) + " prime factors (" + string(_idx) + ") = " + _txt);
	*/
	
	return _res;
}

function lowestSharedPrimeFactors(_v1, _v2) {
	var _pf1 = primeFactorize(_v1);
	var _pf2 = primeFactorize(_v2);
	show_debug_message("======> " + string(_pf1));
	show_debug_message("======> " + string(_pf2));
	var _pf1_len = array_length(_pf1);
	var _res = array_create(min(_pf1_len, array_length(_pf2)), undefined);
	var _idx = 0;
	
	for(var _i = 0; _i < _pf1_len; _i++) {
		var _p = _pf1[_i].getPower(_pf2);
		var _min_power = min(_pf1[_i].power, _p);
		if(_min_power > 0) {
			_res[_idx] = new basePower(_pf1[_i].base, _min_power);
			_idx++;
		}
		
	}
	array_resize(_res, _idx);
	show_debug_message("======> SPF = " + string(_res));
	return _res;
}

function lcf(_v) {
	var _len = array_length(_v);
	var _res = 1;
	for(var _i = 0; _i < _len; _i++) {
		_res *= power(_v[_i].base, _v[_i].power);
	}
	show_debug_message("======> LCF = " + string(_res));
	return _res;
}

var _sw = 1920;
var _sh = 1200;

var _zz = lowestSharedPrimeFactors(_sw, _sh);
var _lcf = lcf(_zz);
var _v1 = _sw / _lcf;
var _v2 = _sh / _lcf;
var _picker = 8;
var _SSFactor = min(_v1, _v2);
var _rr = 0;
show_debug_message("======> Block = " + string(_v1) + " x " + string(_v2));
show_debug_message("======> SSF = " + string(_SSFactor));
for(var _i=1; _i<= _lcf; _i++) {
	var _w = _i * _v1;
	var _h = _i * _v2;
	if(((_w % _picker) == 0) && ((_h % _picker) == 0)) {
		_rr++;
		show_debug_message("======> (" + string(_rr) + "=" + string(_w / _sw) + ")" + string(_w) + " x " + string(_h));
	}
}

/*
primeFactorize(101);
primeFactorize(1920);
primeFactorize(1080);
primeFactorize(3440);
primeFactorize(1440);
primeFactorize(3144);
*/
// game_end();