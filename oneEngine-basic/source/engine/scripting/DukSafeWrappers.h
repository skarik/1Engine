//
// Created by rdsnijder on 3/10/16.
//

#ifndef RENGIN_DUKSAFEWRAPPERS_H
#define RENGIN_DUKSAFEWRAPPERS_H

#include "../../duktape-1.4.0/duktape.h"

static duk_ret_t json_decode_wrapper(duk_context *ctx)
{
    duk_json_decode(ctx,-1);
    return 1;
}
#endif //RENGIN_DUKSAFEWRAPPERS_H
