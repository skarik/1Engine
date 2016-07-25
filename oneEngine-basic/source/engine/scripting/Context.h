//
// Created by roelof on 2/9/16.
//

#pragma once //RENGIN_CONTEXT_H

#include <stack>
#include <map>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <iostream>
#include "../../duktape-1.4.0/duktape.h"
#include "../Exception/CreateFailedException.h"
#include "Function.h"
#include "JSStack.h"
#include "HeapContainer.h"

namespace rengin {
    namespace Script {



        class Context : public HeapContainer
        {
        private:
            duk_idx_t mTmpId;
            std::stack<std::string> mParents;
        protected: 
            static Context *mCurrentContext;
            static std::map<int, std::string> mTypes;
        public:
            std::vector<BridgeFunction *> mStorage;

            Context() : HeapContainer()
            {

                if (Context::mCurrentContext)
                    printf("Warning, additional context created!\n"); //Multiple Contexts not supported right now

                Context::mCurrentContext = this;

            }


            //Parse a file
            void parse(std::string fileName);


            virtual ~Context();
 

            template<typename T>
            static std::string getJSTypeName()
            {
                auto result = mTypes[typeid(T).hash_code()];
                if (result.empty())
                    return "undefined";
                return result;
            }

        protected:
#define CALL_MEMBER(object, ptrToMember)  ((object)->*(ptrToMember))

#define METHOD_CALL_PRE typedef BridgeFunctionSpec<FunctionType> Function; \
            duk_push_this(ctx); \
            duk_get_prop_string(ctx, -1, "\xff""\xff""data"); \
            Class *obj = static_cast<Class*>(duk_to_pointer(ctx, -1));

#define METHOD_CALL_POST\
            int result = bridgeFunctionReturn(ctx, &ret ); \
            if (result == -1) \
            { \
        typedef typename std::remove_pointer<Return>::type R; \
                result = bridgeFunctionReturn(ctx, &ret, Context::getJSTypeName<R>()); \
            } \
            return result;

#define CONSTRUCTOR_CALL_POST\
            duk_put_prop_string(ctx, -2, "\xff""\xff""data");\
\
            duk_push_boolean(ctx, false);\
            duk_put_prop_string(ctx, -2, "\xff""\xff""deleted");\
            duk_push_boolean(ctx, false);\
            duk_put_prop_string(ctx, -2, "\xff""\xff""native_c_instance");\
\
            duk_push_c_function(ctx, bridgeDestructorCall<Class>, argCount);\
            duk_set_finalizer(ctx, -2);\
            return 0;

            //Template madness beyond this point
            template<class Class>
            static duk_ret_t bridgeDestructorCall(duk_context *ctx)
            {
                //http://stackoverflow.com/questions/30296953/export-c-class-to-duktape
                // The object to delete is passed as first argument instead
                duk_get_prop_string(ctx, 0, "\xff""\xff""deleted");

                bool deleted = duk_to_boolean(ctx, -1);
                duk_pop(ctx);
                duk_get_prop_string(ctx, 0, "\xff""\xff""native_c_instance"); //Is the object lifetime managed by C or by JS

                bool mayDelete = !duk_to_boolean(ctx, -1);
                duk_pop(ctx);

                if (!deleted&&mayDelete)
                {
                    duk_get_prop_string(ctx, 0, "\xff""\xff""data");
                    delete static_cast<Class *>(duk_to_pointer(ctx, -1));
                    duk_pop(ctx);

                    // Mark as deleted
                    duk_push_boolean(ctx, true);
                    duk_put_prop_string(ctx, 0, "\xff""\xff""deleted");
                }

                return 0;
            }

            template<class Class>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }

