var b = require("./index")

b.onGC(function(usage,type,flags,type_raw,flags_raw){
	console.log("Heap After GC: ",usage,type,flags,type_raw,flags_raw);
});

var l=[];
var m=[];

setInterval(function(){
	var x = new Object();
	x.time = new Date();
	m.push(x);
},1);

setInterval(function(){
	var x = new Object();
	x.time = new Date();
	l.push(x);
},1);

setInterval(function(){
	l = [];
},2000);
