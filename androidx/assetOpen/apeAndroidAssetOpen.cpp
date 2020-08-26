#include "apeAndroidAssetOpen.h"

AAssetManager* ape::AndroidAssetOpen::assetManager;

FILE* ape::AndroidAssetOpen::open(const char* fname, const char* mode)
{
    if(mode[0] == 'w') return nullptr;

    AAsset* asset = AAssetManager_open(assetManager, fname, 0);
    if(!asset) return nullptr;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

int ape::AndroidAssetOpen::android_read(void* cookie, char* buf, int size)
{
    int rdsz = AAsset_read((AAsset*)cookie, buf, size);
    return rdsz;
}

int ape::AndroidAssetOpen::android_write(void* cookie, const char* buf, int size)
{
    return EACCES; // can't provide write access to the apk
}

fpos_t ape::AndroidAssetOpen::android_seek(void* cookie, fpos_t offset, int whence)
{
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

int ape::AndroidAssetOpen::android_close(void* cookie)
{
    AAsset_close((AAsset*)cookie);
    return 0;
}

void ape::AndroidAssetOpen::setAssetManager(AAssetManager* manager)
{
    assetManager = manager;
}