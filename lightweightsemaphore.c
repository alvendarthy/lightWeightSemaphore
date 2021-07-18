#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <sys/time.h>

#include "gcc_atom.h"
#include "lightweightsemaphore.h"

int lw_sem_init(lw_sem_t *sem, int pshared, unsigned int value, int maxSpin){
    assert(NULL != sem);
    assert(maxSpin > 0);

    sem->count = value;
    sem->maxSpin = maxSpin;

    return sem_init(&sem->sem, pshared, 0);
}

static bool smart_wait(lw_sem_t *sem, int wait_msecs){
    int ret = 0;
    struct timeval cur_time;
    struct timespec abs_timeout;
    int saveErr = 0;

    if(wait_msecs <= 0){
        sem_wait(&sem->sem);
        return true;
    }

    gettimeofday(&cur_time, NULL);

    abs_timeout.tv_sec = cur_time.tv_sec + wait_msecs / 1000;
    abs_timeout.tv_nsec = ((long)wait_msecs % 1000) * 1000000;

    for(;;)
    {
        ret = sem_timedwait(&sem->sem, &abs_timeout);

        if (ret == 0){
            return true;
        } 

        saveErr = errno;
        if(saveErr == EINTR)
        {
            continue;
        }
        
        if (saveErr == ETIMEDOUT)
        {
            return false;
        }

        // unexpected
        return false;
    }

    // unexpected
    return false;
}

static bool waitWithPartialSpinning(lw_sem_t *sem, int timeout_msecs){
    int spin = 0;
    lw_sem_count_t oldCount = 0;

    assert(NULL != sem);

    spin = sem->maxSpin;

    while (--spin >= 0)
    {
        oldCount = sem->count;
        if ((oldCount > 0) && (CAS_bool(&sem->count, oldCount, oldCount - 1))){
            return true;
        }
    }

    oldCount = AO_F_SUB(&sem->count, 1);

    if (oldCount > 0){
        return true;
    }

    if (smart_wait(sem, timeout_msecs)){
        return true;
    }

    while (true)
    {
        oldCount = sem->count;
        if (oldCount >= 0 && 0 == sem_trywait(&sem->sem)){
            return true;
        }

        if (oldCount < 0 && CAS_bool(&sem->count, oldCount, oldCount + 1)){
            return false;
        }
    }
}

bool lw_sem_try_wait(lw_sem_t *sem){
    int spin = 0;
    lw_sem_count_t oldCount = sem->count;

    spin = sem->maxSpin;

    while (spin-- > 0)
    {
        oldCount = sem->count;

        if(oldCount <= 0){
            return false;
        }

        if(CAS_bool(&sem->count, oldCount, oldCount - 1)){
            return true;
        }
    }
}

bool lw_sem_wait(lw_sem_t *sem){
    return lw_sem_try_wait(sem) || waitWithPartialSpinning(sem, 0);
}

bool lw_sem_time_wait(lw_sem_t *sem, int timeout_msecs){
    return lw_sem_try_wait(sem) || waitWithPartialSpinning(sem, timeout_msecs);
}

void lw_sem_signal(lw_sem_t *sem){
    lw_sem_count_t count = 1;
    lw_sem_count_t oldCount = AO_F_ADD(&sem->count, 1);
    lw_sem_count_t toRelease = -oldCount < count ? -oldCount : count;
    if (toRelease > 0)
    {
        sem_post(&sem->sem);
    }
}
