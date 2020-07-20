#ifndef APE_AASETOPEN_H
#define APE_AASETOPEN_H

#include <stdio.h>
#include <errno.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace ape
{
    class AAssetOpen
    {
    public:
        static FILE* open(const char* fname, const char* mode);
        static void setAssetManager(AAssetManager* manager);
    private:
        AAssetOpen() {}
        static int android_read(void* cookie, char* buf, int size);
        static int android_write(void* cookie, const char* buf, int size);
        static fpos_t android_seek(void* cookie, fpos_t offset, int whence);
        static int android_close(void* cookie);
        
        static AAssetManager* assetManager;
    };
}

#endif