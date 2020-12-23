#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h>  
#include <unistd.h>
#include <stdlib.h>

#define First_bowl 1
#define Second_bowl 2
#define non_blocking 1
#define blocking 0

typedef struct mysemaphore{
    pthread_mutex_t lockit;
    int value_of_semaphore;
    int type;
    pthread_cond_t for_signalling;
}mysemaphore;

int mysemaphore_initialize(mysemaphore *semaphore,int value_of_semaphore, int x){
    semaphore->value_of_semaphore=value_of_semaphore;
    if(x==0){
        semaphore->type=blocking;
    }
    else if(x==1){ 
        semaphore->type=non_blocking;
    }    
    pthread_mutex_init(&(semaphore->lockit),NULL);
    pthread_cond_init(&(semaphore->for_signalling),NULL);
    return 0;
}

int wait(mysemaphore *semaphore){
    //critical section
    pthread_mutex_lock(&(semaphore->lockit));
    if(semaphore->type==blocking){
        semaphore->value_of_semaphore--;
        if(semaphore->value_of_semaphore<0){
            //Blocking until it's value become non-negative
            pthread_cond_wait(&(semaphore->for_signalling),&(semaphore->lockit));
        }
        //release the lock
        pthread_mutex_unlock(&(semaphore->lockit));
        return 0;
    }
    if(semaphore->type==non_blocking){
        if(semaphore->value_of_semaphore==0){
            pthread_mutex_unlock(&(semaphore->lockit));
            return -1;
        }
        else{
            semaphore->value_of_semaphore--;
             pthread_mutex_unlock(&(semaphore->lockit));
            return 0;  
        }
    }
}

int signal_print(mysemaphore *sem){
    printf("%d",sem->value_of_semaphore);
    return sem->value_of_semaphore;
}

int signal(mysemaphore *semaphore){
    //critical section
    pthread_mutex_lock(&(semaphore->lockit));
    if(semaphore->type==non_blocking){
        semaphore->value_of_semaphore++;
        pthread_mutex_unlock(&(semaphore->lockit));
    }
    if(semaphore->type==blocking){
        semaphore->value_of_semaphore++;
        pthread_cond_signal(&(semaphore->for_signalling));
        pthread_mutex_unlock(&(semaphore->lockit));
    }
    return 0;
}
//List For philosophers
int *phil;

//List For Fork Mutex
mysemaphore *forks;

//List for bowls
mysemaphore *bowls;

//Avoid_Deadlock Semaphore
mysemaphore bowl;
int n;

//Which version user want to implement blocking or non-blocking
int version;
//Philosopher is Eating

void eating(int i, int fork1,int fork2,int bowl1, int bowl2){
    printf("The Philosopher %d which thread number %ld is now eating with fork %d, fork %d and both bowls .....Delicious\n",i+1,pthread_self(),fork1+1,fork2+1);
    sleep(2);
    return;
}

//Philosopher is Thinking
void thinking(int i){
    printf("The Philosopher %d which thread number %ld is now thinking\n",i+1,pthread_self());
    sleep(3);
    return;
}

//Philosopher are sitting on the table
void make_philosophers(){
    for(int i=0;i<n;i++){
        phil[i]=i;
    }
    return;
}

//Philosopher doing their work
void* go(void* index){
    int *i = index;
    do
    {
        thinking(*i);
        //Odds numbered philosopher first grab the left and then right fork
        if(*i%2==1){
            if(version==non_blocking){
                while (wait(&forks[*i])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&forks[*i]);
            }
            printf("The Philosopher %d grabs %d fork from the table\n",*i+1,*i+1);
            if(version==non_blocking){
                while (wait(&forks[((*i)+1)%n])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&forks[((*i)+1)%n]);
            }
            printf("The Philosopher %d grabs %d fork from the table\n",*i+1,((*i)+1)%n+1);
            if(version==non_blocking){
                while (wait(&bowls[0])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&bowls[0]);
            }
            printf("The Philosospher %d takes one bowl\n",*i+1);
            sleep(0.3);
            if(version==non_blocking){
                while (wait(&bowls[1])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&bowls[1]);
            }
            printf("The Philosospher %d takes another bowl\n",*i+1);
            eating(*i,*i,((*i)+1)%n,First_bowl,Second_bowl);
            signal(&bowls[0]);
            signal(&bowls[1]);
            signal(&forks[*i]);
            printf("The Philosopher %d puts %d fork on the table\n",*i+1,*i+1);
            signal(&forks[((*i)+1)%n]);
            printf("The Philosopher %d puts %d fork on the table\n",*i+1,((*i)+1)%n+1);
        }
        //Even numbered philosopher first grabs the right and then left fork
        else if(*i%2==0){
            if(version==non_blocking){
                while (wait(&forks[((*i)+1)%n])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&forks[((*i)+1)%n]);
            }
            printf("The Philosopher %d grabs %d fork from the table\n",*i+1,((*i)+1)%n+1);
            
            if(version==non_blocking){
                while (wait(&forks[*i])==-1)
                {
                    usleep(100);
                }
            }
            else{
                wait(&forks[*i]);
            }
            printf("The Philosopher %d grabs %d fork from the table\n",*i+1,*i+1);
            if(version==non_blocking){
                while (wait(&bowls[0])==-1)
                {
                    usleep(100);
                }
                    
            }
            else{
                wait(&bowls[0]);
            }
                printf("The Philosospher %d takes one bowl\n",*i+1);
            sleep(0.3);
            if(version==non_blocking){
                while (wait(&bowls[1])==-1)
                {
                    usleep(100);
                }
                
            }
            else{
                wait(&bowls[1]);
            }
            printf("The Philosospher %d takes another bowl\n",*i+1);
            eating(*i,*i,((*i)+1)%n,First_bowl,Second_bowl);
            signal(&bowls[0]);
            signal(&bowls[1]);
            signal(&forks[((*i)+1)%n]);
            printf("The Philosopher %d puts %d fork on the table\n",*i+1,((*i)+1)%n+1);
            signal(&forks[*i]);
            printf("The Philosopher %d puts %d fork on the table\n",*i+1,*i+1);
        }

    } while (1);
    
}

//Main Function
int main(){
    printf("Which version of semaphore do you want for solving the problem:\nEnter 0 for blocking and 1 for non-blocking: ");
    scanf("%d",&version);
    printf("Enter the number of philosopher you want: ");
    scanf("%d",&n);
    pthread_t philosopher[n];
    phil = (int*)malloc(n*sizeof(int));
    forks = (mysemaphore*)malloc(n*sizeof(mysemaphore));
    bowls = (mysemaphore*)malloc(2*sizeof(mysemaphore));
    make_philosophers();
    //Initialise the semaphores with 1 and all forks are available at the start
    for(int i = 0;i<n;i++){
        mysemaphore_initialize(&forks[i],1,version);
    }
    mysemaphore_initialize(&bowl,1,version);mysemaphore_initialize(&bowls[0],1,version);mysemaphore_initialize(&bowls[1],1,version);
    //Creating threads
    for(int i=0;i<n;i++){
        pthread_create(&philosopher[i],NULL,go,&phil[i]);
    }
    //Joining the threads so they do not terminate before program ends
    for(int i=0;i<n;i++){
        pthread_join(philosopher[i],NULL);
    }
}