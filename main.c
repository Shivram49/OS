//Including the required libraries
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//definitions
#define TOTAL_QUEUES 10
#define ROW_SIZE 10

//structure definitions
typedef struct customer
{
    int arrival_time;
    int customer_Id;
    int res_t;
    int tat;
} customer;

typedef struct customerQueue_struct
{
    customer *client;
} customerQueue;

typedef struct row_struct
{
    pthread_mutex_t mutex;
} row;

typedef struct pthread_args_struct
{
    int tempid;
    int r_id;
    char *sell_typ;
    customerQueue *cq;
} pthread_args;

// seat status enum
enum seat_state
{
    AVAILABLE,
    SOLD,
    PROCESSING
};

typedef struct seat_struct
{
    int id;
    int count;
    enum seat_state state;
    customer *cust;
    pthread_args *p_args;
} seat;

struct seat_manager_struct
{
    int free_seats;
    seat *h_seat;
    seat *m_seat;
    seat *l_seat;
} seat_manager;

// setting up the parameters
customerQueue *cQ;
row tickets[10];
static int serve_time = 0;
int pt = 0;

int h_cust = 0;
int m_cust = 0;
int l_cust = 0;

int throughput_H = 0;
int t_a_t_H = 0;
int response_t_H = 0;
int total_h_cust = 0;

int throughput_L1 = 0;
int t_a_t_L1 = 0;
int response_t_L1 = 0;
int total_l1_cust= 0;

int throughput_L2 = 0;
int t_a_t_L2 = 0;
int response_t_L2 = 0;
int total_l2_cust = 0;

int throughput_L3 = 0;
int t_a_t_L3 = 0;
int response_t_L3 = 0;
int total_l3_cust = 0;

int throughput_L4 = 0;
int t_a_t_L4 = 0;
int response_t_L4 = 0;
int total_l4_cust = 0;

int throughput_L5 = 0;
int t_a_t_L5 = 0;
int response_t_L5 = 0;
int total_l5_cust = 0;

int throughput_L6 = 0;
int t_a_t_L6 = 0;
int response_t_L6 = 0;
int total_l6_cust = 0;

int throughput_M1 = 0;
int t_a_t_M1 = 0;
int response_t_M1 = 0;
int total_m1_cust = 0;

int throughput_M2 = 0;
int t_a_t_M2 = 0;
int response_t_M2 = 0;
int total_m2_cust = 0;

int throughput_M3 = 0;
int t_a_t_M3 = 0;
int response_t_M3 = 0;
int total_m3_cust = 0;

// total amount of customers
int N;

// establishing the thread's parameters
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t seat_lock = PTHREAD_MUTEX_INITIALIZER; // The lock for the seat manager struct

// producing 100 seats
seat theater[100];

//for reproducibility


