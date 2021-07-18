#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include<stdlib.h>

#include "macro_utils.h"
#include "lightweightsemaphore.h"

#define PLINE(fmt,args...)  printf(fmt"\n", ##args)

#define LW_SEM

#ifdef LW_SEM
    typedef lw_sem_t Sem;

    #define SEM_INIT(sem,thread,v) lw_sem_init(sem,thread,v,128)
    #define SEM_WAIT(sem)          lw_sem_wait(sem)
    #define SEM_TRY_WAIT(sem)      lw_sem_try_wait(sem)
    #define SEM_POST(sem)          lw_sem_signal(sem)
    
    #define SUCCESS                true
    #define FAILED                 false

#else
    typedef sem_t Sem;

    #define SEM_INIT(sem,thread,v) sem_init(sem,thread,v)
    #define SEM_WAIT(sem)          sem_wait(sem)
    #define SEM_TRY_WAIT(sem)      sem_trywait(sem)
    #define SEM_POST(sem)          sem_post(sem)
    
    #define SUCCESS                0
    #define FAILED                 -1
#endif

Sem *semPush = NULL;
Sem *semPop = NULL;

#define NSEND  2
#define NRCV   1


//#define WAIT

static void * shm_create(size_t size, int * id){
    int shmid = 0;
    void * shm = NULL;

	time_t t;

	srand((unsigned) time(&t));

	for(key_t key = rand();;key = rand()){
    	shmid = shmget(key, size, 0666|IPC_CREAT);
    	if(shmid ==-1)
    	{
			if(errno == EACCES){
				return NULL;
			}

    	    continue;
    	}

        // we got a valide shmid here
        break;
	}

    shm = shmat(shmid,0,0);
    if(shm ==(void*)-1)
    {
        return NULL;
    }

    *id = shmid;

	return shm;
}

bool shm_free(int id){
    shmctl(id, IPC_RMID, 0);

    return true;
}

void send(){
	int pid = 0;
    int i = 0;

    struct timeval start, end;
    int total_time = 0;
    double total_time_sec = 0.0;


    pid = getpid();

    gettimeofday(&start, NULL);


    PLINE("Send start.");
#ifdef WAIT
    PLINE("waitting");
    sleep(30);
#endif
    for(i = 0; ;){
		if(SUCCESS != SEM_TRY_WAIT(semPush)){
            continue;
		}

		SEM_POST(semPop);
		
		i ++;
    }
}


void recv(){
	int pid = 0;
    int i = 0;
    
    struct timeval start, end;
    int total_time = 0;
    double total_time_sec = 0.0;

    pid = getpid();

    gettimeofday(&start, NULL);

    PLINE("RCV start.");
#ifdef WAIT
    PLINE("waitting");
    sleep(30);
#endif
    for(i = 0; ;){
        if(SUCCESS != SEM_TRY_WAIT(semPop)){
			continue;
		}

        i ++;


        SEM_POST(semPush);
    

        if(0 == i % 10000000){
            gettimeofday(&end, NULL);
            total_time = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
            total_time_sec = total_time/1000000.0;
            PLINE("trans 1000W signals in %.2f Secs (%d usecs).", total_time_sec, total_time);
            gettimeofday(&start, NULL);
        }

    }
}


int main(){
	size_t memSize = 1024 * 8;
	
	int pid = 0;
    
	int  shmId = 0;
	
    char *pmem = NULL;

    char * mem = shm_create(memSize, &shmId);
	if(NULL == mem){
		PLINE("alloc mem faild.");
	}

    pmem = mem;

    semPush = (Sem *)pmem;
    pmem += sizeof(Sem);

    semPop = (Sem *)pmem;
    pmem += sizeof(Sem);


    SEM_INIT(semPop, 1, 0);
    SEM_INIT(semPush, 1, 8);


    for(int i = 0; i < NSEND; i++){
		pid = fork();
		if(0 == pid) send();
    }
	
	for(int i = 0; i < NRCV; i++){
		pid = fork();
		if(0 == pid) recv();
	}

    while(true){
        sleep(1000);
    }

    
	shm_free(shmId);
	
	return 0;
}
