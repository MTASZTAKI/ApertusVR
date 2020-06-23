#include "android_fopen.h"

AAssetManager* android_asset_manager;

FILE* android_fopen(const char* fname, const char* mode)
{
    if(mode[0] == 'w') return NULL;

    AAsset* asset = AAssetManager_open(android_asset_manager, fname, 0);
    if(!asset) return NULL;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

int android_read(void* cookie, char* buf, int size) {
    int rdsz = AAsset_read((AAsset*)cookie, buf, size);
    return rdsz;
}

int android_write(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

int android_close(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

void android_fopen_set_asset_manager(AAssetManager* manager) {
    android_asset_manager = manager;
}
