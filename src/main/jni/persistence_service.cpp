#include <jni.h>
#include <android/log.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "persistence_service.h"

#define LOG_TAG "PersistenceService"

void selfReplicate() {
    const char* source = "/data/data/com.omega.breachers/lib/libbreachers_cheat.so";
    const char* dest = "/data/local/tmp/libbreachers_cheat_backup.so";

    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(dest, std::ios::binary);

    if (src && dst) {
        dst << src.rdbuf();
        chmod(dest, 0755);
        LOGD("Self-replication successful: %s", dest);
    } else {
        LOGE("Self-replication failed");
    }
}

void installInitScript() {
    const char* script = R"(
#!/system/bin/sh
insmod /data/local/tmp/libbreachers_cheat_backup.so
    )";

    std::ofstream init("/data/local/tmp/breachers_init.sh");
    if (init) {
        init << script;
        chmod("/data/local/tmp/breachers_init.sh", 0755);
        LOGD("Init script installed");
    }
}

void ensurePersistence() {
    selfReplicate();
    installInitScript();

    // Add to startup via /data/adb/post-fs-data.d (requires root)
    // This is a placeholder for root-based persistence
}

extern "C" JNIEXPORT void JNICALL
Java_com_omega_breachers_MainActivity_ensurePersistence(JNIEnv* env, jobject obj) {
    ensurePersistence();
}
