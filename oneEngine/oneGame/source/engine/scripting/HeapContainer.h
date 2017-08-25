//
// Created by rdsnijder on 3/10/16.
//

#ifndef RENGIN_HEAPCONTAINER_H
#define RENGIN_HEAPCONTAINER_H


#include "../../duktape-1.4.0/duktape.h"
#include "../Exception/CreateFailedException.h" 
#include <iostream>

namespace rengin { namespace Script {
    namespace {
        static void fatalhandler (duk_hthread* ctx, duk_errcode_t code, const char *msg)
        {
            std::cout << "handler: " << msg << std::endl;
            duk_pop(ctx);
        }
        static void* duk_malloc(void* duk, duk_size_t size) {
            return malloc(size);
        }
        static void* duk_realloc(void* duk, void* ptr, duk_size_t size) {
            return realloc(ptr, size);
        }
        static void duk_free(void* duk, void* ptr) {
            return free(ptr);
        }
        // typedef void (*duk_fatal_function) (duk_context *ctx, duk_errcode_t code, const char *msg);
    }
    class HeapContainer {
    private:
        duk_context *mContext;
    public:
        HeapContainer()
        {
            mContext = duk_create_heap(duk_malloc, duk_realloc, duk_free, (void *) 0xdeadbeef, fatalhandler);
            if (!mContext)
                throw Exception::CreateFailedException("DukTape heap");
        }
        virtual ~HeapContainer()
        {
            duk_destroy_heap(mContext);
        }
        inline duk_context* ctx() const { return mContext; }

        void printStack()
        {
            duk_push_context_dump(mContext);
            std::cout << duk_to_string(mContext, -1) << std::endl;
            duk_pop(mContext);
        }
    };

    }}
#endif //RENGIN_HEAPCONTAINER_H
