/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2013, Advanced Micro Devices, Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the OpenCV Foundation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "precomp.hpp"

#ifdef HAVE_CLAMDBLAS

#include "opencv2/core/opencl/runtime/opencl_core.hpp"
#include "opencv2/core/opencl/runtime/opencl_clamdblas.hpp"

#if defined(_WIN32)
#include <windows.h>

    static void* WinGetProcAddress(const char* name)
    {
        static HMODULE opencl_module = NULL;
        if (!opencl_module)
        {
            opencl_module = GetModuleHandleA("clAmdBlas.dll");
            if (!opencl_module)
            {
                opencl_module = LoadLibraryA("clAmdBlas.dll");
                if (!opencl_module)
                    return NULL;
            }
        }
        return (void*)GetProcAddress(opencl_module, name);
    }
    #define CV_CL_GET_PROC_ADDRESS(name) WinGetProcAddress(name)
#endif // _WIN32

#if defined(__linux__)
    #include <dlfcn.h>
    #include <stdio.h>

    static void* GetProcAddress (const char* name)
    {
        static void* h = NULL;
        if (!h)
        {
            h = dlopen("libclAmdBlas.so", RTLD_LAZY | RTLD_GLOBAL);
            if (!h)
                return NULL;
        }

        return dlsym(h, name);
    }
    #define CV_CL_GET_PROC_ADDRESS(name) GetProcAddress(name)
#endif

#ifndef CV_CL_GET_PROC_ADDRESS
#define CV_CL_GET_PROC_ADDRESS(name) NULL
#endif

static void* openclamdblas_check_fn(int ID);

#include "runtime_common.hpp"

//
// BEGIN OF CUSTOM FUNCTIONS
//

#define CUSTOM_FUNCTION_ID 1000

//
// END OF CUSTOM FUNCTIONS HERE
//

#include <dlfcn.h>
static void* AppleCLGetProcAddress2(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle)
    {
        if(!initialized)
        {
            initialized = true;
            const char* path = "/Users/albertachtenberg/Work/temp/clBLAS/src/library/libclBLAS.dylib";

            handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
            if (handle == NULL)
            {
                fprintf(stderr, "WTF?!");
            }
        }
        if (!handle)
            return NULL;
    }
    return dlsym(handle, name);
}

#include "autogenerated/opencl_clamdblas_impl.hpp"

static void* openclamdblas_check_fn(int ID)
{
    assert(ID >= 0 && ID < (int)(sizeof(openclamdblas_fn)/sizeof(openclamdblas_fn[0])));
    const struct DynamicFnEntry* e = openclamdblas_fn[ID];
    void* func = AppleCLGetProcAddress2(e->fnName);
    if (!func)
    {
        CV_Error(cv::Error::OpenCLApiCallError, cv::format("OpenCL AMD BLAS function is not available: [%s]", e->fnName));
    }
    *(e->ppFn) = func;
    return func;
}

#endif
