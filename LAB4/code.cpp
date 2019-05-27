#include "stdio.h"
#include"stdlib.h"
#include"time.h"
#define P 32    //物理内存块数
#define V 64   //虚拟内存块数
#define block 3 //分配内存块数
double  clockBegin, clockEnd;//计算开销

int* memo;//存储快
void initMemo();
bool isInMemo (int n);
void generate();//生成访问序列

void OPT (int n);
void testOptimal();

void LRU (int n);
void testLRU();

typedef struct node
{
    int num;
    int flag;//访问位
    int modify;//修改位//PBA用
    node* next;
} Node, *pNode;
typedef struct queue
{
    int n;
    pNode front;
    pNode rear;

} Queue, *pQueue;//queue无法遍历
void initQueue (pQueue q);//初始
void push (pQueue q, int num);//
void pop (pQueue q);//
void destroy (pQueue q);//销毁
bool findInQueue (pQueue q, int num);//查
void testfifo();
void fifo (pQueue q, int num);//算法

struct LNode
{
    int data;
    int flag;//访问位
    int modify;//修改位

}* nodes;
void updated_Clock (int n);
void testClock();


struct Link
{
    int num;//当前链表上的结点数
    Node* next;
};
Link idle;
Link modified;
void addToLink (int data, int type);
void emptyIdle();
void emptyModi();
void PBA (pQueue q,int num);
void testPBA ();

int access[V]; //访问序列
int lost = 0;//缺页数
int index = 0;//指示当前内存下标



int main (int argc, char* argv[])
{
    generate();

    int i = 0,choose;
    printf ("\n\n页面访问序列为： ");
    for (; i < V; i++)
    {
        printf ("%d, ", access[i]);
    }
    printf ("\n");
    while(1)
    {
        printf ("\n选择1.OPT;2.LRU;3.FIFO;4.CLOCK;5.PBA： ");
        scanf("%d",&choose);
        printf ("\n");
        if(choose==1) testOptimal();
        else if(choose==2) testLRU();
        else if(choose==3) testfifo();
        else if(choose==4) testClock();
        else if(choose==5) testPBA();
    }
    /*

    */

    getchar();
    return 0;
}

void generate()//符合局部访问特性的随机生成算法
{
    srand ( (unsigned) time (NULL)); //用时间做种，每次产生随机数不一样
    int p = rand() % V;
    int m = 16, e = 8, i, j;
    double t;
    t = rand() % 10 / 10.0;

    for (i = 0; i < V/m; i++)
    {
        for (j = i * m; j < (i + 1) *m; j++)
        {
            access[j] = (p + rand() % e) % V;
        }

        double r = (rand() % 10) / 10.0;

        if (r < t)
            p = rand() % V;
        else
            p = (p + 1) % V;
    }
}

void initMemo()//初始化存储空间
{
    memo = (int*) malloc (block * sizeof (int));
    int i = 0;

    for (; i < block; i++)
    {
        memo[i] = -1;
    }

}

bool  isInMemo (int n)
{
    int i = 0;

    for (; i < block; i++)
    {
        if (access[n] == memo[i])
        {
            return true;
        }
    }

    return false;
}

