#include "mapdb.hpp"
#include "map_algorithm.hpp"
#include "store.hpp"



namespace exports {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Number;
    using v8::Array;
    using v8::Exception;

    using namespace std;

    void Method(const FunctionCallbackInfo<Value>& args) {
    }

    void loadMap(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadMap(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void loadMapFromXML(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadMapFromXML(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void loadCluster(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadCluster(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void getN(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> n = Number::New(isolate, store::getN());
        args.GetReturnValue().Set(n);
    }


    void getM(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> m = Number::New(isolate, store::getM());
        args.GetReturnValue().Set(m);
    }


    void getW(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> w = Number::New(isolate, store::getW());
        args.GetReturnValue().Set(w);
    }


    void getNode(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::n) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        auto point = store::getPoint(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "lat"), Number::New(isolate, point.lat));
        obj->Set(String::NewFromUtf8(isolate, "lon"), Number::New(isolate, point.lon));
        obj->Set(String::NewFromUtf8(isolate, "id"), Number::New(isolate, point.id));

        args.GetReturnValue().Set(obj);
    }

    void getNodeById(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || store::mapping.find(args[0]->NumberValue()) == store::mapping.end()) {
            printf("%d\n", args[0]->IsNumber());
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        long long index = (long long)(args[0]->NumberValue());
        auto point = store::getPointById(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "lat"), Number::New(isolate, point.lat));
        obj->Set(String::NewFromUtf8(isolate, "lon"), Number::New(isolate, point.lon));
        obj->Set(String::NewFromUtf8(isolate, "id"), Number::New(isolate, point.id));
        args.GetReturnValue().Set(obj);
    }
    

    void getEdge(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::m) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        auto edge = store::getEdge(index);


        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "a"), Number::New(isolate, edge.a));
        obj->Set(String::NewFromUtf8(isolate, "b"), Number::New(isolate, edge.b));

        args.GetReturnValue().Set(obj);
    }