                const int argCount = 0;

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class());
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 1;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0));
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0, typename Arg1>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 2;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 1, &arg1, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0, arg1));
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 3;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 1, &arg1, true);
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 2, &arg2, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0, arg1, arg2));
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 3;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 1, &arg1, true);
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 2, &arg2, true);
                Arg3 arg3;
                bridgeFunctionGetArgument(ctx, 3, &arg3, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0, arg1, arg2, arg3));
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 4;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 1, &arg1, true);
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 2, &arg2, true);
                Arg3 arg3;
                bridgeFunctionGetArgument(ctx, 3, &arg3, true);
                Arg4 arg4;
                bridgeFunctionGetArgument(ctx, 4, &arg4, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0, arg1, arg2, arg3, arg4));
                CONSTRUCTOR_CALL_POST

            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            static duk_ret_t bridgeConstructorCall(duk_context *ctx)
            {
                if (!duk_is_constructor_call(ctx))
                {
                    return DUK_RET_TYPE_ERROR;
                }
                const int argCount = 4;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0, true);
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 1, &arg1, true);
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 2, &arg2, true);
                Arg3 arg3;
                bridgeFunctionGetArgument(ctx, 3, &arg3, true);
                Arg4 arg4;
                bridgeFunctionGetArgument(ctx, 4, &arg4, true);
                Arg5 arg5;
                bridgeFunctionGetArgument(ctx, 4, &arg5, true);

                duk_push_this(ctx);

                duk_push_pointer(ctx, new Class(arg0, arg1, arg2, arg3, arg4, arg5));
                CONSTRUCTOR_CALL_POST

            }

            //C Functions
            template<typename FunctionType, typename Return>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Function *fPtr = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)]);
                Return ret = fPtr->function();
                return bridgeFunctionReturn(ctx, &ret);
            }

            template<class Class, typename FunctionType, typename Return>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)();
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return>
            static duk_ret_t bridgeVariableCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                FunctionType fPtr = func->function; //Not actually a function but a member variable!!
                Return ret = CALL_MEMBER(obj, fPtr);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return>
            static duk_ret_t bridgeVariableSetCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                FunctionType fPtr = func->function; //Not actually a function but a member variable!!
                Return data;
                bridgeFunctionGetArgument(ctx, 0, &data);

                CALL_MEMBER(obj, fPtr) = data;
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Return, typename Arg0>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0);

                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return, typename Arg0>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {


                METHOD_CALL_PRE
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0);
                METHOD_CALL_POST
            }

            template<typename FunctionType, typename Return, typename Arg0, typename Arg1>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);

                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0, arg1);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return, typename Arg0, typename Arg1>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);

                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0);
                METHOD_CALL_POST
            }

            template<typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);

                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0, arg1, arg2);
                METHOD_CALL_POST
            }


            template<class Class, typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);

                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2);
                METHOD_CALL_POST
            }


            template<typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);


                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0, arg1, arg2, arg3);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);

                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3);
                METHOD_CALL_POST
            }

            template<typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);


                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0, arg1, arg2, arg3, arg4);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);

                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3, arg4);
                METHOD_CALL_POST
            }

            template<typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            static duk_ret_t bridgeFunctionCall(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                Arg5 arg5;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);
                bridgeFunctionGetArgument(ctx, 5, &arg5);


                Return ret = ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(
                        arg0, arg1, arg2, arg3, arg4, arg5);
                METHOD_CALL_POST
            }

            template<class Class, typename FunctionType, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            static duk_ret_t bridgeMethodCall(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                Arg5 arg5;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);
                bridgeFunctionGetArgument(ctx, 4, &arg5);

                FunctionType fPtr = func->function;
                Return ret = CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3, arg4, arg5);
                METHOD_CALL_POST
            }

            //VOID RETURN TYPES

            template<typename FunctionType>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function();
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)();
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                bridgeFunctionGetArgument(ctx, 0, &arg0);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0);
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0, typename Arg1>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);

                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0, arg1);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0, typename Arg1>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0, arg1);
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0, typename Arg1, typename Arg2>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);

                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0, arg1,
                                                                                                        arg2);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0, typename Arg1, typename Arg2>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2);
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);


                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0, arg1,
                                                                                                        arg2, arg3);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3);
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);


                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0, arg1,
                                                                                                        arg2, arg3,
                                                                                                        arg4);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3, arg4);
                return bridgeFunctionReturn(ctx);
            }

            template<typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            static duk_ret_t bridgeFunctionCallV(duk_context *ctx)
            {
                typedef BridgeFunctionSpec<FunctionType> Function;
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                Arg5 arg5;
                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);
                bridgeFunctionGetArgument(ctx, 5, &arg5);


                ((Function *) Context::mCurrentContext->mStorage[duk_get_current_magic(ctx)])->function(arg0, arg1,
                                                                                                        arg2, arg3,
                                                                                                        arg4, arg5);
                return bridgeFunctionReturn(ctx);
            }

            template<class Class, typename FunctionType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            static duk_ret_t bridgeMethodCallV(duk_context *ctx)
            {
                METHOD_CALL_PRE
                size_t magic = duk_get_current_magic(ctx);
                Function *func = ((Function *) Context::mCurrentContext->mStorage[magic]);
                //Return (Class::*)()
                Arg0 arg0;
                Arg1 arg1;
                Arg2 arg2;
                Arg3 arg3;
                Arg4 arg4;
                Arg5 arg5;

                bridgeFunctionGetArgument(ctx, 0, &arg0);
                bridgeFunctionGetArgument(ctx, 1, &arg1);
                bridgeFunctionGetArgument(ctx, 2, &arg2);
                bridgeFunctionGetArgument(ctx, 3, &arg3);
                bridgeFunctionGetArgument(ctx, 4, &arg4);
                bridgeFunctionGetArgument(ctx, 4, &arg5);

                FunctionType fPtr = func->function;
                CALL_MEMBER(obj, fPtr)(arg0, arg1, arg2, arg3, arg4, arg5);
                return bridgeFunctionReturn(ctx);
            }


