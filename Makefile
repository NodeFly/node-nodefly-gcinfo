.PHONY: clean bin all build test

PLAT=`node -e "process.stdout.write(process.platform)"`
VERS=`node -e "process.stdout.write(process.version)"`
ARCH=`node -e "process.stdout.write(process.arch)"`

RELEASE=compiled/${PLAT}/${ARCH}/${VERS}

all:
	@echo "Usage: [COMMAND]"
	@echo 
	@echo "Commands:"
	@echo
	@echo "\tbuild - build system binary"
	@echo "\tclean - clear existing build directory"

clean:
	rm -rf ./build

build: bin
	
bin:
	mkdir -p ${RELEASE}
	node-gyp configure --dest-cpu=${ARCH}
	node-gyp build
	cp build/Release/gcinfo.node ${RELEASE}

test: build/Release/gcinfo.node
	node -e "require('./index.js').onGC(function(){console.log('GC Event')}); var x = []; setInterval(function(){x.push(new Date())},1)"

build/Release/gcinfo.node: bin
