//
// Created by roelof on 2/9/16.
//

#include "Context.h"
#include "../Exception/JSONException.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
namespace rengin {
    namespace Script {
        Context* Context::mCurrentContext = nullptr;
        std::map<int, std::string> Context::mTypes;

        Context::~Context()
        {
            mCurrentContext = nullptr;
            for (auto it = mStorage.begin(); it != mStorage.end(); it++)
            {
                delete (*it);
            }
        }


        void Context::parse(std::string fileName)
        {
            if (duk_peval_file(ctx(), fileName.c_str()) != 0) {
                throw duk_safe_to_string(ctx(), -1);
            }
            duk_pop(ctx());
        }


    }
}