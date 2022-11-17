#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define handle_error_en(rt, msg)    \
        do { errno = rt; perror("msg"); exit(EXIT_FAILURE); } while(0)


#define MAXSTRINGSZ     4096
extern char** environ;


static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static pthread_key_t env_key;
static pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

static void thread_init(void)
{
    pthread_key_create(&env_key, &free);
}


static char* getenv_r(const char* name)
{
    int i, len;
    char *envbuf = NULL;

    pthread_once(&once_control, &thread_init);
    pthread_mutex_lock(&env_mutex);
    envbuf = pthread_getspecific(env_key);
    if(envbuf == NULL) {
        envbuf  = calloc(MAXSTRINGSZ, 1);
        if(envbuf == NULL) {
            pthread_mutex_unlock(&env_mutex);
            return NULL;
        }
        pthread_setspecific(env_key, envbuf);
    }
    len = strlen(name);
    for(i = 0; environ[i] != NULL; i++) {
        if( strncmp(name, environ[i], len) == 0 &&
            ( environ[i][len] == '=') ) {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ -1);
        }
    }

    pthread_mutex_unlock(&env_mutex);
    return envbuf;
}


static pthread_barrier_t g_barrier;
static char* env_key_array[] = { "OLDPWD", "LOGNAME", "SHELL", "HOME"};

static void* tmain(void* arg)
{
    long index = (long)arg;
    printf("index %ld\n", index);
    char* name = env_key_array[index];
    char* value = getenv_r(name);
    sleep(index);
    puts("pthread_barrier_wait");
    pthread_barrier_wait(&g_barrier);
    printf(" index %ld --> %s=%s\n",index, name, value);
}


int main(int argc, char** argv)
{
    long i , rt;
    int num = sizeof(env_key_array) / sizeof(char*);
    pthread_barrier_init(&g_barrier,NULL, num);
    pthread_t tidarr[num];

    for(i = 0; i < num; ++i) {
        rt = pthread_create(&(tidarr[i]), NULL, &tmain, (void*)i);
        if(rt != 0)
            handle_error_en(rt, "pthread_create");
    }

    for(i =0; i < num; ++i) {
        pthread_join(tidarr[i], NULL);
    }

    pthread_barrier_destroy(&g_barrier);
    return 0;
}