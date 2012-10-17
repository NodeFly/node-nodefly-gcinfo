var fs = require('fs')

var plat = process.platform
var arch = process.arch

var path = "./compiled/" + plat + "/" + arch

if( !fs.existsSync(path) ){
	throw new Error("Platform Not Supported");
}

var modpath = path + "/gcinfo"

module.exports = require(modpath)
