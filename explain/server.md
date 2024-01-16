# サーバーのコード説明

```c
#include "header.h"
#include <pthread.h>
```
- ヘッダーファイル `header.h` がインクルードされており、pthread ライブラリも使用することを宣言しています。

```c
void showIP(void);
int createServerSocket(unsigned short port, unsigned int maxClients);
void acceptNewClients(struct client *clients, int serverSocket, int *emptyClients, int *emptyClientsCount);
```
- 3つの関数プロトタイプが宣言されています。`showIP` はIPアドレスを表示する関数、`createServerSocket` はサーバーソケットを生成する関数、`acceptNewClients` は新しいクライアントを受け入れる関数です。

```c
// functions for threads
void useThreads(struct client *clients, int maxClients, pthread_t *thread, void *(*function)(void *));
void *sendReady(void *arg);
void *checkReady(void *arg);
void *acceptConnections(void *arg);
void *sendNumbers(void *arg);
void *sendMoney(void *arg);
void *acceptNew(void *arg);
```
- スレッド関連の関数や、それを実行するための関数ポインタを含むプロトタイプが宣言されています。

```c
unsigned int *randomNumbers(void){
    unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
    numbers[0] = rand()%6+1;
    numbers[1] = rand()%6+1;
    printf("Numbers: %d, %d\n", numbers[0], numbers[1]);
    return numbers;
}
```
- 乱数を生成してその結果を表示し、それを返す関数です。

```c
int main(int argc, char *argv[]){
    // define port and maxClients
    if(argc!=3){
        printf("Usage: %s <port> <maxClients>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    unsigned short port = atoi(argv[1]);
    if(port<1024){
        // 1024 is the minimum port number that can be used
        printf("Port number must be greater than 1024\n");
        exit(EXIT_FAILURE);
    }
    int maxClients = atoi(argv[2]);
```
- プログラムのメイン関数が始まり、引数の数やポート番号などのバリデーションが行われています。

```c
    // define pthreads
    pthread_t thread[maxClients];
```
- `maxClients` 数のpthreadを定義しています。

```c
    // create socket
    int serverSocket = createServerSocket(port, maxClients);
    showIP();
    printf("%d\nWaiting for incoming connections...\n", port);
```
- サーバーソケットを生成し、IPアドレスを表示して、接続待機状態に入ります。

```c
    // create client array
    struct client *clients = (struct client *)malloc(maxClients*sizeof(struct client));
    if(clients==NULL) DieWithClose("malloc() failed", serverSocket);
    for(int i=0;i<maxClients;i++){
        clientInit(&clients[i], serverSocket);
    }
```
- `maxClients` 数のクライアント構造体の配列を動的に確保し、初期化しています。

```c
    // accept connections
    useThreads(clients, maxClients, thread, acceptConnections);
```
- スレッドを用いてクライアントの接続を待ちます。

```c
    // set empty client socket
    int *emptyClients = (int *)calloc(maxClients, sizeof(int));

    // loop
    while(1){
        // ... (この部分は繰り返し処理の本体で、以下はその一部)

        // accept new clients
        pthread_t acceptThread[emptyClientsCount];
        struct client *newClients = (struct client *)malloc(emptyClientsCount*sizeof(struct client));
        for (int i = 0; i < emptyClientsCount; i++) {
            clientInit(&newClients[i], serverSocket);
            pthread_create(&acceptThread[i], NULL, acceptNew, &newClients[i]);
        }
        for (int i = 0; i < emptyClientsCount; i++) {
            pthread_join(acceptThread[i], NULL);
            clients[emptyClients[i]] = newClients[i];
        }
        free(newClients);

        useThreads(clients, maxClients, thread, sendReady);

        // ... (以下同様に続く)
    }

    // free all clients
    free(clients);
    free(emptyClients);

    // close server socket
    close(serverSocket);

    return 0;
}
```

- `while(1)` ループが続いています。これは無限ループで、ゲームの進行を担当しています。

- ループ内で、`emptyClientsCount` 変数を使用してクライアントの状態を確認し、全てが終了した場合はループを抜けます。

- `pthread_t acceptThread[emptyClientsCount];` は新しいクライアントを受け入れるためのスレッドを管理するための配列です。

- `struct client *newClients = (struct client *)malloc(emptyClientsCount*sizeof(struct client));` は新しいクライアント構造体の配列を動的に確保します。

- 新しいクライアントの初期化とスレッドの作成が行われ、その後、`pthread_join` を使用してスレッドが終了するのを待っています。

- 新しいクライアントが作成されたら、それを元のクライアント配列に戻します。

- `useThreads(clients, maxClients, thread, sendReady);` は、クライアントに "READY" メッセージを送るためのスレッドを起動します。これはおそらく、クライアントがゲームに準備完了できたことを通知するための処理です。

- その後、おそらくゲームの進行に関する処理が続き、最終的にはクライアントの解答結果に基づいて賞金を計算し、配布する処理が行われます。

- ループを抜けると、`free(clients);` と `free(emptyClients);` でメモリを解放し、`close(serverSocket);` でサーバーソケットを閉じてプログラムが終了します。