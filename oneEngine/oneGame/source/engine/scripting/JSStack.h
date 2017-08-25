//
// Created by rdsnijder on 3/8/16.
//

#ifndef RENGIN_JSON_H
#define RENGIN_JSON_H
#include <string>
#include <stack>
#include <iostream>
#include "../../duktape-1.4.0/duktape.h"
#include "../Exception/RenginException.h"
#include "Function.h"
namespace rengin {
    namespace Script {

    class JSStack
    {
    private:
        duk_context* ctx;
        duk_idx_t mRoot;
        std::vector<std::string> mPath;

        inline void getPropString(std::string key) const
        {
            duk_get_prop_string(ctx, -1, key.c_str());
        }


        inline void setPath(std::vector<std::string> path)
        {
            clear();
            for (unsigned int i = 0; i < path.size(); i++)
            {
                mPath.push_back(path[i]);
                getPropString(path[i]);
                duk_to_object(ctx, -1);
            }
        }

    public:
        JSStack(duk_context* context) : ctx(context)
        {
            mRoot = duk_get_top(context);
            mPath.reserve(10);
        }
        ~JSStack()
        {
            clear();
            duk_pop(ctx);
        }

        JSStack& push(const std::string& key)
        {
            if (mPath.size()==0)
                duk_to_object(ctx, -1);

            mPath.push_back(key);
            getPropString(key);
            return *this;
        }

        inline JSStack& pop(const unsigned int& amount = 1)
        {
            for (unsigned int i = 0; i < amount && i < mPath.size(); i++)
            {
                duk_pop(ctx);
                mPath.pop_back();
            }
            return *this;
        }
        template <typename Return>
        inline Return top()
        {
            Return result;
            getTOS(ctx, result);
            return result;
        }



        inline void clear()
        {
            duk_idx_t top = duk_get_top(ctx);

            while (top != mRoot)
            {
                duk_pop(ctx);
                top = duk_get_top(ctx);
            }
            mPath.clear();
        }
        unsigned int level() const
        {
            return mPath.size();
        }

        const std::vector<std::string>& path() const { return mPath; }
        std::string pathToString() const
        {
            std::string result = "[";
            for (auto it = mPath.begin(); it != mPath.end(); it++)
                result += (*it) + "]->[";
            result[result.size() - 4] = ']';
            return result.substr(0, result.size() - 3);
        }

        void contextDump() const
        {
            duk_push_context_dump(ctx);
            std::cout << duk_to_string(ctx, -1) << std::endl;
            duk_pop(ctx);
        }
    };

    }
}
#endif //RENGIN_JSON_H
