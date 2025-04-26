// sse_search.c
// SERVER: 监听 9000，接 Trapdoor，查 index.db，返回匹配文档列表
// CLIENT: 生成 Trapdoor，连 127.0.0.1:9000，发送并打印返回结果

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#ifdef SERVER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(CLIENT)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define MASTER_KEY   "sse-key-123456"
#define INDEX_FILE   "index.db"
#define SERVER_PORT  9000
#define MAXLINE      4096

// 同 sse_index.c
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

#ifdef SERVER

// 一行一条记录：trapdoor [space] doc1 [space] doc2 ...
void serve_loop() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    char buf[MAXLINE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);
    printf("SSE server listening on port %d …\n", SERVER_PORT);

    while (1) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        // 1) 读取客户端 Trapdoor（以 '\n' 结尾）
        int n = 0;
        char td[MAXLINE];
        while (n < MAXLINE - 1) {
            int r = read(connfd, td + n, 1);
            if (r <= 0 || td[n] == '\n') break;
            n++;
        }
        td[n] = '\0';
        printf("Received trapdoor: %s\n", td);

        // 2) 打开索引文件，查找匹配行并发送结果
        FILE* f = fopen(INDEX_FILE, "r");
        if (f) {
            while (fgets(buf, MAXLINE, f)) {
                if (strncmp(buf, td, n) == 0 && buf[n] == ' ') {
                    // buf+n+1 为以空格分隔的文档列表
                    // 在 serve_loop() 里
                    ssize_t nw = write(connfd, buf + n + 1, strlen(buf + n + 1));
                    if (nw < 0) {
                        perror("write to client failed");
                        // 根据需要决定是跳出、continue 还是其他处理
                    }

                    // 在 do_search() 里
                    ssize_t ns = write(socket, td, strlen(td));
                    if (ns < 0) {
                        perror("write trapdoor failed");
                        exit(1);
                    }
                    ns = write(socket, "\n", 1);
                    if (ns < 0) {
                        perror("write newline failed");
                        exit(1);
                    }
                    break;
                }
            }
            fclose(f);
        }
        close(connfd);
    }
}

int main() {
    serve_loop();
    return 0;
}

#elif defined(CLIENT)

void do_search(const char* keyword) {
    // 1) 生成 Trapdoor
    char td[EVP_MAX_MD_SIZE * 2 + 1];
    compute_trapdoor(keyword, td);

    // 2) 连接服务器
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }

    // 3) 发送 Trapdoor + '\n'
    write(sockfd, td, strlen(td));
    write(sockfd, "\n", 1);

    // 4) 接收并打印服务器返回的文档列表
    printf("Results for \"%s\":\n", keyword);
    char buf[MAXLINE];
    int n;
    while ((n = read(sockfd, buf, MAXLINE - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    close(sockfd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <keyword>\n", argv[0]);
        return 1;
    }
    do_search(argv[1]);
    return 0;
}

#else

#error "Please compile with -DSERVER or -DCLIENT"

#endif
