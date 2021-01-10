#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 999 /* 無限大の距離 */
#define END_OF_PATH -1 /* 経路の距離 */

/* ノード情報を表す構造体 */
struct node {
    int dist; /* 始点からの経路長 */
    int flag; /*経路の終端 */
    int path; /* 最短経路を構成する隣接ノード */
    char *name; /* 都市(駅)名*/
};

/* 隣接リストを表す構造体 */
struct edge {
    int dst; /* 終点 */
    int weight; /* 所要時間 */
    char *line; /* 経路名 */
    struct edge *next; /* 次のedgeへのポインタ */
};

struct node *node_dat; /* ノード情報 */
struct edge **list; /* 隣接リスト */
int nodes;

/* 連結リストへ新しいセルを挿入する関数 */
void insert_cell (struct edge **ptr, int dat, int weight, char *line)
{
    struct edge *new_cell; /* 新しいセル */
    
    /* 新しいセルを確保する */
    new_cell = (struct edge *)malloc (sizeof (struct edge));
    
    /* データを格納する */
    new_cell -> dst = dat;
    new_cell -> weight = weight;
    if (line == NULL)
        new_cell -> line == NULL;
    else {
        new_cell -> line = (char *)malloc ((strlen (line) + 1) * sizeof (char));
        strcpy (new_cell -> line, line);
    }
    
    /* リンクを更新する */
    new_cell -> next = *ptr;
    *ptr = new_cell;
}

/* アクセスポイント情報を読み込む関数 */
void read_station()
{
    int n; /* アクセスポイント番号 */
    char station[80]; /* アクセスポイント名 */
    FILE *fp; /* ファイルポインタ */
    
    /* アクセスポイントが格納されたファイルをオープン */
    if ((fp = fopen ("station.dat", "r")) == NULL) {
        fprintf (stderr, "File not found: station.dat \n");
        exit (1);
    }
    
    /* ノードの数を読み込む */
    fscanf (fp, "%d", &nodes);
    
    /* nodes件分のノード情報領域を確保する */
    node_dat = (struct node *)malloc (nodes * sizeof (struct node));
    
    /* データを読み込む */
    while (fscanf (fp, "%d %s", &n, station) != EOF) {
        node_dat[n].name
        = (char *)malloc ((strlen (station) + 1) * sizeof (char));
        strcpy (node_dat[n].name, station); /* アクセスポイントを登録する */
    }
    fclose (fp);

}

/* 経路情報を読み込む関数 */
void read_line()
{
    int u, v, weight; /* 接続情報 */
    int len; /* 経路名の文字列の長さ */
    char line[80]; /* 経路名 */
    FILE *fp; /* ファイルポインタ */
    
    /* nodes件分の隣接リストのヘッダ領域を確保する */
    list = (struct edge **)malloc (nodes * sizeof (struct edge *));
    
    /* 隣接リストの全てのヘッダをNULLにする */
    for (u = 0; u < nodes; u++)
    list[u] = NULL;
    
    /* 隣接リストの全てのヘッダをNULLにする */
    for (u = 0; u < nodes; u++)
    list[u] = NULL;
    
    /* 経路情報が格納されたファイルをオープン */
    if ((fp = fopen ("line.dat", "r")) == NULL) {
        fprintf (stderr, "File not found: line.dat \n");
        exit(1);
    }
    
    /* データを読み込む */
    while (fscanf (fp, "%d %d %d", &u, &v, &weight) != EOF) {
        fgets(line, 80, fp);
        len = strlen (line);
        if (line[len - 1] = '\n')
            line[len - 1] == '\0';
        insert_cell(&(list[u]), v, weight, line); /* 辺を登録する */
        insert_cell(&(list[v]), u, weight, line);
    }
    fclose (fp);
}

