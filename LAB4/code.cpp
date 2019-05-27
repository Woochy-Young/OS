#include "stdio.h"
#include"stdlib.h"
#include"time.h"
#define P 32    //�����ڴ����
#define V 64   //�����ڴ����
#define block 3 //�����ڴ����
double  clockBegin, clockEnd;//���㿪��

int* memo;//�洢��
void initMemo();
bool isInMemo (int n);
void generate();//���ɷ�������

void OPT (int n);
void testOptimal();

void LRU (int n);
void testLRU();

typedef struct node
{
    int num;
    int flag;//����λ
    int modify;//�޸�λ//PBA��
    node* next;
} Node, *pNode;
typedef struct queue
{
    int n;
    pNode front;
    pNode rear;

} Queue, *pQueue;//queue�޷�����
void initQueue (pQueue q);//��ʼ
void push (pQueue q, int num);//
void pop (pQueue q);//
void destroy (pQueue q);//����
bool findInQueue (pQueue q, int num);//��
void testfifo();
void fifo (pQueue q, int num);//�㷨

struct LNode
{
    int data;
    int flag;//����λ
    int modify;//�޸�λ

}* nodes;
void updated_Clock (int n);
void testClock();


struct Link
{
    int num;//��ǰ�����ϵĽ����
    Node* next;
};
Link idle;
Link modified;
void addToLink (int data, int type);
void emptyIdle();
void emptyModi();
void PBA (pQueue q,int num);
void testPBA ();

int access[V]; //��������
int lost = 0;//ȱҳ��
int index = 0;//ָʾ��ǰ�ڴ��±�



int main (int argc, char* argv[])
{
    generate();

    int i = 0,choose;
    printf ("\n\nҳ���������Ϊ�� ");
    for (; i < V; i++)
    {
        printf ("%d, ", access[i]);
    }
    printf ("\n");
    while(1)
    {
        printf ("\nѡ��1.OPT;2.LRU;3.FIFO;4.CLOCK;5.PBA�� ");
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

void generate()//���Ͼֲ��������Ե���������㷨
{
    srand ( (unsigned) time (NULL)); //��ʱ�����֣�ÿ�β����������һ��
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

void initMemo()//��ʼ���洢�ռ�
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
    printf ("����û��㷨��\n");

    for (; i < V; i++)
    {
        OPT (i);
        printf ("%d %d %d\n", memo[0], memo[1], memo[2]);
    }
    clockEnd=clock();
    printf ("����û��㷨ȱҳ�ʣ�%f��ȱҳ��%d����ʱ��%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    lost = 0;
    free (memo);
    index = 0;
}

//�����Ӧ�㷨
void OPT (int n)
{
    int i = 0, j = 0;

    if (!isInMemo (n))//����
    {

        if (index == block)
        {
            lost++;
            int max = 0, pos, tag;//max��ò��ã�������pos

            for (i = 0; i < block; i++)//һ�����
            {
                tag = -1;

                for (j = n + 1; j < V; j++)//�����һ��ʹ��λ��tag
                {
                    if (access[j] == memo[i])
                    {
                        tag = j;
                        break;
                    }
                }

                if (tag == -1)//֮����ʹ��
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

        else//Ԥ��
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
    printf ("������δʹ���㷨:\n");

    for (i = 0; i < V; i++)
    {
        LRU (i);
        printf ("%d %d %d\n", memo[0], memo[1], memo[2]);
    }
clockEnd=clock();
    printf ("������δʹ��ȱҳ�ʣ�%f��ȱҳ��%d����ʱ��%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    lost = 0;
    index = 0;
    free (memo);
}

//�㷨
void LRU (int n)
{
    int i, j;

    if (!isInMemo (n))

        if (index == block)
        {
            lost++;
            int max = n, pos = -1, tag;//max���

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
                        break;//���
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
    printf ("�Ƚ��ȳ��û��㷨:\n");

    for (; i < V; i++)
    {
        fifo (&q, access[i]);
        p = q.front->next;//��һλ0

        while (p)
        {
            printf ("%d ", p->num);
            p = p->next;
        }

        printf ("\n");
    }
clockEnd=clock();
    printf ("�Ƚ��ȳ��㷨ȱҳ�ʣ�%f ,ȱҳ��%d����ʱ��%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    destroy (&q);
    lost=0;
    index=0;
}

void fifo (pQueue q, int num)
{
    if (!findInQueue (q, num))
    {
        if (q->n == block)//����
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
        if (index == block)//����
        {
            lost++;
            int i = 0, tag = -1;//tagҪ������

            while (true)
            {
                if ( (i / block) % 2 == 0)//��һ��
                {
                    if (nodes[i % block].flag == 0 && nodes[i % block].modify == 0)
                    {
                        tag = i % block;//��¼
                        break;
                    }
                }

                if ( (i / block) % 2 == 1)//�ڶ���
                {
                    if (nodes[i % block].flag == 0 && nodes[i % block].modify == 1)
                    {
                        tag = i % block;
                        break;
                    }

                    else
                    {
                        nodes[i % block].flag = 0;//������������ҳ�����λ��0
                    }
                }

                i++;
            }

            nodes[tag].data = access[n];
            nodes[tag].flag = 1;

            if (rand() % 10 < 4)//����޸ĸ�ҳ
                nodes[tag].modify = 1;
            else
                nodes[tag].modify = 0;
        }

        else//Ԥװ
        {
            nodes[index].data = access[n];
            nodes[index].flag = 1;

            if (rand() % 10 < 4)//����޸ĸ�ҳ
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
    printf ("�Ľ���Clock�û��㷨\n");
    nodes = (LNode*) malloc (block * sizeof (LNode));

    for (i = 0; i < block; i++)//��ʼ��
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
    printf ("�Ľ���Clock�û��㷨ȱҳ�ʣ� %f��ȱҳ�� %d����ʱ��%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
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
        if (p->num == n)//�ڿ�������
        {
            if (q != NULL)
            {
                q->next = p->next;
                p->next = NULL;
                idle.num--;//�Ƴ�
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

    if (p == NULL)//���޸�
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
                    modified.num--;//�Ƴ�
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

void addToLink (int data, int type)//ҳ����ӵ����޸�ҳ������Ϳ���������
{
    node* p;
    node* q;
    q = (node*) malloc (sizeof (node));
    q->num = data;
    q->modify = 1;

    if (type == 1)//�޸�����
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

        if (idle.num == 20)//һ���������
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
    printf ("ҳ�滺���û��㷨:\n");

    for (; i < V; i++)
    {
        PBA (&q, access[i]);
        p = q.front->next;//��һλ0

        while (p)
        {
            printf ("%d ", p->num);
            p = p->next;
        }

        printf ("\n");
    }
clockEnd=clock();
    printf ("ҳ�滺���㷨ȱҳ�ʣ�%f ,ȱҳ��%d����ʱ��%f\n", lost*1.0/V, lost,clockEnd-clockBegin);
    destroy (&q);
     lost=0;
    index=0;
}

void PBA (pQueue q, int num)
{
    Node *p;
    if (!findInQueue (q, num))
    {
        if (q->n == block)//����
        {
            if ( (p = isinLinks (num)) != NULL)//������
            {
                pop (q);
                push (q, p->num);
            }
            else//����
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
