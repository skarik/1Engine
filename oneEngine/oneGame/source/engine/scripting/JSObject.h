//
// Created by rdsnijder on 3/11/16.
//

#ifndef RENGIN_JSOBJECT_H
#define RENGIN_JSOBJECT_H

#include "JSStack.h"
#include <string>
#include <map>

namespace rengin {
    namespace Script {
        class JSObject;

        struct JSProperty
        {
        protected:
            virtual JSStack *putTop(const std::vector<std::string> &) const
            {
                throw "parent calls!";
                return nullptr;
            };
            std::vector<std::string> keys;
            JSProperty *root;
        public:
            const std::string key;
            const unsigned int level;

            JSProperty(const std::vector<std::string> &pKeys, const std::string &key, unsigned int level,
                       JSProperty *root) : keys(pKeys), root(root), key(key), level(level)
            {
                if (level != 0)
                {
                    keys.push_back(key);
                }
            }

            template<typename Return>
            Return value() const
            {
                return root->putTop(keys)
                           ->top<Return>();

            }

            JSProperty operator[](std::string key)
            {
                return JSProperty(keys, key, level + 1, root);
            };

            std::string path()
            {
                std::string result = "[";
                for (auto it = keys.begin(); it != keys.end(); it++)
                    result += (*it) + "]->[";
                result[result.size() - 4] = ']';
                return result.substr(0, result.size() - 3);
            }
        };

        class JSObject : public JSProperty
        {
        private:

            JSStack *mStack;

            const std::vector<std::string> mRoot;

            bool isSubPath() const
            {
                auto otherPath = mStack->path();

                if (mRoot.size() > otherPath.size())
                    return false;

                for (unsigned int i = 0; i < mRoot.size(); i++)
                {
                    if (mRoot[i] != otherPath[i])
                        return false;
                }
                return true;
            }

            JSStack *putTop(const std::vector<std::string> &keys) const
            {
                const std::vector<std::string>& stack = mStack->path();
                auto targetStack = mRoot;
                if (targetStack.empty())
                    targetStack = keys;
                else
                    targetStack.insert(targetStack.end(), keys.begin(), keys.end());

                unsigned int stackId = stack.size();
                if (stackId > 0)
                {
                    stackId--;
                    for (unsigned int i = stackId; i > 0; i--)
                    {
                        if (i >= targetStack.size() || stack[i] != targetStack[i])
                        {
                            stackId = i;
                            mStack->pop();
                        }
                    }
                }
                for (; stackId < targetStack.size(); stackId++)
                {
                    mStack->push(targetStack[stackId]);
                }
                return mStack;
            }

        public:
            JSObject(JSStack *stack) : JSProperty(std::vector<std::string>(), "root", 0, nullptr), mStack(stack),
                                       mRoot(stack->path())
            {
                root = this;
            }

        };

    }
}


#endif //RENGIN_JSOBJECT_H
