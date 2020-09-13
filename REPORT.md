# ASSIGNMENT REPORT

### Overview
I have submitted a working program that correctly propagates transformed data from source to sink for about 25 sec on a machine with 8 CPU cores and 32 GB memory. I have added three new files: `pipeline.h`, `pipeline.cpp`, and `threadsafe_queue.h`. The files `pipeline.h/cpp` contains the implementation of Pipeline, which acts as temporary storage for the data. Data from source is written to the pipeline, is then read from the pipeline, transformed and then propagated to the sink. The file `threadsafe_queue.h` contains the basic implementation of Threadsafe queue.

The given architecture created two threads: `producer` and `consumer`. I have modified this architecture and instead created additional threads. I have introduced a temporary storage between producer and consumer that I call a pipeline. Since, speed at which data is produced is more than speed at which data is consumed, this pipeline acts as a buffer and saves the data until consumer is ready to read the data. The new flow works in the following way:

Data from producer thread is written to the pipeline and once that is done, `secret_channel` informs the producer that data is received. Data from the pipeline is read by the consumer thread. The consumer thread assigns section of read data to `transform` threads. Each `transform` thread reads its section of the data and transforms it from *spherical coordinates* to *cartesian*. Once all the `transform` threads have finished, the consumer thread propagates data to the sink and is ready to read next data.

### Pipeline
The pipeline internally creates two queues: `write` and `read`. `Write` queue stores pointers to buffers that are of fixed size to hold incoming data. When a request to write data comes, a buffer is taken from the `write` queue, and data is written to the buffer. Pipeline then moves this buffer to the `read` queue. Request to read data is served from the `read` queue. At the start, `write` queue holds all the buffers, whereas `read` queue is empty.

### Result
The program correctly propagates transformed data from source to sink for about 25 sec. The pipeline has 500 buffers and each buffer holds 25000 objects. Reducing the capacity of each buffer causes the program to end early because there are no free buffers to write data and producer thread is waiting on consumer thread to free buffer. Further increasing the capacity does not cause program to run longer as now the producer thread is unable to write all the data in permitted time even though free buffers are available.

### Process
I intially created two buffers, producer thread would write data to one buffer and consumer thread would read data from that buffer once producer thread is done. The buffers would resize based on the amount of input data. Once done, the threads would swap their buffers. Consumer thread would transform the data and propagate it to the sink. I noticed that producer thread waited long for consumer thread to finish. Majority of consumer thread time is spent in tranform. Hence, I added multiple `transform` threads to speed up the tranform. I also noticed that significant amount of time was spent in re-sizing the buffers to hold larger input data, hence I added change to create multiple fixed size buffers.

### Improvements
To get the program run infinitely, the consumer speed should match the producer speed. Hence, if producer speed increases, either you increase speed of consumption of data by adding more consumer threads, or add more storage to store data.

To allow the program to run longer, I have following suggestions:
1. Create multiple producer threads - At some point in time, writing data to pipeline will take more time than what is allowed, hence multiple producer threads would be required.
2. Increase the number of fixed size buffers or increase buffer capacity - With more available memory, producer thread would be able to continue to write longer.
3. Increase the number of transform threads - To speed up the consumption of the data, we can create extra `transform` threads. In this case we should make sure that size of buffer is big enough so that each `trasform` thread gets reasonable section of the buffer to transform.

### Other Design thoughts
My design created multiple `transform` threads. These threads transform section of data read by consumer thread from the buffer. Instead, I could change it such that consumer thread provides entire buffer data to a `transform` thread. In that case, when propagating data to the sink, we need to make sure that sequence of input data is maintained and `transform` thread holding data from first buffer writes first and so on.

I also notice that if we have enough memory, producer thread can also become a bottleneck where time needed to write data to the buffer may take longer than allowed time. In this case, we would need multiple producer threads each writing to different buffers. If we do that, we would also need to make sure that buffers added to the `read` queue by the pipeline are in order and data sequence is not lost.

If memory is limited, `Pipeline` can store the produced data on disk in multiple small files instead of using in-memory buffers. Consumer thread will read the data from these files. This design will also be fault-tolerant, as the produced data will be preserved even if the machine dies, and data will eventually be propagated to the sink.

My implementation of Threadsafe queue is basic. Every `push` and `pop` locks the entire queue. This implementation can be improved such that locking is not required when more than one element is present in the queue.

### Tools Used
I used `totalview` for debugging and added C++ clock timers in the code to determine time spent in various sections of the code.