/* Copyright (c) 2019 The Khronos Group Inc.
 * Copyright (c) 2019 Valve Corporation
 * Copyright (c) 2019 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: John Zulauf <jzulauf@lunarg.com>
 *
 */
#pragma once
#ifndef TYPESAFE_VULKAN_
#define TYPESAFE_VULKAN_

#include "cast_utils.h"

// Flag that non dispatchable handles are always typesafe
#define TYPESAFE_NON_DISPATCHABLE_HANDLES

#ifdef VK_DEFINE_NON_DISPATCHABLE_HANDLE
#error Must include this header before including vulkan/vulkan.h
#endif

#if !(defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || \
      defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__))
// 32bit default macro doesn't allow for type safe operations or type struct.
// Flag that we're using the 32 bit non dispatchable handles (i.e. they aren't pointers)
#define TYPESAFE_32BIT_NON_DISPATCHABLE_HANDLES

struct TypeSafeVkHandleBase {
   protected:
    uint64_t payload;

   public:
    operator uint64_t() const { return payload; }
    TypeSafeVkHandleBase() : payload(0) {}
    explicit TypeSafeVkHandleBase(uint64_t payload_) : payload(payload_) {}
    explicit TypeSafeVkHandleBase(const void *pointer) : payload(reinterpret_cast<uint64_t>(pointer)) {}
    TypeSafeVkHandleBase &operator=(uint64_t payload_) {
        payload = payload_;
        return *this;
    }

    template <typename T>
    TypeSafeVkHandleBase &operator=(const T *pointer) {
        payload = static_cast<uint64_t>(pointer);
        return *this;
    }
};

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(T)                                       \
    struct T : public TypeSafeVkHandleBase {                                       \
        T() : TypeSafeVkHandleBase() {}                                            \
        T(int payload_) : TypeSafeVkHandleBase(static_cast<uint64_t>(payload_)) {} \
        T(uint64_t payload_) : TypeSafeVkHandleBase(payload_) {}                   \
        T(const T &from) : TypeSafeVkHandleBase(from.payload) {}                   \
        explicit T(const void *pointer) : TypeSafeVkHandleBase(pointer) {}         \
        T &operator=(uint64_t value) {                                             \
            payload = value;                                                       \
            return *this;                                                          \
        }                                                                          \
        bool operator==(const T &rhs) const { return payload == rhs.payload; }     \
        bool operator==(int value) const { return payload == value; }              \
        bool operator==(uint64_t value) const { return payload == value; }         \
    };

#endif

#endif