///OPT
void testOptimal()
{
    clockBegin=clock();
    initMemo();
    int i = 0;
    printf ("最佳置换算法：\n");

    for (; i < V; i++)
    {
        OPT (i);
        printf ("%d %d %d\n", memo[0], memo[1], memo[2]);
    }
    clockEnd=clock();
    printf ("最佳置换算法缺页率：%f，缺页：%d，耗时：%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    lost = 0;
    free (memo);
    index = 0;
}

//最佳适应算法
void OPT (int n)
{
    int i = 0, j = 0;

    if (!isInMemo (n))//不在
    {

        if (index == block)
        {
            lost++;
            int max = 0, pos, tag;//max最久不用；换掉第pos

            for (i = 0; i < block; i++)//一块块算
            {
                tag = -1;

                for (j = n + 1; j < V; j++)//算出下一次使用位置tag
                {
                    if (access[j] == memo[i])
                    {
                        tag = j;
                        break;
                    }
                }

                if (tag == -1)//之后不再使用
                {
                    max = V;
                    pos = i;
                    break;
                }

                if (max < tag)
                    {
                        max = tag;
                        pos = i;
                    }

            }

            memo[pos] = access[n];
        }

        else//预存
        {
            memo[index] = access[n];
            index++;
        }
    }

}

///LRU
void testLRU()
{
    clockBegin=clock();
    int i;
    initMemo();
    printf ("最近最久未使用算法:\n");

    for (i = 0; i < V; i++)
    {
        LRU (i);
        printf ("%d %d %d\n", memo[0], memo[1], memo[2]);
    }
clockEnd=clock();
    printf ("最近最久未使用缺页率：%f，缺页：%d，耗时：%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    lost = 0;
    index = 0;
    free (memo);
}

//算法
void LRU (int n)
{
    int i, j;

    if (!isInMemo (n))

        if (index == block)
        {
            lost++;
            int max = n, pos = -1, tag;//max最久

            for (i = 0; i < block; i++)
            {
                for (j = n - 1; j >= 0; j--)
                {
                    if (access[j] == memo[i])
                    {
                        tag = j;
                        break;
                    }
                }

                if (tag < max)
                {
                    max = tag;
                    pos = i;

                    if (max == 0)
                    {
                        break;//最久
                    }
                }
            }

            memo[pos] = access[n];
        }

        else
        {
            memo[index] = access[n];
            index++;
        }
}


///QUEUE
void initQueue (pQueue q)
{
    q->rear = (pNode) malloc (sizeof (Node));

    if (q->rear == NULL)
    {
        printf ("failed\n");
    }

    else
    {
        q->front = q->rear;
        q->rear->next = NULL;
        q->front->next = NULL;
        q->n = 0;
    }
}

void push (pQueue q, int num)
{
    pNode p = (pNode) malloc (sizeof (Node));

    if (p == NULL)
    {
        printf ("failed");
    }

    else
    {
        p->next = NULL;
        p->num = num;
        p->flag=1;
        if (rand() % 10 < 4)
                {
                    p->modify = 0;
                }

                else
                {
                    p->modify = 1;
                }

        if (q->front == q->rear)
        {
            q->front->next = p;
            q->rear = p;
        }

        else
        {
            q->rear->next = p;
            q->rear = p;
        }

        q->n++;
    }
}

void pop (pQueue q)
{
    pNode p;

    if (q->front != q->rear)
    {
        p = q->front->next;
        q->front->next = p->next;

        if (p == q->rear)
        {
            q->front = q->rear;
        }

        q->n--;
        free (p);
    }
}

void destroy (pQueue q)
{
    while (q->front != q->rear)
    {
        pop (q);
    }
}

bool findInQueue (pQueue q, int num)
{
    pNode p;

    if (q->front != q->rear)
    {
        p = q->front->next;

        while (p)
        {
            if (p->num == num)
            {
                return true;
            }

            else
            {
                p = p->next;
            }
        }
    }

    return false;
}
///FIFO
void testfifo()
{
    clockBegin=clock();
    Queue q;
    pNode p;
    initQueue (&q);
    int i = 0;
    printf ("先进先出置换算法:\n");

    for (; i < V; i++)
    {
        fifo (&q, access[i]);
        p = q.front->next;//第一位0

        while (p)
        {
            printf ("%d ", p->num);
            p = p->next;
        }

        printf ("\n");
    }
clockEnd=clock();
    printf ("先进先出算法缺页率：%f ,缺页：%d，耗时：%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    destroy (&q);
    lost=0;
    index=0;
}

void fifo (pQueue q, int num)
{
    if (!findInQueue (q, num))
    {
        if (q->n == block)//满了
        {
            pop (q);
            push (q, num);
            lost++;
        }

        else
        {
            push (q, num);
        }
    }
}


bool isInNodes (int n)
{
    int i;

    for (i = 0; i < block; i++)
    {
        if (nodes[i].data == access[n])
        {
            return true;
        }
    }

    return false;
}

void updated_Clock (int n)
{
    if (!isInNodes (n))
        if (index == block)//满了
        {
            lost++;
            int i = 0, tag = -1;//tag要换掉的

            while (true)
            {
                if ( (i / block) % 2 == 0)//第一遍
                {
                    if (nodes[i % block].flag == 0 && nodes[i % block].modify == 0)
                    {
                        tag = i % block;//记录
                        break;
                    }
                }

                if ( (i / block) % 2 == 1)//第二遍
                {
                    if (nodes[i % block].flag == 0 && nodes[i % block].modify == 1)
                    {
                        tag = i % block;
                        break;
                    }

                    else
                    {
                        nodes[i % block].flag = 0;//将经过的所有页面访问位置0
                    }
                }

                i++;
            }

            nodes[tag].data = access[n];
            nodes[tag].flag = 1;

            if (rand() % 10 < 4)//随机修改该页
                nodes[tag].modify = 1;
            else
                nodes[tag].modify = 0;
        }

        else//预装
        {
            nodes[index].data = access[n];
            nodes[index].flag = 1;

            if (rand() % 10 < 4)//随机修改该页
                nodes[index].modify = 1;
            else
                nodes[index].modify = 0;

            index++;
        }
}
void testClock()
{
    clockBegin=clock();
    int i = 0, j = 0;
    printf ("改进型Clock置换算法\n");
    nodes = (LNode*) malloc (block * sizeof (LNode));

    for (i = 0; i < block; i++)//初始化
    {
        nodes[i].data = -1;
        nodes[i].flag = -1;
        nodes[i].modify = -1;
    }

    for (i = 0; i < V; i++)
    {
        updated_Clock (i);

        for (j = 0; j < block; j++)
        {
            printf ("%d ", nodes[j].data);
        }

        printf ("\n");
    }
clockEnd=clock();
    printf ("改进型Clock置换算法缺页率： %f，缺页： %d，耗时：%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    lost = 0;
    index = 0;
}

///linklist
node* isinLinks (int n)
{
    node*p, *q;
    p = idle.next;
    q = NULL;

    while (p)
    {
        if (p->num == n)//在空闲链表
        {
            if (q != NULL)
            {
                q->next = p->next;
                p->next = NULL;
                idle.num--;//移除
                break;
            }

            else
            {
                idle.next = NULL;
            }
        }

        q = p;
        p = p->next;
    }

    if (p == NULL)//在修改
    {
        p = modified.next;

        while (p != NULL)
        {
            if (p->num == n)
            {
                if (p == modified.next)
                { modified.next = p->next; }

                else
                {
                    q->next = p->next;
                    p->next = NULL;
                    modified.num--;//移除
                }

                if (modified.num == 0)
                { modified.next = NULL; }

                break;
            }

            q = p;
            p = p->next;
        }
    }

    return p;
}

void addToLink (int data, int type)//页面添加到已修改页面链表和空闲链表上
{
    node* p;
    node* q;
    q = (node*) malloc (sizeof (node));
    q->num = data;
    q->modify = 1;

    if (type == 1)//修改链表
    {
        q->modify = 1;
        p = modified.next;
    }

    else
    {
        q->modify = 0;
        p = idle.next;
    }

    q->next = NULL;

    if (p == NULL)
    {
        if (type == 0)
        {
            idle.next = q;
        }

        else
        {
            modified.next = q;
        }
    }

    else
    {
        while (p)
        {
            if (p->next == NULL)
            {
                p->next = q;
                break;
            }

            else
            {
                p = p->next;
            }
        }
    }

    if (type == 0)
    {
        idle.num += 1;

        if (idle.num == 20)//一定数量清空
        {
            emptyIdle();
        }
    }

    else
    {
        modified.num += 1;

        if (modified.num == 20)
        {
            emptyModi();
        }
    }
}

void emptyIdle ()
{
    node* p;
    p = idle.next;

    while (p)
    {
        idle.next = p->next;
        free (p);
        p = idle.next;
    }

    idle.num = 0;
}

void emptyModi()
{
    node* p;
    p = modified.next;

    while (p)
    {
        modified.next = p->next;
        free (p);
        p = modified.next;
    }

    modified.num = 0;
}

void testPBA()
{
    clockBegin=clock();
    Queue q;
    pNode p;
    initQueue (&q);
    int i = 0;
    printf ("页面缓冲置换算法:\n");

    for (; i < V; i++)
    {
        PBA (&q, access[i]);
        p = q.front->next;//第一位0

        while (p)
        {
            printf ("%d ", p->num);
            p = p->next;
        }

        printf ("\n");
    }
clockEnd=clock();
    printf ("页面缓冲算法缺页率：%f ,缺页：%d，耗时：%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    destroy (&q);
     lost=0;
    index=0;
}

void PBA (pQueue q, int num)
{
    Node *p;
    if (!findInQueue (q, num))
    {
        if (q->n == block)//满了
        {
            if ( (p = isinLinks (num)) != NULL)//在链表
            {
                pop (q);
                push (q, p->num);
            }
            else//不在
            {
                lost++;
                if(q->front->modify==1)
                    addToLink(num,1);
                else addToLink(num,0);
                pop(q);
                push(q,num);
            }

        }

        else
        {
            push (q, num);
        }
    }
}