    void getWay(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::m) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index = int(args[0]->NumberValue());
        auto way = store::getWay(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "op"), Number::New(isolate, way.op));
        obj->Set(String::NewFromUtf8(isolate, "ed"), Number::New(isolate, way.ed));
        obj->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, way.name.c_str()));
        obj->Set(String::NewFromUtf8(isolate, "roadType"), String::NewFromUtf8(isolate, way.road_type.c_str()));


        args.GetReturnValue().Set(obj);
    }


    void getCluster(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::n) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        
        args.GetReturnValue().Set(Number::New(isolate, map_algorithm::getCluster(index)));
    }


    void getClusterById(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || store::mapping.find(args[0]->NumberValue()) == store::mapping.end()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        long long index = (long long)(args[0]->NumberValue());
        
        args.GetReturnValue().Set(Number::New(isolate, store::getClusterById(index)));
    }


    void getMaxLat(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMaxLat()));
    }

    void getMinLat(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMinLat()));
    }


    void getMaxLon(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMaxLon()));
    }

    void getMinLon(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMinLon()));
    }

    void initMapServer(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsObject()) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        // extract filter 
        static set<pair<string, string>> tmp_extract;
        tmp_extract.clear();
        Local<Array> names = args[0]->ToObject()->GetOwnPropertyNames();
        Local<Object> obj = args[0]->ToObject();
        for (size_t i = 0; i < (size_t)names->Length(); ++i) {
            Local<String> key = names->Get(Number::New(isolate, i))->ToString();
            Local<Array> arr = Local<Array>::Cast(obj->Get(key));
            for (size_t i = 0; i < (size_t)arr->Length(); ++i) {
                tmp_extract.insert(
                        make_pair(
                                *(String::Utf8Value(key)),
                                *(String::Utf8Value(arr->Get(Number::New(isolate, i))->ToString()))
                        )
                );
            }
        }

        map_algorithm::init(store::v, store::e, move(tmp_extract));
        args.GetReturnValue().Set(names);

    }

    void query(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        double ret = map_algorithm::query(args[0]->NumberValue(), args[1]->NumberValue())._1;

        if (ret == -1) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        args.GetReturnValue().Set(Number::New(isolate, ret));
    }

    void queryById(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        double ret = map_algorithm::query(store::mapping[args[0]->NumberValue()], store::mapping[args[1]->NumberValue()])._1;

        if (ret == -1) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        args.GetReturnValue().Set(Number::New(isolate, ret));
    }


    void queryPath(const FunctionCallbackInfo<Value>& args) {
        Isolate *isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        auto ans = map_algorithm::query(args[0]->NumberValue(), args[1]->NumberValue(), 1);
        Local<Array> v8path = Array::New(isolate);
        Local<Object> obj = Object::New(isolate);

        for (size_t i = 0; i < ans._2.size(); ++i)
            v8path->Set(Number::New(isolate, i), Number::New(isolate, ans._2[i]));

        obj->Set(String::NewFromUtf8(isolate, "value"), Number::New(isolate, ans._1));
        obj->Set(String::NewFromUtf8(isolate, "path"), v8path);


        args.GetReturnValue().Set(obj);
    }


    void isRoadPointById(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        bool ret = (map_algorithm::d[store::mapping[args[0]->NumberValue()]] > 0);

        args.GetReturnValue().Set(Number::New(isolate, ret));
    }



    void isRoadPoint(const FunctionCallbackInfo<Value>& args) {
        Isolate *isolate = args.GetIsolate();

        bool ret = map_algorithm::d[args[0]->NumberValue()] > 0;

        args.GetReturnValue().Set(Number::New(isolate, ret));
    }



    void getRandomRoadPoint(const FunctionCallbackInfo<Value>& args) {
        Isolate *isolate = args.GetIsolate();

        int x = rand() % store::getN();
        while (map_algorithm::d[x] == 0)
            x = rand() % store::getN();

        args.GetReturnValue().Set(Number::New(isolate, x));
    }



    void getNearestPoint(const FunctionCallbackInfo<Value>& args) {
        Isolate *isolate = args.GetIsolate();

        if (!args[0]->IsObject()) {
            isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        double x = args[0]->ToObject()->Get(String::NewFromUtf8(isolate, "lon"))->NumberValue(),
               y = args[0]->ToObject()->Get(String::NewFromUtf8(isolate, "lat"))->NumberValue();

        size_t ret = map_algorithm::get_nearest_point(store::Point{y, x, 0, ""}, store::v);

        args.GetReturnValue().Set(Number::New(isolate, ret));
    }


    void init(Local<Object> exports) {

        // For random
        srand(time(0));

        // get
        NODE_SET_METHOD(exports, "getN", getN);
        NODE_SET_METHOD(exports, "getM", getM);
        NODE_SET_METHOD(exports, "getW", getW);
        NODE_SET_METHOD(exports, "getNode", getNode);
        NODE_SET_METHOD(exports, "getNodeById", getNodeById);
        NODE_SET_METHOD(exports, "getEdge", getEdge);
        NODE_SET_METHOD(exports, "getWay", getWay);
        NODE_SET_METHOD(exports, "getMaxLon", getMaxLon);
        NODE_SET_METHOD(exports, "getMaxLat", getMaxLat);
        NODE_SET_METHOD(exports, "getMinLon", getMinLon);
        NODE_SET_METHOD(exports, "getMinLat", getMinLat);
        NODE_SET_METHOD(exports, "query", query);
        NODE_SET_METHOD(exports, "queryById", queryById);
        NODE_SET_METHOD(exports, "queryPath", queryPath);
        NODE_SET_METHOD(exports, "isRoadPoint", isRoadPoint);
        NODE_SET_METHOD(exports, "getNearestPoint", getNearestPoint);

        // init
        NODE_SET_METHOD(exports, "loadMapFromXML", loadMapFromXML);
        NODE_SET_METHOD(exports, "initMapServer", initMapServer);


        // for test
        NODE_SET_METHOD(exports, "loadMap", loadMap);
        NODE_SET_METHOD(exports, "getCluster", getCluster);
        NODE_SET_METHOD(exports, "getClusterById", getClusterById);
        NODE_SET_METHOD(exports, "loadCluster", loadCluster);
        NODE_SET_METHOD(exports, "getRandomRoadPoint", getRandomRoadPoint);
    }

    NODE_MODULE(addon, init)

}
