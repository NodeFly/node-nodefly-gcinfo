var fs = require('fs')

var plat = process.platform
var arch = process.arch
var vers = process.version

var path = "./compiled/" + plat + "/" + arch + "/" + vers

var modpath = path + "/gcinfo"

module.exports = require(modpath)
