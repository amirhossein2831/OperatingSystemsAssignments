#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main (int argc, char *argv[])
{
    int segment_id;
    char* shared_memory;
    int shared_segment_size = 100;
    key_t key = 9564;
    struct shmid_ds shmbuffer;
    int segment_size;

    segment_id = shmget(key, shared_segment_size, 0666|IPC_CREAT);
    shared_memory = (char*) shmat (segment_id, NULL, 0);
    printf("shared memory attached at address %p\n", shared_memory);


    char *shm;
    if((shm = (char*)shmat(segment_id, NULL, 0)) == (char *) -1) {
    	perror("Error");
    }
    else {
    	printf("Read from shared memory: %s\n", shm);
    }
    
    shmdt(shared_memory);
    shmctl(segment_id,IPC_RMID,NULL);
    
    return 0;
}