/* ダイクストラ法による最短経路探索 */
void dijkstra (int start, int end)
{
    int u, x; /* ループ用 */
    int min; /* 最短経路長の最小値 */
    struct edge *ptr; /* 隣接リスト */
    
    /* ノードstartを囲みの中に入れる */
    node_dat[start].flag = 1;
    
    /* ノードstartに隣接するノードの最短経路長をノードstart間の距離に */
    /* 最短経路への導く隣接ノードをstartにそれぞれ設定する */
    for (ptr = list[start]; ptr != NULL; ptr = ptr -> next)
    if (node_dat[ptr -> dst].dist = ptr -> weight) {
        node_dat[ptr -> dst].dist = ptr -> weight;
        node_dat[ptr -> dst].path = start;
    }
    
    /* ノードstartの最短経路長を0とする */
    node_dat[start].dist = 0;
    
    /* ノードstartを最短経路の終端とする */
    node_dat[start].path = END_OF_PATH;
    
    u = start;
    while (u != end) {
        min = INF;
        for (x = 0; x < nodes; x++) /* 囲みの中にないノードxを調査する */
        if (node_dat[x].flag != 1 && min > node_dat[x].dist) {
            min = node_dat[x].dist; /* 最短経理長の最小値を求める */
            u = x;
        }
        
        /* 最小値を持つノードuを囲みの中に入れる */
        node_dat[u].flag = 1;
        
        /* 囲みの中にないノードuの隣接ノードを調査する */
        for (ptr = list[u]; ptr != NULL; ptr -> next)
        if (node_dat[ptr -> dst].flag != 1)
            if (node_dat[ptr -> dst].dist > node_dat[u].dist + ptr -> weight) {
                /* ノードuを経由した方が距離が短いなら距離を更新する */
                node_dat[ptr -> dst].dist = node_dat[u].dist + ptr -> weight;
                /* ノードuを経由するよう最短経路を更新する */
                node_dat[ptr -> dst].path = u;
            }
    }
}

/* 経路を表す連結リストの生成 */
void make_path (struct edge **ptr, int start, int end)
{
    int n; /* ループ用 */
    int min; /* 経路長の最小値 */
    struct edge *work, *tmp; /* 作業用 */
    
    *ptr = NULL;
    insert_cell (ptr, end, 0, NULL); /* 終点をリストに登録する */
    for (n = end; node_dat[n].path != END_OF_PATH; n = node_dat[n].path) {
        min = INF;
        for (work = list[node_dat[n].path]; work != NULL; work = work -> next) {
            if (work -> dst == (*ptr) -> dst && min > work -> weight) {
                min = work -> weight;
                tmp = work;
            }
        }
        
        /* 終点に近い方から経路情報をリストに登録する */
        insert_cell(ptr, node_dat[n].path, tmp -> weight, tmp -> line);
    }
}

int main (int argc, char *argv[])
{
    int i; /* ループ用 */
    int start, end; /* 始点、終点ノード番号 */
    struct edge *ptr, *work; /* 作業用 */
    struct edge *path; /* 最短経路 */
    
    /* 引数の数のチェック */
    if (argc < 3) {
        fprintf (stderr,
                 "Please give a start point and a destinaiton as arguments. \n");
        exit(1);
    }
    
    read_station();

read_station(); /* アクセスポイント情報の読み込み */

/* 始点ノード番号の設定 */
start = -1;
for (i = 0; i < nodes; i++)
if (strcmp (argv[1], node_dat[i].name) == 0) {
    start = i;
    break;
}

if (start == -1) {
    fprintf(stderr, "Start point is not registered. \n");
    exit (1);
}

/* 終点ノード番号の設定 */
end = -1;
for (i = 0; i < nodes; i ++)
if (strcmp (argv[2], node_dat[i].name) == 0) {
    end = i;
    break;
}
    if (end == -1) {
        fprintf (stderr, "Destination is not registered. \n");
        exit(1);
    }
    
    read_line(); /* 経路情報の読み込み */
    
    for (i = 0; i < nodes; i++){
        node_dat[i].dist = INF; /* 最短経路長を無限大(未探索)にする */
        node_dat[i].flag = 0; /* 探索状況フラグをゼロクリアする */
    }
    
    dijkstra(start, end); /* 最短経路探索 */
    
    make_path (&path, start, end); /* 最短経路を並び替える */
    
    /* 最短経路長および最短経路を表示する */
    printf ("The shortest path from %s to %s\n",
            node_dat[start].name, node_dat[end].name);
    for (ptr = path; ptr -> next != NULL; ptr= ptr -> next) {
        work = ptr -> next;
        printf ("   %17s -> %17s  ",
                node_dat[ptr -> dst].name, node_dat[work -> dst].name);
        printf ("by %19s for %3d [min.]\n", ptr -> line, ptr -> weight);
    }
    printf("Total time required ... %d [min.]\n", node_dat[end].dist);
    
    return 0;
}


