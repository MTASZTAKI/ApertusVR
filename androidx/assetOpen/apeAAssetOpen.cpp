#include "apeAAssetOpen.h"

AAssetManager* ape::AAssetOpen::assetManager;

FILE* ape::AAssetOpen::open(const char* fname, const char* mode)
{
    if(mode[0] == 'w') return nullptr;

    AAsset* asset = AAssetManager_open(assetManager, fname, 0);
    if(!asset) return nullptr;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

int ape::AAssetOpen::android_read(void* cookie, char* buf, int size)
{
    int rdsz = AAsset_read((AAsset*)cookie, buf, size);
    return rdsz;
}

int ape::AAssetOpen::android_write(void* cookie, const char* buf, int size)
{
    return EACCES; // can't provide write access to the apk
}

fpos_t ape::AAssetOpen::android_seek(void* cookie, fpos_t offset, int whence)
{
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

int ape::AAssetOpen::android_close(void* cookie)
{
    AAsset_close((AAsset*)cookie);
    return 0;
}

void ape::AAssetOpen::setAssetManager(AAssetManager* manager)
{
    assetManager = manager;
}