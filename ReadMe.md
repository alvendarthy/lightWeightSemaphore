# What's this?

I'm working on a Realtime Application Cluster these days. We already have built an Architecture which heavily relies on the semaphore. Through some experiments, I found that when only a few processes are concurrent, 10 million semaphore notifications per second can be provided on a single-core computer. But when run the same demo a multi-core computer, or the number of concurrent processes increases, the performance quickly drops to one-tenth. 

After searching on the Internet for a long time, I found that there is no ready-made solution. So I transplanted a C++ version implementation of cameron 314 into C version. You may see his original implementation [here](https://github.com/cameron314/concurrentqueue/blob/master/lightweightsemaphore.h).

It turns out to works very well. In the case of less concurrency, it is at least 10% faster than the system semaphore. At high concurrency, it may be twice as fast as the system semaphore.

Please note these APIs are not compatible with sysV semaphores API, whether it is the meaning of the parameters or the return value. Please check *lightweightsemaphore.h* and *main.c* to get more details.

# How to

```shell
make
./main
```
