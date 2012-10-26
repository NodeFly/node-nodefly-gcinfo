var fs = require('fs')

var plat = process.platform
var arch = process.arch
var vers = process.version

var version_defaults = {
	
	// Version 8
	'v0.8.0': 'v0.8.12',
	'v0.8.1': 'v0.8.12',
	'v0.8.2': 'v0.8.12',
	'v0.8.3': 'v0.8.12',
	'v0.8.4': 'v0.8.12',
	'v0.8.5': 'v0.8.12',
	'v0.8.6': 'v0.8.12',
	'v0.8.7': 'v0.8.12',
	'v0.8.8': 'v0.8.12',
	'v0.8.9': 'v0.8.12',
	'v0.8.10': 'v0.8.12',
	'v0.8.11': 'v0.8.12',
	'v0.8.12': 'v0.8.12',
	
	// Version 6
	'v0.6.0': 'v0.6.12',
	'v0.6.1': 'v0.6.12',
	'v0.6.2': 'v0.6.12',
	'v0.6.3': 'v0.6.12',
	'v0.6.4': 'v0.6.12',
	'v0.6.5': 'v0.6.12',
	'v0.6.6': 'v0.6.12',
	'v0.6.7': 'v0.6.12',
	'v0.6.8': 'v0.6.12',
	'v0.6.9': 'v0.6.12',
	
	// Version 9
	'v0.9.0': 'v0.9.3',
	'v0.9.1': 'v0.9.3',
	'v0.9.2': 'v0.9.3',
	'v0.9.3': 'v0.9.3'
	
}

var path = "./compiled/" + plat + "/" + arch + "/" + version_defaults[vers]

var modpath = path + "/gcinfo"

module.exports = require(modpath)
