#include "queue.h"
#include <v8.h>
#include <node.h>

namespace {

using v8::Arguments;
using v8::Context;
using v8::Function;
using v8::FunctionTemplate;
using v8::GCCallbackFlags;
using v8::GCType;
using v8::Handle;
using v8::HandleScope;
using v8::HeapStatistics;
using v8::Integer;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Undefined;
using v8::V8;
using v8::Value;
using v8::kGCCallbackFlagCompacted;
using v8::kGCTypeAll;
using v8::kGCTypeMarkSweepCompact;
using v8::kGCTypeScavenge;
using v8::kNoGCCallbackFlags;

// Global Singletons Containing Our Async Callbacks //
Persistent<Function> afore_callback;
Persistent<Object>   afore_context;
uv_idle_t            idle_handle;
QUEUE                baton_queue;

struct Baton {

	QUEUE baton_queue;
	GCType gcType;
	GCCallbackFlags gcFlags; 
	HeapStatistics stats;

};

static void after_gc_idle(uv_idle_t*, int) {

	HandleScope handle_scope;

	uv_idle_stop(&idle_handle);
	while(!QUEUE_EMPTY(&baton_queue)) {
		QUEUE* q = QUEUE_HEAD(&baton_queue);
		QUEUE_REMOVE(q);

		Baton* baton = QUEUE_DATA(q, Baton, baton_queue);

		const int argc = 5;
		Handle<Value> argv[argc];

		argv[0] = Integer::New(baton->stats.used_heap_size());

		switch(baton->gcType)
		{
			case kGCTypeAll:
				argv[1] = String::New("kGCTypeAll");
				break;
			case kGCTypeMarkSweepCompact:
				argv[1] = String::New("kGCTypeMarkSweepCompact");
				break;
			case kGCTypeScavenge:
				argv[1] = String::New("kGCTypeScavenge");
				break;
			default:
				argv[1] = String::New("UnknownType");
				break;
		}

		switch(baton->gcFlags)
		{
			case kGCCallbackFlagCompacted:
				argv[2] = String::New("kGCCallbackFlagCompacted");
				break;
			case kNoGCCallbackFlags:
				argv[2] = String::New("kNoGCCallbackFlags");
				break;
			default:
				argv[2] = String::New("UnknownFlags");
				break;
		}

		argv[3] = Integer::New(baton->gcType);
		argv[4] = Integer::New(baton->gcFlags);
		delete baton;

		afore_callback->Call(afore_context, argc, argv);
	}

}

// Callback executed on whatever thread the GC runs
// on we get the heap statistics and queue a reply
// to execute on the main thread
static void after_gc(GCType type, GCCallbackFlags flags)
{

	Baton *baton = new Baton();
	baton->gcType  = type;
	baton->gcFlags = flags;
	V8::GetHeapStatistics(&baton->stats);
	uv_idle_start(&idle_handle, after_gc_idle);
	QUEUE_INSERT_TAIL(&baton_queue, &baton->baton_queue);

}

// Handle<Value> qualifies as a V8 Function
static Handle<Value> RegisterCallback(const Arguments& args) {
	
	Handle<Function> cb = Handle<Function>::Cast(args[0]);
	
	// We retain these as Persistent objects for async callbacks
	afore_callback = Persistent<Function>::New(cb);
	afore_context  = Persistent<Object>::New(Context::GetCalling()->Global());
	
	// this callback executes on a separate thread
	V8::AddGCEpilogueCallback(after_gc);
	
	// The function must return, even if it's nothing
	return Undefined();
	
}

// Setup the module
static void init(Handle<Object> target) {

	QUEUE_INIT(&baton_queue);
	uv_idle_init(uv_default_loop(), &idle_handle);
	uv_unref(reinterpret_cast<uv_handle_t*>(&idle_handle));

	// target is like module.exports
	// we are assigning the function 'onGC'
	// to the module
	target->Set(String::NewSymbol("onGC"),
		FunctionTemplate::New(RegisterCallback)->GetFunction());
	
}

}	// namespace anonymous

NODE_MODULE(gcinfo, init)
