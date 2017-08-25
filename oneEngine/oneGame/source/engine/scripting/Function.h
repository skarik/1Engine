#pragma once
#include <string>
#include "../../duktape-1.4.0/duktape.h"
namespace rengin {
    namespace Script {

        inline void getTOS(duk_context *ctx, std::string& result)
        {
            result = duk_to_string(ctx, -1);
        }
        inline void getTOS(duk_context *ctx, int& result)
        {
            result = duk_to_int(ctx, -1);
        }
        inline void getTOS(duk_context *ctx, const char*& result)
        {
            result = duk_to_string(ctx, -1);
        }
        inline void getTOS(duk_context *ctx, double& result)
        {
            result = duk_to_number(ctx, -1);
        }
        inline void getTOS(duk_context *ctx, bool& result)
        {
            result = duk_to_boolean(ctx, -1);
        }

        // create the argument conversions
        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, double *arg, bool required = false)
        {
            if (required)
                *arg = duk_require_number(ctx, position);
            else
                *arg = duk_get_number(ctx, position);
            return false;
        }

        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, bool *arg, bool required = false)
        {
            if (required)
                *arg = duk_require_boolean(ctx, position) != 0;
            else
                *arg = duk_get_boolean(ctx, position) != 0;
            return false;
        }

        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, int *arg, bool required = false)
        {
            if (required)
                *arg = duk_require_int(ctx, position);
            else
                *arg = duk_get_int(ctx, position);
            return false;
        }

        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, char **arg, bool required = false)
        {
            if (required)
                *arg = (char *) duk_require_string(ctx, position);
            else
                *arg = (char *) duk_get_string(ctx, position);
            return false;
        }

        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, std::string *arg, bool required = false)
        {
            if (required)
                *arg = (char *) duk_require_string(ctx, position);
            else
                *arg = (char *) duk_get_string(ctx, position);
            return false;
        }

        template <typename T>
        inline bool bridgeFunctionGetArgument(duk_context *ctx, const int position, T* arg, bool required = false)
        {
            int mask = duk_get_type_mask(ctx, position);
            if (mask | DUK_TYPE_MASK_OBJECT)
            {   //TODO: make safe
                void* ptr = duk_get_heapptr(ctx, position);

                duk_get_prop_string(ctx, 0, "\xff""\xff""data");

                *arg = static_cast<T>(duk_to_pointer(ctx, -1));
                duk_push_heapptr(ctx, ptr);
            }
            return true;
        }
// create the return type conversions
        inline int bridgeFunctionReturn(duk_context *ctx, double *ret, std::string typeName="")
        {
            duk_push_number(ctx, *ret);
            return 1;
        }

        inline int bridgeFunctionReturn(duk_context *ctx, bool *ret, std::string typeName="")
        {
            duk_push_boolean(ctx, *ret);
            return 1;
        }

        inline int bridgeFunctionReturn(duk_context *ctx, int *ret, std::string typeName="")
        {
            duk_push_int(ctx, *ret);
            return 1;
        }

        inline int bridgeFunctionReturn(duk_context *ctx, char **ret, std::string typeName="")
        {
            duk_push_string(ctx, *ret);
            return 1;
        }

        inline int bridgeFunctionReturn(duk_context *ctx, std::string *ret, std::string typeName="")
        {
            duk_push_string(ctx, (*ret).c_str());
            return 1;
        }

        template<class T>
        inline int bridgeFunctionReturn(duk_context *ctx, T *ret, std::string typeName="")
        {
            if (typeName.empty())
                return -1;

            duk_idx_t id = duk_push_object(ctx);
            duk_push_string(ctx, (typeName+".prototype;").c_str());
            duk_eval(ctx);
            duk_set_prototype(ctx, id);
            duk_push_pointer(ctx, *ret);
            duk_put_prop_string(ctx, id, "\xff""\xff""data");
            duk_push_boolean(ctx, false);
            duk_put_prop_string(ctx, id, "\xff""\xff""deleted");
            duk_push_boolean(ctx, true);
            duk_put_prop_string(ctx, id, "\xff""\xff""native_c_instance");

            return 1;
        }

        inline int bridgeFunctionReturn(duk_context *ctx)
        {
            return 0;
        }

        struct BridgeFunction
        {

        };

        //=========================================//
        // holding functions
        //=========================================//
        template<typename Function>
        struct BridgeFunctionSpec: public BridgeFunction
        {
            Function function;
            bool isMethod;
            BridgeFunctionSpec(Function func, bool isMethod=false) : function(func), isMethod(isMethod)
            { ; }

        };


    }
}