#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

// Global Singletons Containing Our Async Callbacks //
Persistent<Function> afore_callback;
Persistent<Object>   afore_context;

struct Baton {
	
	// Required //
	uv_work_t request;
	
	// Use Defined [Optional] //
	
	size_t * stats; // The HeapStatistics Snapshot
	GCType gcType;
	GCCallbackFlags gcFlags; 
	
};

static size_t* getheap() {
	
	HeapStatistics heap;
	
	V8::GetHeapStatistics(&heap);
	
	size_t * stats = new size_t[4];
	
	stats[0] = heap.used_heap_size();
	stats[1] = heap.total_heap_size();
	stats[2] = heap.heap_size_limit();
	stats[3] = heap.total_heap_size_executable();
	
	return stats;
	
}

static void nothing(uv_work_t* request)
{
	// Does nothing
	// Necessary for function
}

// Executed on a the main even loop / thread
static void after_gc_async(uv_work_t* request, int throwaway)
{
	
	Baton *baton = static_cast<Baton*>(request->data);
	
	Handle<Value> argv[5];
	argv[0] = Integer::New(baton->stats[0]);
	
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
	
	afore_callback->Call(afore_context,5, argv);
	
	delete [] baton->stats;
	delete baton;
	
}

// Callback executed on whatever thread the GC runs
// on we get the heap statistics and queue a reply
// to execute on the main thread
static void after_gc(GCType type, GCCallbackFlags flags)
{

	Baton *baton = new Baton();
	baton->request.data = baton;
	
	baton->stats   = getheap();
	
	baton->gcType  = type;
	baton->gcFlags = flags;
	
	uv_queue_work(uv_default_loop(), &baton->request, nothing, after_gc_async);

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
	
	// target is like module.exports
	// we are assigning the function 'onGC'
	// to the module
	target->Set(String::NewSymbol("onGC"),
		FunctionTemplate::New(RegisterCallback)->GetFunction());
	
}

NODE_MODULE(gcinfo, init)
