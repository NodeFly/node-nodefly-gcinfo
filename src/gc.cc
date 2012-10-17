#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

Persistent<Function> afore_callback;
Persistent<Object>   afore_context;

struct ReshelveBaton {
	uv_work_t request;
	size_t heap_size;
};

static size_t getheap() {
	
	HeapStatistics stats;

	V8::GetHeapStatistics(&stats);
  
	size_t heap_size_total = stats.total_heap_size();
	size_t heap_size_limit = stats.heap_size_limit();
	size_t heap_size_exec  = stats.total_heap_size_executable();
	size_t heap_size_used  = stats.used_heap_size();

	return heap_size_used;

}

static void nothing(uv_work_t* request)
{
	// Does nothing
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
	
	afore_callback = Persistent<Function>::New(cb);
	afore_context  = Persistent<Object>::New(Context::GetCalling()->Global());

	V8::AddGCEpilogueCallback(after_gc);

	return Undefined();
}

static void init(Handle<Object> target) {
  
	target->Set(String::NewSymbol("on"),
		FunctionTemplate::New(RegisterCallback)->GetFunction());

}

NODE_MODULE(gcinfo, init)
