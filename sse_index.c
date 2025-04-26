// sse_index.c
// ���� / ���� SSE ���������д�� index.db

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#define MASTER_KEY  "sse-key-123456"
#define INDEX_FILE  "index.db"

// HMAC-SHA256 ���Ϊ hex �ַ��������� 64��
void compute_trapdoor(const char* keyword, char* hex_out) {
    unsigned char* hmac;
    unsigned int len = 0;
    hmac = HMAC(EVP_sha256(),
        MASTER_KEY, strlen(MASTER_KEY),
        (const unsigned char*)keyword, strlen(keyword),
        NULL, &len);
    for (unsigned int i = 0; i < len; i++) {
        sprintf(hex_out + 2 * i, "%02x", hmac[i]);
    }
    hex_out[2 * len] = '\0';
}

// ��̬�洢һ���ؼ��ʵĵ����б�
typedef struct {
    char* keyword;
    char* trapdoor;      // hex �ַ���
    char** docs;         // �ļ�·������
    int    doc_count;
    int    doc_cap;
} Entry;

static Entry* entries = NULL;
static int    ne = 0, ce = 0;

// �� doc ���� keyword ��Ӧ�ĵ����б�ȥ�أ�
void add_posting(const char* keyword, const char* docpath) {
    // 1) ���� existing entry
    for (int i = 0; i < ne; i++) {
        if (strcmp(entries[i].keyword, keyword) == 0) {
            // ȥ��
            for (int j = 0; j < entries[i].doc_count; j++)
                if (strcmp(entries[i].docs[j], docpath) == 0)
                    return;
            // ����
            if (entries[i].doc_count == entries[i].doc_cap) {
                entries[i].doc_cap *= 2;
                entries[i].docs = realloc(entries[i].docs, sizeof(char*) * entries[i].doc_cap);
            }
            entries[i].docs[entries[i].doc_count++] = strdup(docpath);
            return;
        }
    }
    // 2) �½� entry
    if (ne == ce) {
        ce = ce ? ce * 2 : 16;
        entries = realloc(entries, sizeof(Entry) * ce);
    }
    Entry* e = &entries[ne++];
    e->keyword = strdup(keyword);
    e->trapdoor = NULL;
    e->doc_cap = 4;
    e->doc_count = 1;
    e->docs = malloc(sizeof(char*) * e->doc_cap);
    e->docs[0] = strdup(docpath);
}

// �ݹ����Ŀ¼���������� .txt �ļ�
void traverse_dir(const char* dir) {
    DIR* dp = opendir(dir);
    if (!dp) return;
    struct dirent* ent;
    char path[4096];
    struct stat st;
    while ((ent = readdir(dp))) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);
        if (stat(path, &st) < 0) continue;
        if (S_ISDIR(st.st_mode)) {
            traverse_dir(path);
        }
        else if (S_ISREG(st.st_mode)) {
            // ֻ���� .txt �ļ�
            const char* ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0) {
                FILE* f = fopen(path, "r");
                if (!f) continue;
                char word[256];
                while (fscanf(f, "%255s", word) == 1) {
                    add_posting(word, path);
                }
                fclose(f);
            }
        }
    }
    closedir(dp);
}

// ���ڴ��еĵ�������д�� INDEX_FILE
void dump_index() {
    FILE* out = fopen(INDEX_FILE, "w");
    if (!out) { perror("fopen"); exit(1); }
    char td[EVP_MAX_MD_SIZE * 2 + 1];
    for (int i = 0; i < ne; i++) {
        // ��������
        compute_trapdoor(entries[i].keyword, td);
        fprintf(out, "%s", td);
        for (int j = 0; j < entries[i].doc_count; j++)
            fprintf(out, " %s", entries[i].docs[j]);
        fprintf(out, "\n");
    }
    fclose(out);
    printf("Index built: %d keywords\n", ne);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_dir>\n", argv[0]);
        return 1;
    }
    traverse_dir(argv[1]);
    dump_index();
    return 0;
}
