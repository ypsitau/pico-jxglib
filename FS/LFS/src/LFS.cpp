//==============================================================================
// LFS.cpp
//==============================================================================
#include "jxglib/LFS.h"

#if 0
// LittleFSのコンフィグレーション
lfs_t lfs;
lfs_config cfg = {
    // 必要に応じて以下の設定を変更してください
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

void initializeLFS() {
    // LittleFSの初期化
    int err = lfs_mount(&lfs, &cfg);
    if (err) {
        // マウントに失敗した場合はフォーマットして再マウント
        lfs_format(&lfs, &cfg);
        err = lfs_mount(&lfs, &cfg);
        if (err) {
            // 再マウント失敗時のエラーハンドリング
            printf("Failed to mount LFS\n");
            return;
        }
    }
    printf("LFS mounted successfully\n");
}

void writeFile(const char* path, const char* data) {
    // ファイルを書き込む
    lfs_file_t file;
    int err = lfs_file_open(&lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT);
    if (err) {
        printf("Failed to open file for writing\n");
        return;
    }
    lfs_file_write(&lfs, &file, data, strlen(data));
    lfs_file_close(&lfs, &file);
    printf("File written successfully\n");
}

void readFile(const char* path) {
    // ファイルを読み込む
    lfs_file_t file;
    int err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err) {
        printf("Failed to open file for reading\n");
        return;
    }
    char buffer[128];
    int bytesRead = lfs_file_read(&lfs, &file, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the string
        printf("File content: %s\n", buffer);
    }
    lfs_file_close(&lfs, &file);
}

void unmountLFS() {
    // LittleFSのアンマウント
    lfs_unmount(&lfs);
    printf("LFS unmounted successfully\n");
}
#endif

#if 0
int main() {
    initializeLFS();

    // ファイル操作の例
    writeFile("/hello.txt", "Hello, LittleFS!");
    readFile("/hello.txt");

    unmountLFS();
    return 0;
}
#endif
