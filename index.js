var fs = require('fs')

var plat = process.platform
var arch = process.arch

var path = "./compiled/" + plat + "/" + arch

var modpath = path + "/gcinfo"

module.exports = require(modpath)