#		define CONTEXT_METHOD_PRE        if ( ctx() ) {                                                        \
                                    typedef BridgeFunctionSpec<FunctionType> Function;                        \
                                    Function* func = new Function(function, true); mStorage.push_back(func);    \

#		define CONTEXT_PRE        if ( ctx() ) {                                                        \
                                    typedef BridgeFunctionSpec<FunctionType> Function;                        \
                                    Function* func = new Function(function); mStorage.push_back(func);    \
                                    duk_push_global_object(ctx());

#       define CONTEXT_POST     duk_set_magic(ctx(), -1, duk_int_t(mStorage.size()-1));            \
                                duk_put_prop_string( ctx(), -2, bindingName ); } return this;
#       define CONSTRUCT_POST   mTmpId = duk_push_object(ctx());\
            mParents.push(bindingName);\
            Context::mTypes[typeid(Class).hash_code()] = bindingName;\
            return this;
        public:
            //Call this function to start defining a class, template requires Class and Argument types.
            template<class Class>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class > , 0);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0>
            Context *beginClass(const char *bindingName)
            {


                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0 > , 1);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0, typename Arg1>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0, Arg1 > , 2);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0, Arg1, Arg2 > , 3);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0, Arg1, Arg2, Arg3 > , 4);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0, Arg1, Arg2, Arg3, Arg4 > , 5);
                CONSTRUCT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *beginClass(const char *bindingName)
            {
                duk_push_c_function(ctx(), bridgeConstructorCall < Class, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5 > , 6);
                CONSTRUCT_POST
            }

            //Finishes declaring the class and puts it in the global namespace.
            Context *endClass()
            {
                duk_put_prop_string(ctx(), -2, "prototype");
                duk_put_global_string(ctx(), mParents.top().c_str());
                mParents.pop();
                return this;
            }

            template<class Class, typename Type>
            Context *bindProperty(Type Class::*get, const char *bindingName, bool isModifyable=false)
            {

                typedef Type (Class::*VariableType);
                if (ctx())
                {
                    typedef BridgeFunctionSpec<VariableType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeVariableCall < Class, VariableType, Type >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    if (isModifyable)
                    {
                        Function *func = new Function(get, true);
                        mStorage.push_back(func);
                        duk_push_c_function(ctx(), bridgeVariableSetCall < Class, VariableType, Type >, 1);
                        duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    }
                    duk_uint_t flags = DUK_DEFPROP_HAVE_GETTER;
                    if (isModifyable)
                        flags |= DUK_DEFPROP_HAVE_SETTER;
                    duk_def_prop(ctx(), mTmpId, flags);
                }
                return this;
            }

            template<class Class, typename Type>
            Context *bindProperty(const Type Class::*get, const char *bindingName)
            {
                typedef const Type (Class::*VariableType);
                if (ctx())
                {
                    typedef BridgeFunctionSpec<VariableType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeVariableCall < Class, VariableType, Type >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    duk_uint_t flags = DUK_DEFPROP_HAVE_GETTER;
                    duk_def_prop(ctx(), mTmpId, flags);
                }
                return this;
            }

            template<class Class, typename Return>
            Context *bindProperty(Return (Class::*get)() const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)() const;
                if (ctx())
                {
                    typedef BridgeFunctionSpec<FunctionType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    duk_def_prop(ctx(), mTmpId, DUK_DEFPROP_HAVE_GETTER);
                }
                return this;
            }
            template<class Class, typename Return>
            Context *bindProperty(Return (Class::*get)(), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)();
                if (ctx())
                {
                    typedef BridgeFunctionSpec<FunctionType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    duk_def_prop(ctx(), mTmpId, DUK_DEFPROP_HAVE_GETTER);
                }
                return this;
            }
            template<class Class, typename Return>
            Context *bindProperty(Return (Class::*get)() const, void (Class::*set)(Return), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)() const;
                typedef void (Class::*SetFunctionType)(Return);

                if (ctx())
                {
                    typedef BridgeFunctionSpec<FunctionType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    typedef BridgeFunctionSpec<SetFunctionType> SetterFunction;
                    SetterFunction *settfunc = new SetterFunction(set, true);
                    mStorage.push_back(settfunc);
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, SetFunctionType, Return >, 1);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    duk_def_prop(ctx(), mTmpId, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
                }
                return this;
            }

            template<class Class, typename Return>
            Context *bindProperty(Return (Class::*get)(), void (Class::*set)(Return), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)();
                typedef void (Class::*SetFunctionType)(Return);

                if (ctx())
                {
                    typedef BridgeFunctionSpec<FunctionType> Function;
                    Function *func = new Function(get, true);
                    mStorage.push_back(func);
                    duk_push_string(ctx(), bindingName);
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return >, 0);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    typedef BridgeFunctionSpec<SetFunctionType> SetterFunction;
                    SetterFunction *settfunc = new SetterFunction(set, true);
                    mStorage.push_back(settfunc);
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, SetFunctionType, Return >, 1);
                    duk_set_magic(ctx(), -1, duk_int_t(mStorage.size() - 1));
                    duk_def_prop(ctx(), mTmpId, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
                }
                return this;
            }

            /**
             * Make (member) function callable from JavaScript.
             */
            template<typename Return>
            Context *bindFunction(Return (*function)(), const char *bindingName)
            {

                typedef Return (*FunctionType)();
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return > , 1);
                CONTEXT_POST
            }

            template<class Class, typename Return>
            Context *bindFunction(Return (Class::*function)(), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)();
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return > , 0);
                CONTEXT_POST
                 
            }
            template<class Class, typename Return>
            Context *bindFunction(Return (Class::*function)() const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)() const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return > , 0);
                CONTEXT_POST
                 
            }

            template<typename Return, typename Arg0>
            Context *bindFunction(Return (*function)(Arg0), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0 > , 1);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0>
            Context *bindFunction(Return (Class::*function)(Arg0), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0 > , 1);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0>
            Context *bindFunction(Return (Class::*function)(Arg0) const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0 > , 1);
                CONTEXT_POST
            }

            template<typename Return, typename Arg0, typename Arg1>
            Context *bindFunction(Return (*function)(Arg0, Arg1), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0, Arg1);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1) const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<typename Return, typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(Return (*function)(Arg0, Arg1, Arg2), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0, Arg1, Arg2);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0, Arg1, Arg2 > , 3);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2 > ,
                                        3);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2) const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2 > ,
                                        3);
                CONTEXT_POST
            }

            template<typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(Return (*function)(Arg0, Arg1, Arg2, Arg3), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0, Arg1, Arg2, Arg3);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0, Arg1, Arg2, Arg3 > ,
                                        4);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3 > , 4);
                CONTEXT_POST
            }
            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3) const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3 > , 4);
                CONTEXT_POST
            }
                template<typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(Return (*function)(Arg0, Arg1, Arg2, Arg3, Arg4), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0, Arg1, Arg2, Arg3,
                                        Arg4 > , 5);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4), const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3, Arg4 > , 5);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4) const, const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3, Arg4 > , 5);
                CONTEXT_POST
            }
            template<typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(Return (*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), const char *bindingName)
            {

                typedef Return (*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCall < FunctionType, Return, Arg0, Arg1, Arg2, Arg3,
                                        Arg4, Arg5 > , 6);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5),
                                  const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3, Arg4, Arg5 > , 6);
                CONTEXT_POST
            }

            template<class Class, typename Return, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(Return (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const,
                                  const char *bindingName)
            {

                typedef Return (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCall < Class, FunctionType, Return, Arg0, Arg1, Arg2,
                                        Arg3, Arg4, Arg5 > , 6);
                CONTEXT_POST
            }

            //VOID BIND FUNCTIONS
            Context *bindFunction(void (*function)(), const char *bindingName)
            {

                typedef void (*FunctionType)();
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType > , 1);
                CONTEXT_POST
            }

            template<class Class>
            Context *bindFunction(void (Class::*function)(), const char *bindingName)
            {

                typedef void (Class::*FunctionType)();
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType > , 0);
                CONTEXT_POST
            }

            template<class Class>
            Context *bindFunction(void (Class::*function)() const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)() const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType > , 0);
                CONTEXT_POST
            }

            template<typename Arg0>
            Context *bindFunction(void (*function)(Arg0), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0 > , 1);
                CONTEXT_POST
            }

            template<class Class, typename Arg0>
            Context *bindFunction(void (Class::*function)(Arg0), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0 > , 1);
                CONTEXT_POST
            }

            template<class Class, typename Arg0>
            Context *bindFunction(void (Class::*function)(Arg0) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0 > , 1);
                CONTEXT_POST
            }

            template<typename Arg0, typename Arg1>
            Context *bindFunction(void (*function)(Arg0, Arg1), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0, Arg1);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1 > , 2);
                CONTEXT_POST
            }

            template<typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(void (*function)(Arg0, Arg1, Arg2), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0, Arg1, Arg2);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0, Arg1, Arg2 > , 3);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2 > , 3);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2 > , 3);
                CONTEXT_POST
            }

            template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(void (*function)(Arg0, Arg1, Arg2, Arg3), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0, Arg1, Arg2, Arg3);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0, Arg1, Arg2, Arg3 > , 4);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3 > ,
                                        4);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3 > ,
                                        4);
                CONTEXT_POST
            }

            template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(void (*function)(Arg0, Arg1, Arg2, Arg3, Arg4), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0, Arg1, Arg2, Arg3, Arg4 > ,
                                        5);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3,
                                        Arg4 > , 5);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3,
                                        Arg4 > , 5);
                CONTEXT_POST
            }

            template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(void (*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), const char *bindingName)
            {

                typedef void (*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
                CONTEXT_PRE
                    duk_push_c_function(ctx(), bridgeFunctionCallV < FunctionType, Arg0, Arg1, Arg2, Arg3, Arg4,
                                        Arg5 > , 6);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3, Arg4,
                                        Arg5 > , 6);
                CONTEXT_POST
            }

            template<class Class, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
            Context *bindFunction(void (Class::*function)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const, const char *bindingName)
            {

                typedef void (Class::*FunctionType)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const;
                CONTEXT_METHOD_PRE
                    duk_push_c_function(ctx(), bridgeMethodCallV < Class, FunctionType, Arg0, Arg1, Arg2, Arg3, Arg4,
                                        Arg5 > , 6);
                CONTEXT_POST
            }

# undef CONTEXT_PRE
# undef CONTEXT_POST
# undef CONSTRUCT_POST
# undef CONTEXT_METHOD_PRE
# undef CONSTRUCTOR_CALL_POST
# undef METHOD_CALL_PRE
# undef METHOD_CALL_POST
# undef CALL_MEMBER
        };

    }
}




