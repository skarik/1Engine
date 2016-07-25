//
// Created by rdsnijder on 3/10/16.
//

#ifndef RENGIN_JSONCONTEXT_H
#define RENGIN_JSONCONTEXT_H

#include "../../duktape-1.4.0/duktape.h"
#include "JSStack.h"
#include "HeapContainer.h"

namespace rengin { namespace Script {
        class JSONContext : public HeapContainer
        {
        public:
            JSONContext() : HeapContainer()
            {
            }
            JSStack parse(const std::string& json) const;
            JSStack parseFile(const std::string& file) const;


        };
    }}
#endif //RENGIN_JSONCONTEXT_H
