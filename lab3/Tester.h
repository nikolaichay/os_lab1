#include <iostream>
#include <pthread.h>
#include <thread>
#include <chrono>
#include "Queue.h" 

class Tester {
private:
    int NUM_THREADS; 
    int NUM_VALUES = 10; 

    Queue queue;
    pthread_t* writers;
    pthread_t* readers;
    int* testValues;

    struct WriterData {
        Tester* tester;
        std::atomic<double>* time;
    };

    static void* writer(void* arg) {
        Tester* tester = static_cast<WriterData*>(arg)->tester;
        std::atomic<double>* time = static_cast<WriterData*>(arg)->time;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i{}; i < tester->NUM_VALUES; ++i) {
            tester->queue.enqueue(tester->testValues[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        (*time).store((*time).load() + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

        return nullptr;
    }
    
    struct ReaderData {
        Tester* tester;
        std::atomic<double>* time;
        std::atomic<int>* cnts;
    };

    static void* reader(void* arg) {
        Tester* tester = static_cast<ReaderData*>(arg)->tester;
        std::atomic<double>* time = static_cast<ReaderData*>(arg)->time;
        std::atomic<int>* cnts = static_cast<ReaderData*>(arg)->cnts;

        auto start = std::chrono::high_resolution_clock::now();
        int value, cnt = 0;
        while (tester->queue.dequeue(value)) {
            cnts[value]++;
            cnt++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        (*time).store((*time).load() + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

        return nullptr;
    }

public:
    Tester(int valuesNumber = 10) {
        NUM_VALUES = valuesNumber;
        NUM_THREADS = std::thread::hardware_concurrency();
        writers = new pthread_t[NUM_THREADS];
        readers = new pthread_t[NUM_THREADS];

        testValues = new int[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; i++) {
            testValues[i] = i;
        }
    }

    bool testWrite(int threadsCnt = 0, double* time = nullptr) {
        if (threadsCnt == 0) {
            threadsCnt = NUM_THREADS;
        }

        std::atomic<double> t;
        WriterData* wd = new WriterData;
        wd->tester = this;
        wd->time = &t;

        for (int i = 0; i < threadsCnt; ++i)
            pthread_create(&writers[i], nullptr, writer, wd);

        for (int i = 0; i < threadsCnt; ++i)
            pthread_join(writers[i], nullptr);

        int cnts[NUM_VALUES]{};

        int num{};
        while (queue.dequeue(num)) {
            cnts[num]++;
        }

        for (int i = 0; i < NUM_VALUES; ++i) {
            if (cnts[i] != threadsCnt) {
                return false;
            }
        }

        if (time) {
            (*time) = t / threadsCnt / NUM_VALUES;
        }

        return true;
    }

    bool testRead(int threadsCnt = 0, double* time = nullptr) {
        if (threadsCnt == 0) {
            threadsCnt = NUM_THREADS;
        }

        for (int i = 0; i < NUM_VALUES; i++) 
            queue.enqueue(testValues[i]);

        std::atomic<int> cnts[NUM_VALUES]{};

        std::atomic<double> t;
        ReaderData* rd = new ReaderData;
        rd->tester = this;
        rd->time = &t;
        rd->cnts = cnts;

        for (int i = 0; i < threadsCnt; i++)
            pthread_create(&readers[i], nullptr, reader, rd);

        for (int i = 0; i < threadsCnt; i++)
            pthread_join(readers[i], nullptr);

        for (int i = 0; i < NUM_VALUES; i++) {
            if (cnts[i].load() != 1) {
                return false;
            }
        }

        if (time) {
            (*time) = t / threadsCnt / NUM_VALUES;
        }

        return true;
    }

    bool testCommonSpeed() {

        std::atomic<int> cnts[NUM_VALUES]{};
        std::atomic<double> t;

        for (int writersCnt = 1; writersCnt < NUM_THREADS; writersCnt++) {
            for (int readersCnt = 1; readersCnt < NUM_THREADS - writersCnt + 1; readersCnt++) {
                WriterData* wd = new WriterData;
                wd->tester = this;
                wd->time = &t;

                for (int i = 0; i < writersCnt; i++) {
                    int res = pthread_create(&writers[i], nullptr, writer, wd);
                    if (res) std::cout << "pthread_create(3) error for writer: " << res << std::endl;
                }

                ReaderData* rd = new ReaderData;
                rd->tester = this;
                rd->time = &t;
                rd->cnts = cnts;

                for (int i = 0; i < readersCnt; i++) {
                    int res = pthread_create(&readers[i], nullptr, reader, rd);
                    if (res) std::cout << "pthread_create(3) error for reader: " << res << std::endl;
                }

                for (int i = 0; i < writersCnt; i++)
                    pthread_join(writers[i], nullptr);

                for (int i = 0; i < readersCnt; i++)
                    pthread_join(readers[i], nullptr);

                for (int i = 0; i < NUM_VALUES; i++) {
                    if (cnts[i].load() != writersCnt) {
                        return false;
                    }
                    cnts[i].store(0);
                }

            }
        }

        return true;
    }

    bool testSpeed() {
        double writeTime = 0.0, readTime = 0.0, time = 0.0;

        for (int threads = 0; threads < NUM_THREADS; threads++) {
            if (!testWrite(threads, &time)) {
                return false;
            }
            writeTime += time;

            if (!testRead(threads, &time)) {
                return false;
            }
            readTime += time;
        }

        writeTime /= NUM_THREADS;
        readTime /= NUM_THREADS;

        std::cout << "Average write time:\t" << writeTime << std::endl;
        std::cout << "Average read time:\t" << readTime << std::endl;

        return true;
    }
};