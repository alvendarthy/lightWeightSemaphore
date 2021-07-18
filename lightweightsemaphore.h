#ifndef LIGHTWEIGHTSEMAPHORE_H
#define LIGHTWEIGHTSEMAPHORE_H

#include <semaphore.h>
#include "macro_utils.h"

typedef ssize_t lw_sem_count_t;

typedef struct lw_sem_s{
    volatile lw_sem_count_t count;
    int maxSpin;
    sem_t sem;
}lw_sem_t;

int lw_sem_init(lw_sem_t *sem, int pshared, unsigned int value, int maxSpin);

bool lw_sem_try_wait(lw_sem_t *sem);

bool lw_sem_wait(lw_sem_t *sem);

bool lw_sem_time_wait(lw_sem_t *sem, int timeout_usecs);

void lw_sem_signal(lw_sem_t *sem);

#endif
