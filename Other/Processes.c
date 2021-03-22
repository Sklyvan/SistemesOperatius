#define NSTRUCTS 100;
#define NPROCESSES 2;

type_def struct num_struct { int n; bool isDone; } NumStruct;
int Process(int n){ sleep(1); return n + 1; }

int FD = open("Nums.dat", O_RDWR, 0600);
NumStruct auxStruct1; NumStruct auxStruct2;

for (int i=0; i<NPROCESSES; i++){
  int ProcessID = fork();
  if (ProcessID == 0 && i == 0){ // Child 1 Code
    for (int i=0; i<NSTRUCTS; i++){
      lseek(FD, i*sizeof(NumStruct), SEEK_SET);
      read(FD, &auxStruct1, sizeof(NumStruct));
      if (&auxStruct1->isDone) break;
      auxStruct1->n = Process(auxStruct1->n);
      auxStruct1->isDone = true;
      write(FD, &auxStruct1, sizeof(NumStruct));
    }
    exit(0);
  }
  else if (ProcessID == 0 && i == 1){ // Child 2 Code
    for (int i=NSTRUCTS-1; i>=0; i--){
      lseek(FD, i*sizeof(NumStruct), SEEK_SET);
      read(FD, &auxStruct2, sizeof(NumStruct));
      if (&auxStruct1->isDone) break;
      auxStruct2->n = Process(auxStruct2->n);
      auxStruct2->isDone = true;
      write(FD, &auxStruct2, sizeof(NumStruct));
    }
    exit(0);
  }
}

// Father Code
while (wait(NULL) > 0);
