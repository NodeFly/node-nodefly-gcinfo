// Author: Eugene Kaydalov
// Date: 3/14/2013 3:59:53 PM
//
// Today we precompile nodefly-gcinfo module for 4 operating systems for 
// both 32 and 64 bit architectures.
//
// Frankly I don't know why we ever started doing this. One day we'll pull the trigger 
// and let the users compile it themselves during 'npm install nodefly' or not have
// this feature if they can't compile it (e.g. don't have all the necessary dev tools installed).
//
// lib_ver will contain something like 'v0.8.12' which indicates the exact minor version (.12) (or is it micro?)
// that the library was compiled under.
//
// If we can't find the mapping  we don't try a default compiled version like we used to

var version_map = {
	// using . instead of \\. for clarity
	// what are the chances we'll have version 0.60?
	'v0.6.*': 'v0.6.9',
	'v0.8.*': 'v0.8.12',
	'v0.9.*': 'v0.9.3',
	'v0.10.*': 'v0.10.0'
	}
	, platform = process.platform
	, key
	, lib_ver
	;

if (platform == 'solaris') platform = 'sunos';

for (key in version_map) {
	if (RegExp(key).test(process.version)) {
		lib_ver = version_map[key];
	}
}

// if lib_ver stays undefined require will fail and we'll catch it
// same behaviour when there's a new version of node and we haven't 
// compiled for it yet
var modpath = "./compiled/" + platform + "/" + process.arch + "/" + lib_ver + "/gcinfo";

try {
	module.exports = require(modpath);
}
catch (err) {
	module.exports = { onGC: function(){} }
}