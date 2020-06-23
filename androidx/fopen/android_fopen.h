#ifndef ANDROID_FOPEN_H
#define ANDROID_FOPEN_H

#include <stdio.h>
#include <errno.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

FILE* android_fopen(const char* fname, const char* mode);
int android_read(void* cookie, char* buf, int size);
int android_write(void* cookie, const char* buf, int size);
fpos_t android_seek(void* cookie, fpos_t offset, int whence);
int android_close(void* cookie);
void android_fopen_set_asset_manager(AAssetManager* manager);


#endif
