#include <pthread.h>
#include <semaphore.h>

struct Task { int id, x0, y0; };
struct Thread_Info { int id; Array<Task> tasks; uint32_t* image; Render_Context* ctx; };

static sem_t semaphore;
static int current_task = -1;

Array<Thread_Info> create_thread_infos(uint32_t* image, Render_Context* ctx)
{
    Array<Thread_Info> thread_infos; thread_infos.allocate(THREAD_COUNT);
    int x_tiles = WIDTH / TILE_SIZE;
    int y_tiles = HEIGHT / TILE_SIZE;
    int tile_count = x_tiles * y_tiles;

    Array<Task> tasks; tasks.allocate(tile_count);
    for(int x = 0; x < tile_count; x++)
    {
        int x0 = TILE_SIZE * (x % x_tiles);
        int y0 = TILE_SIZE * (x / x_tiles);
        tasks[x] = Task{x, x0, y0};
    }
    for(int x = 0; x < THREAD_COUNT; x++)
    {
        thread_infos[x] = Thread_Info{x, tasks, image, ctx};
    }
    return thread_infos;
}
void destroy_thread_infos(Array<Thread_Info> thread_infos)
{
    thread_infos[0].tasks.deallocate();
    thread_infos.deallocate();
}

void* thread_main(void* args);
Array<pthread_t> launch_threads(Array<Thread_Info> thread_infos)
{
    Array<pthread_t> threads; threads.allocate(THREAD_COUNT);
    sem_init(&semaphore, 0, 1);
    for(int x = 0; x < THREAD_COUNT; x++)
    {
        pthread_create(&threads[x], nullptr, &thread_main, &thread_infos[x]);
    }
    return threads;
}
void wait_for_threads(Array<pthread_t> threads)
{
    for(int x = 0; x < THREAD_COUNT; x++)
    {
        pthread_join(threads[x], nullptr);
    }
}
void destroy_threads(Array<pthread_t> threads)
{
    sem_destroy(&semaphore);
    threads.deallocate();
}

void* thread_main(void* args)
{
    Thread_Info* info = (Thread_Info*)args;
    int thread_id = info->id;
    Array<Task> tasks = info->tasks;
    uint32_t* image = info->image;
    Render_Context* ctx = info->ctx;
    bool is_complete = false;
    int x0 =0, y0 =0;

    while(true)
    {
        sem_wait(&semaphore);
        current_task++;
        if(current_task > tasks.size -1) { is_complete = true; }
        if(!is_complete)
        {
            x0 = tasks[current_task].x0;
            y0 = tasks[current_task].y0;
        }
        sem_post(&semaphore);
        if(is_complete) { break; }

        for(int y = 0; y < TILE_SIZE; y++)
        {
            for(int x = 0; x < TILE_SIZE; x++)
            {
                int index = WIDTH * (y0 + y) + (x0 + x);
                image[index] = render_tile(x0+x, y0+y, ctx);
            }
        }
    }
    return nullptr;
}
