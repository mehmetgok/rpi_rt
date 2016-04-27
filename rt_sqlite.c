#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>

/* Ayrý bir thread oluþturacaðýz */
#include <pthread.h>

#include <signal.h> /* Ctrl+C Handling */


#include <sys/time.h>
#include <unistd.h>

#include <sqlite3.h>

 #include <stdint.h>

#define TABLE "CREATE TABLE IF NOT EXISTS TTC (id INTEGER PRIMARY KEY AUTOINCREMENT, millis BIGINT)"

static volatile int keepRunning = 1;


pthread_t th1;

void readFTDI();


void intrHandler(int dummy) {
    keepRunning = 0;
}


#define MY_PRIORITY (49) /* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

void stack_prefault(void) {

        unsigned char dummy[MAX_SAFE_STACK];

        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}

int main(int argc, char* argv[])
{
       
	/* Create thread */	
	
	signal(SIGINT, intrHandler);
	
	pthread_create(&th1, NULL, (void *) &readFTDI, NULL);
	
	
	// Wait for thread ending
	pthread_join( th1, NULL);

     

      
}
   


void readFTDI()
{
	struct timespec t;
    struct sched_param param;
    int interval = 5000000 ; /* 5000000 ns */
	 
	 
	struct timeval start, end;
	
	 uint64_t millis;

   long mtime, seconds;

   double useconds = 0.0;   
   
   int id = 0;
   int i;
   sqlite3 *db;
   sqlite3_stmt * stmt;
   
   char * sErrMsg = 0;
	
	
   char *zErrMsg = 0;
   int rc;
	 
	 
	 
	 
	    /* Declare ourself as a real time task */

        param.sched_priority = MY_PRIORITY;
        if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                perror("sched_setscheduler failed");
                exit(-1);
        }

        /* Lock memory */

        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                perror("mlockall failed");
                exit(-2);
        }

        /* Pre-fault our stack */

        stack_prefault();

       
		
		
		
		 rc = sqlite3_open("test.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
	  
	  sqlite3_exec(db, TABLE, NULL, NULL, &sErrMsg);  
	  
	  // Free table
	   sqlite3_exec(db, "delete from TTC", NULL, NULL, &sErrMsg);  
	  
	  
   }
   
   sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
   
   char buffer[] = "INSERT INTO TTC(millis) VALUES (?1)";
	
	sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL);
		
		 clock_gettime(CLOCK_MONOTONIC ,&t);
        /* start after one second */
        t.tv_sec++;
		
		
		 while( keepRunning ) 
		{
            /* wait until next shot */
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

            /* do the stuff */
		    gettimeofday(&start, NULL);
			
		   // useconds =  start.tv_sec/1.0 + start.tv_usec/(1000000.0);
		   
		 //  millis = (start.tv_sec * (uint64_t)1000) + (start.tv_usec / 1000);
		 
		 millis = (start.tv_sec * (uint64_t)1000000) + (start.tv_usec);
		
		
		// sqlite3_bind_int(stmt, 1, ++id);
		 sqlite3_bind_int64(stmt, 1, millis);
 
		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			printf("Commit Failed!\n");
		}
 
		sqlite3_reset(stmt);
			
				
				
				
			
				

                /* calculate next shot */
                t.tv_nsec += interval;

                while (t.tv_nsec >= NSEC_PER_SEC) 
				{
                       t.tv_nsec -= NSEC_PER_SEC;
                        t.tv_sec++;
                }
		}
		
		
		  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);  
   
   sqlite3_finalize(stmt);
   
   printf("Write OK\r\n");
   
   
   sqlite3_close(db);
	 
	
		
}

