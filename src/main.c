#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <vector>

int N, M;
int** A;
int** res;
int R;
int K;

struct pthread_args {
    int start;
    int end;
};

int compare(const void * x1, const void * x2)   // функция сравнения элементов массива
{   
    return ( *(int*)x1 - *(int*)x2 );              
}

void* Median_filter(void* arg) {
    struct pthread_args* args = ((struct pthread_args*) arg);
    int start = args->start;
    int end = args->end;
    while (start != end) {
        int y = start / M;
        int x = start % M;

        int x1, x2, y1, y2;
        (x - R) >= 0 ? x1 = x - R: x1 = 0;
        (x + R) < M ? x2 = x + R: x2 = M - 1;
        (y - R) >= 0 ? y1 = y - R: y1 = 0;
        (y + R) < N ? y2 = y + R: y2 = N - 1;
        int k = (x2 - x1 + 1)*(y2 - y1 + 1);
        
        int med_list[k];
        
        int i = 0;
        for (int ex = x1; ex <= x2; ++ex)
        {
            for (int ey = y1; ey <= y2; ++ey)
            {
                med_list[i] = A[ey][ex];
                i+=1;
            } 
        }

        qsort(med_list, k, sizeof(int), compare);
    
        int median = med_list[k/2];
                
        res[y][x] = median;

        start += 1;
    }
    pthread_exit(0);
    return 0;
}

int main(int argc, char* argv[]) {
    int count_pthreads;
    std::cout << "N, M: ";
    std::cin >> N >> M;
    std::cout << "Num of threads: ";
    std::cin >> count_pthreads;
    if (count_pthreads > M*N) {
        std::cout << "Too many threads. Was set to " << N*M;
        count_pthreads = N*M;
    }
    std::cout << "K: ";
    std::cin >> K;
    std::cout << "R: ";
    std::cin >> R;
    
    A = new int*[N];
    res = new int*[N];
    for (int i = 0; i < N; ++i) {
        A[i] = new int[M];
        res[i] = new int[M];
    }

    std::cout << "Matrix: " << std::endl;
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < M; ++x) {
            std::cin >> A[y][x];
        }
    }

    int step = (M*N) / count_pthreads;

    pthread_t tid[count_pthreads];
    struct pthread_args *args_pth = (struct pthread_args*) malloc (count_pthreads * sizeof(struct pthread_args));

    for (int i = 0; i < count_pthreads; ++i) {
        if (i == count_pthreads - 1) {
            args_pth[i].start = i * step;
            args_pth[i].end = M*N;
        } else {
            args_pth[i].start = i * step;
            args_pth[i].end = (i + 1) * step;
        }
    }

    struct timespec start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int k = 0; k < K; k++) {

        for (int i = 0; i < count_pthreads; ++i) {
            pthread_create(&tid[i], NULL, Median_filter, (void*)&args_pth[i]);
        }

        for (int i = 0; i < count_pthreads; ++i)
        {
            pthread_join(tid[i], NULL);
        }
        
        for (int y = 0; y < N; ++y) {
            for (int x = 0; x < M; ++x) {
                A[y][x] = res[y][x];
        }
    }
    }

    clock_gettime(CLOCK_MONOTONIC, &stop);
    long long elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000000 + (stop.tv_nsec - start.tv_nsec);
    std::cout << "Threads and time: " << count_pthreads << ' ' << elapsed_time << std::endl << std::endl;


    std::cout << "Answer: " << std::endl;
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < M; ++x) {
            std::cout << res[y][x] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return 0;
}