// Outputs theater
void display_seats()
{
    int i, j, index;
    for (i = 0; i < 10; i++)
    {
        printf("Row %d |", i);
        for (j = 0; j < 10; j++)
        {
            index = i * 10 + j;
            if (theater[index].state == AVAILABLE)
            {
                printf(" S%02d: --------- |", theater[index].id);
            }
            else
            {
                if (theater[index].cust == NULL)
                {
                    printf("error found NULL\n");
                }
                else
                {
                    printf(" S%02d: %s%d-%02d-C%02d |", theater[index].id, theater[index].p_args->sell_typ, theater[index].p_args->tempid, theater[index].count, theater[index].cust->customer_Id);
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

// initializes theater
void theater_initialize()
{
    int i;
    for (i = 0; i < 100; i++)
    {
        theater[i].state = AVAILABLE;
        theater[i].id = i;
        theater[i].cust = NULL;
        theater[i].p_args = NULL;
    }
}

// initializes seat manager
void seat_manager_init()
{
    seat_manager.h_seat = &theater[0];
    seat_manager.m_seat = &theater[40];
    seat_manager.l_seat = &theater[99];
    seat_manager.free_seats = 100;
}

// moves on to seat h
void next_h_seat()
{
    seat *temporary_seat = seat_manager.h_seat;
    while (temporary_seat->state != AVAILABLE && seat_manager.free_seats > 0)
    {
        temporary_seat++;
    }
    seat_manager.h_seat = temporary_seat;
}

// moves on to seat m
void next_m_seat()
{
    seat *temporary_seat = seat_manager.m_seat;
    while ((temporary_seat->id==50)||(temporary_seat->state != AVAILABLE && seat_manager.free_seats > 0))
    {
        // To go to the preceding row, subtract 19 if the seat ID is 99, 89, 79, etc.

        switch (temporary_seat->id)
        {
            case 50:
                temporary_seat = &theater[30];
                continue;

            case 40 ... 49:
                if(temporary_seat->id==49)temporary_seat = &theater[20];
                else temporary_seat++;
                continue;

            case 10 ... 19:
                if(temporary_seat->id==19)temporary_seat = &theater[60];
                else temporary_seat++;
                continue;

            case 30 ... 39:
                if(temporary_seat->id==39)temporary_seat = &theater[20];
                else temporary_seat++;
                continue;

            case 51 ... 59:
                if(temporary_seat->id==59)temporary_seat = &theater[10];
                else temporary_seat++;
                continue;

            case 20 ... 29:
                if(temporary_seat->id==29)temporary_seat = &theater[51];
                else temporary_seat++;
                continue;

            case 60 ... 69:
                if(temporary_seat->id==69)temporary_seat = &theater[70];
                else temporary_seat++;
                continue;



            case 70 ... 79:
                if(temporary_seat->id==79)temporary_seat = &theater[0];
                else temporary_seat++;
                continue;



            case 80 ... 89:
                if(temporary_seat->id==89)temporary_seat = &theater[90];
                else temporary_seat++;
                continue;

            case 0 ... 9:
                if(temporary_seat->id==9)temporary_seat = &theater[80];
                else temporary_seat++;
                continue;

            default:
                temporary_seat++;
        }
    }
    seat_manager.m_seat = temporary_seat;
}

// moves on to seat l
void next_l_seat()
{
    seat *temporary_seat = seat_manager.l_seat;

    while (temporary_seat->state != AVAILABLE && seat_manager.free_seats > 0 && temporary_seat->id >=0)
    {


        // try the next seat
        temporary_seat--;
    }
    seat_manager.l_seat = temporary_seat;
}

// sells h seat
seat *h_pick_free_seat()
{
    seat *allocated_seat_to_sell;
    pthread_mutex_lock(&seat_lock);
    if (seat_manager.free_seats > 0)
    {
        // Find a free seat if the one the seat manager referred to isn't available.
        if (seat_manager.h_seat->state != AVAILABLE)
            next_h_seat();
        allocated_seat_to_sell = seat_manager.h_seat;
        allocated_seat_to_sell->state = PROCESSING;
        seat_manager.free_seats--;
        h_cust++;

    }
    else
    {
        allocated_seat_to_sell = NULL;
    }
    pthread_mutex_unlock(&seat_lock);
    return allocated_seat_to_sell;
}

// sells m seat
seat *m_pick_free_seat()
{
    seat *allocated_seat_to_sell;
    pthread_mutex_lock(&seat_lock);
    if (seat_manager.free_seats > 0)
    {
        //Find a free seat if the one the seat manager referred to isn't available.

        next_m_seat();
        allocated_seat_to_sell = seat_manager.m_seat;
        printf("M seat: %d \n",seat_manager.m_seat->id);
        allocated_seat_to_sell->state = PROCESSING;
        seat_manager.free_seats--;
        m_cust++;
        seat_manager.m_seat++;
    }
    else
    {
        allocated_seat_to_sell = NULL;
    }
    pthread_mutex_unlock(&seat_lock);
    return allocated_seat_to_sell;
}

// sells l seat
seat *l_pick_free_seat()
{
    seat *allocated_seat_to_sell;
    pthread_mutex_lock(&seat_lock);
    if (seat_manager.free_seats > 0)
    {
        // Find a free seat if the one the seat manager referred to isn't available.
        if (seat_manager.l_seat->state != AVAILABLE)
            next_l_seat();
        allocated_seat_to_sell = seat_manager.l_seat;
        printf("L seat:%d\n", seat_manager.l_seat->id);
        allocated_seat_to_sell->state = PROCESSING;
        seat_manager.free_seats--;
        l_cust++;

    }
    else
    {
        allocated_seat_to_sell = NULL;
    }
    pthread_mutex_unlock(&seat_lock);
    return allocated_seat_to_sell;
}

// one quanta will be served by the seller thread.
void *thread_for_seller(pthread_args *pargs)
{
    int customer_index = 0;
    int all_done = 0;
    int process_time = 0;
    int done_time = 0;
    seat *sts;
    customer *nc;

    while (1)
    {
        // clock barrier
        pthread_mutex_lock(&mutex_cond);
        pthread_cond_wait(&cond, &mutex_cond);
        pthread_mutex_unlock(&mutex_cond);

        // Verify if we are presently having a sale before continuing.
        if (serve_time < done_time)
        {

            continue;
        }

        // If the next customer has not yet arrived, skip this turn.
        if (pargs->cq->client[customer_index].arrival_time > serve_time)
        {

            continue;
        }

        // Obtain the client if we are not yet finished.
        if (customer_index < N)
        {
            nc = &(pargs->cq->client[customer_index++]);
        }
        else
        {
            if (all_done == 0)
            {
                all_done = 1;
            }
            continue;
        }

        // Currently, we have a customer to serve. Please have a seat, since the wait time will vary based on the seller.
        if (strcmp(pargs->sell_typ, "H") == 0)
        {
            sts = h_pick_free_seat();
            process_time = 1 + (rand() % 1);
            nc->res_t=serve_time-nc->arrival_time;
        }
        else if (strcmp(pargs->sell_typ, "M") == 0)
        {
            sts = m_pick_free_seat();
            process_time = 2 + (rand() % 2);
            nc->res_t=serve_time-nc->arrival_time;
        }
        else
        {
            sts = l_pick_free_seat();
            process_time = 4 + (rand() % 4);
            nc->res_t=serve_time-nc->arrival_time;
        };

        if (sts == NULL)
        {

            break;
        }

        // concluding the customer's processing
        done_time = serve_time + process_time;
        nc->tat = done_time - nc->arrival_time +1;
        sts->cust = nc;
        sts->p_args = pargs;
        sts->state = SOLD;
        if (strcmp(pargs->sell_typ, "H") == 0)
        {
            total_h_cust++;
            throughput_H++;

            t_a_t_H = t_a_t_H + nc->tat;

            response_t_H = response_t_H + nc->res_t;
            printf("Thread H1: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);
            sts->count = total_h_cust;
        }

        if (strcmp(pargs->sell_typ, "M") == 0)
        {
            if (pargs->tempid == 1)
            {
                total_m1_cust++;
                sts->count = total_m1_cust;

                throughput_M1++;
                t_a_t_M1 = t_a_t_M1 + nc->tat;
                response_t_M1 = response_t_M1 + nc->res_t;
                printf("Thread M1: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);

            }
            if (pargs->tempid == 2)
            {
                total_m2_cust++;
                sts->count = total_m2_cust;

                throughput_M2++;
                t_a_t_M2 =t_a_t_M2 + nc->tat;
                response_t_M2 = response_t_M2 + nc->res_t;
                printf("Thread M2: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);

            }
            if (pargs->tempid == 3)
            {
                total_m3_cust++;
                sts->count = total_m3_cust;

                throughput_M3++;
                t_a_t_M3 = t_a_t_M3 + nc->tat;
                response_t_M3 = response_t_M3 + nc->res_t;
                printf("Thread M3: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);

            }
        }

        if (strcmp(pargs->sell_typ, "L") == 0)
        {
            if (pargs->tempid == 4)
            {
                total_l1_cust++;

                sts->count = total_l1_cust;

                throughput_L1++;
                t_a_t_L1 = t_a_t_L1 + nc->tat;
                response_t_L1 = response_t_L1 + nc->res_t;
                printf("Thread L4: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);


            }
            if (pargs->tempid == 5)
            {
                total_l2_cust++;
                sts->count= total_l2_cust;

                throughput_L2++;
                t_a_t_L2 = t_a_t_L2 + nc->tat;
                response_t_L2 = response_t_L2+ nc->res_t;
                printf("Thread L5: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);


            }
            if (pargs->tempid == 6)
            {
                total_l3_cust++;
                sts->count = total_l3_cust;

                throughput_L3++;
                t_a_t_L3= t_a_t_L3 + nc->tat;
                response_t_L3 = response_t_L3 + nc->res_t;
                printf("Thread L6: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);


            }
            if (pargs->tempid == 7)
            {
                total_l4_cust++;
                sts->count = total_l4_cust;

                throughput_L4++;
                t_a_t_L4 = t_a_t_L4 + nc->tat;
                response_t_L4 = response_t_L4 + nc->res_t;
                printf("Thread L7: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);


            }
            if (pargs->tempid == 8)
            {
                total_l5_cust++;
                sts->count = total_l5_cust;

                throughput_L5++;
                t_a_t_L5 = t_a_t_L5 + nc->tat;
                response_t_L5 = response_t_L5 + nc->res_t;
                printf("Thread L8: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);


            }
            if (pargs->tempid == 9)
            {
                total_l6_cust++;
                sts->count = total_l6_cust;

                throughput_L6++;
                t_a_t_L6 = t_a_t_L6 + nc->tat;
                response_t_L6 = response_t_L6 + nc->res_t;
                printf("Thread L9: End time = %d Arrival time = %d customer id = C%02d response time = %d Service time = %d\n",done_time,nc->arrival_time,nc->customer_Id,nc->res_t,serve_time);

            }
        };
        pt++;
    }
    // thread exits
    return NULL;
}

// launches each thread
void seller_thread_all_start()
{
    pthread_mutex_lock(&mutex_cond);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex_cond);
}

// in order to compare arrival times
int arrival_times_comparator(const void *a, const void *b)
{
    customer *c1 = (customer *)a;
    customer *c2 = (customer *)b;
    int r = c1->arrival_time - c2->arrival_time;
    if (r > 0)
        return 1;
    if (r < 0)
        return -1;
    return r;
}

// create the customer queues
void create_customer_queue(int N)
{
    int i, j, arrival_time;
    cQ = (customerQueue *)malloc(sizeof(customerQueue) * TOTAL_QUEUES);

    // for reproducibility

    for (i = 0; i < TOTAL_QUEUES; ++i)
    {
        cQ[i].client = (customer *)malloc(sizeof(customer) * N);
        for (j = 0; j < N; ++j)
        {
            // creating arbitrary arrival times
            arrival_time = rand() % 60;
            cQ[i].client[j].customer_Id = i * N + j; // i*N+j to get a global customer ID
            cQ[i].client[j].arrival_time = arrival_time;
        }
    }

    //order customers according to arrival times
    for (i = 0; i < TOTAL_QUEUES; ++i)
    {
        qsort((void *) cQ[i].client, N, sizeof(customer), arrival_times_comparator);
    }

    // initialize all mutex value
    for (i = 0; i < ROW_SIZE; ++i)
    {
        pthread_mutex_init(&tickets[i].mutex, NULL);
    }
}

// Outputs customer queue
void display_customer_queue(int N)
{
    int i, j;

    printf("\n--------------CUSTOMER QUEUE WITH ID and ARRIVAL TIME----------------------\n");
    for (i = 0; i < TOTAL_QUEUES; ++i)
    {

        for (j = 0; j < N; ++j)
        {

            printf(" %03d:%02d |", cQ[i].client[j].customer_Id, cQ[i].client[j].arrival_time);
        }
        printf("\n----------------------------------------------");
        printf("\n");
    }
}

// begins the simulation
int main(int argc, char *argv[])
{

    pthread_args *pargs;

    //for reproducibility
    time_t t;
    srand((unsigned)time(&t));

    theater_initialize();
    seat_manager_init();

    printf("Enter the number of customers: ");
    scanf("%d", &N);
    create_customer_queue(N);
    display_customer_queue(N);

    int i;
    pthread_t tids[10];
    pargs = (pthread_args *)malloc(sizeof(pthread_args));

    // H
    pargs->sell_typ = (char *)malloc(strlen("H") + 1);
    memcpy(pargs->sell_typ, "H", strlen("H"));
    pargs->r_id = 0;
    pargs->tempid = 0;
    pargs->cq = &cQ[0];
    pthread_create(&tids[0], NULL, (void *) thread_for_seller, (void *)pargs);

    // M
    for (i = 1; i < 4; i++)
    {
        pargs = (pthread_args *)malloc(sizeof(pthread_args));
        pargs->sell_typ = (char *)malloc(strlen("M") + 1);
        memcpy(pargs->sell_typ, "M", strlen("M"));
        pargs->r_id = i;
        pargs->tempid = i;
        pargs->cq = &cQ[i];
        pthread_create(&tids[i], NULL, (void *) thread_for_seller, (void *)pargs);
    }

    // L
    for (i = 4; i < 10; i++)
    {
        pargs = (pthread_args *)malloc(sizeof(pthread_args));
        pargs->sell_typ = (char *)malloc(strlen("L") + 1);
        memcpy(pargs->sell_typ, "L", strlen("L"));
        pargs->r_id = i;
        pargs->tempid = i;
        pargs->cq = &cQ[i];
        pthread_create(&tids[i], NULL, (void *) thread_for_seller, (void *)pargs);
    }

    // begin simulation clock
    for (i = 0; i < 500; i++)
    {
        seller_thread_all_start();
        usleep(30);

        serve_time++;
    }

    printf("\n\n After Sale:\n");
    display_seats();

    double th_h = total_h_cust/60.0;
    double th_m = m_cust/180.0;
    double th_l = l_cust/360.0;

    int th_nh = total_h_cust/1;
    int th_nm= m_cust/3;
    int th_nl = l_cust/6;



    printf("\n\n Seats Allotted: Total: %d | Turn away: %d \n",
           (h_cust + m_cust + l_cust), N * 10 - (h_cust + m_cust + l_cust));

    printf("\n Average Turn around Time (H): %f  |  Average Response Time (H): %f  |  Throughput (H): %f ", t_a_t_H == 0 ? 0 : t_a_t_H / total_h_cust*1.0,
           response_t_H == 0 ? 0 : response_t_H / total_h_cust*1.0,
           th_h);
    printf("\n Average Turn around Time (M): %f  |  Average Response Time (M): %f  |  Throughput (M): %f ", t_a_t_M1 + t_a_t_M2 + t_a_t_M3 == 0 ? 0 : t_a_t_M1 + t_a_t_M2 + t_a_t_M3 / m_cust*1.0,
           response_t_M1 + response_t_M2 + response_t_M3 == 0 ? 0 : response_t_M1 + response_t_M2 + response_t_M3 / m_cust*1.0,
           th_m);
    printf("\n Average Turn around Time (L): %f  |  Average Response Time (L): %f  |  Throughput (L): %f \n",
           t_a_t_L1 + t_a_t_L2 + t_a_t_L3 + t_a_t_L4 + t_a_t_L5 + t_a_t_L6 == 0 ? 0 : t_a_t_L1 + t_a_t_L2 + t_a_t_L3 + t_a_t_L4 + t_a_t_L5 + t_a_t_L6 / l_cust*1.0,
           response_t_L1 + response_t_L2 + response_t_L3 + response_t_L4 + response_t_L5 + response_t_L6 == 0 ? 0 : response_t_L1 + response_t_L2 + response_t_L3 + response_t_L4 + response_t_L5 + response_t_L6 / l_cust*1.0,
           th_l);

}
