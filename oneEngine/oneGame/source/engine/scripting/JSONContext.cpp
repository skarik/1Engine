//
// Created by rdsnijder on 3/10/16.
//

#include "JSONContext.h"
#include "JSStack.h"
#include "DukSafeWrappers.h"
#include "../Exception/JSONException.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

rengin::Script::JSStack rengin::Script::JSONContext::parse(const std::string& json) const {
    duk_push_string(ctx(), json.c_str());
    duk_int_t ret = duk_safe_call(ctx(), json_decode_wrapper, 1, 1);

    if (ret == DUK_EXEC_ERROR)
    {
        const char* message = duk_to_string(ctx(), -1);
        duk_pop(ctx());

        throw Exception::JSONException(message, json);
    }

    return Script::JSStack(ctx());
}

rengin::Script::JSStack rengin::Script::JSONContext::parseFile(const std::string& file) const {
    string json = "";
    ifstream filestream;
    filestream.open(file);
    while(!filestream.eof())
    {
        string buf;
        getline(filestream, buf);
        json += buf;
    }
    filestream.close();
    return parse(json);
}
