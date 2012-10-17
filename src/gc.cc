#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

// Global Singletons //
Persistent<Function> afore_callback;
Persistent<Object>   afore_context;

struct ReshelveBaton {
	// Required //
	uv_work_t request;
	
	// Use Defined [Optional] //
	size_t heap_size;
};

static size_t getheap() {
	
	HeapStatistics stats;

	V8::GetHeapStatistics(&stats);

	return stats.used_heap_size();

}

static void nothing(uv_work_t* request)
{
	// Does nothing
	// Necessary for function
}

// Executed on a the main even loop / thread
static void after_gc_async(uv_work_t* request)
{
  
	ReshelveBaton *baton = static_cast<ReshelveBaton*>(request->data);

	Handle<Value> argv[1];
	argv[0] = Integer::New(baton->heap_size);

	afore_callback->Call(afore_context,1, argv);

}

// Callback executed on whatever thread the GC runs
// on we get the heap statistics and queue a reply
// to execute on the main thread
static void after_gc(GCType type, GCCallbackFlags flags)
{

	ReshelveBaton *baton = new ReshelveBaton();
	baton->request.data = baton;

	baton->heap_size = getheap();

	uv_queue_work(uv_default_loop(), &baton->request, nothing, after_gc_async);

}

static Handle<Value> RegisterCallback(const Arguments& args) {
	    
	Handle<Function> cb = Handle<Function>::Cast(args[0]);
	Handle<BooleanObject> handleAll = Handle<BooleanObject>::Cast(args[1]);
	
	GCType type = kGCTypeMarkSweepCompact;
	
	// We retain these as Persistent objects for async callbacks
	afore_callback = Persistent<Function>::New(cb);
	afore_context  = Persistent<Object>::New(Context::GetCalling()->Global());

	// Receive a callback _only_ after a full GC of
	// type kGCTypeMarkSweepCompact
	// 
	// this callback executes on a separate thread
	V8::AddGCEpilogueCallback(after_gc,type);
	
	
	// The function must return, even if it's nothing
	return Undefined();
}

static void init(Handle<Object> target) {
  
	target->Set(String::NewSymbol("onGC"),
		FunctionTemplate::New(RegisterCallback)->GetFunction());

}

NODE_MODULE(gcinfo, init)
