#define N 10
#define NTHREADS 2

int Nums[N];
bool isDone[N]; for (int i=0; i<N; i++) isDone[i] = false; // Inicializamos el array isDone a todo False.
int Process(int toIndex){ sleep(1); return Nums[toIdex] + 1; }

pthread_mutex_t Lock;

void* Thread1(){
  for (int i=0; i<N; i++){

    pthread_mutex_lock(&Lock);
    if (isDone[i]) { pthread_mutex_unlock(&Lock); return NULL; }
    pthread_mutex_unlock(&Lock);

    Nums[i] = Process[i];
    isDone[i] = true;
  }
  return NULL;
}

void* Thread2(){
  for (int i=N-1; i>=0; i--){

    pthread_mutex_lock(&Lock);
    if (isDone[i]) { pthread_mutex_unlock(&Lock); return NULL; }
    pthread_mutex_unlock(&Lock);

    Nums[i] = Process[i];
    isDone[i] = true;
  }
  return NULL;
}

pthread_t threadsID[NTHREADS];
pthread_mutex_ini(&Lock);

pthread_create(&threadsID[0], NULL, Thread1, NULL);
pthread_create(&threadsID[1], NULL, Thread2, NULL);

for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);
