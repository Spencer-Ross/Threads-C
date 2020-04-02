/***********************************************************************
name: Spencer Ross
	assignment6 -- Five Dining Philosophers
description:	
	Five philosophers dine at a table. Watch as they fight to the death over
	the only chopsticks and win the chance to sit at the table to eat
	what might be their last meal. Who will win this savage feast?
	This program implements the Five Dining Philosophers problem using posix
	threads.
note:
	I left in the grabbing and putting comments to provide clarity on what is
	happening and to show that this program works as intended. Without them,
	a few print statesments made it appear as those some philosophers were
	eating at the same time as their adjacent stick-mates.
***********************************************************************/

/* Includes and definitions */
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define NUM_PHILOS 	5	//number of philosophers/chopsticks
#define MEALS		100	//number of cycles
#define T_MEAN		11	//think µ for randomGaussian
#define T_STD		7	//think σ for randomGaussian
#define E_MEAN		9	//eat σ for randomGaussian
#define E_STD		3	//eat µ for randomGaussian

#define THINK 	't'
#define EAT 	'e'
#define REACH 	'r'
#define PUT 	'p'

/* Text Colors (The spice of life)*/
#define KNRM	"\x1B[0m"
#define KBLK	"\x1B[30m"
#define KRED	"\x1B[31m"
#define KGRN	"\x1B[32m"
#define KYEL	"\x1B[33m"
#define KBLU	"\x1B[34m"
#define KMAG	"\x1B[35m"
#define KCYN	"\x1B[36m"
#define KWHT	"\x1B[37m"

//Prototypes
int randomGaussian(int mean, int stddev); //mean = µ, stddev = σ
int getWait(int a, int b);
void printAction(char action, int n, int a, int b);
static void* philosopher(void* arg);

//Globals
static pthread_t philos[NUM_PHILOS];
static pthread_mutex_t chopsticks[NUM_PHILOS];

//Create threads (philosophers), mutexes (chopsticks)
//Main ends threads and destroys mutexes at end
int main(int argc, char *argv[]){
    srand(time(NULL));
    int ret, index[NUM_PHILOS];

    for(int i = 0; i < NUM_PHILOS; i++) {
    	ret = pthread_mutex_init(&chopsticks[i], NULL);
    	if(ret != 0) {
			perror("main: pthread_mutex_init");
			exit(1);
		}
		index[i] = i + 1; //fill philosopher index
    }
    for(int i = 0; i < NUM_PHILOS; i++) {
    	ret = pthread_create(&philos[i], NULL, philosopher, &index[i]);
    	if(ret != 0) {
			perror("main: pthread_create");
			exit(1);
		}
    }
    for (int i = 0; i < NUM_PHILOS; i++) {
    	ret = pthread_join(philos[i], NULL);
    	if(ret != 0) {
			perror("main: pthread_join");
			exit(1);
		}
    }
    for(int i = 0; i < NUM_PHILOS; i++) {
    	ret = pthread_mutex_destroy(&chopsticks[i]);
    	if(ret != 0) {
			perror("main: pthread_mutex_destroy");
			exit(1);
		}
    }
	
    return 0;
}

static void* philosopher(void* arg) {
	int n = *(int *) arg;
	int checkL, checkR, left, right, eat = 0, wait = 0, think = 0;
	left  = n;
	right = (n + 1) % NUM_PHILOS;
	right = (right != 0) ? right : NUM_PHILOS;	/**	If the right stick is divisible by max
													number, it sets it to the max number.**/

	for(int i = 0; eat < MEALS; i++) {
		//THINK
		wait = getWait(T_MEAN, T_STD); 
		think += wait;
		printAction(THINK, n, wait, think);
		fflush(stdout); 
		sleep(wait);
		//PICKUP CHOPSTICKS
		printAction(REACH, n, left, right);
		checkL = pthread_mutex_lock(&chopsticks[n - 1]);
		checkR = pthread_mutex_lock(&chopsticks[n % NUM_PHILOS]);
		if((checkL != 0) || (checkR != 0)) {
				perror("philosopher: pthread_mutex_lock");
				exit(1);
		}
		//EAT
		wait = getWait(E_MEAN, E_STD);
		eat += wait;
		printAction(EAT, n, wait, eat);
		fflush(stdout);
		sleep(wait);
		//PUTDOWN CHOPSTICKS
		pthread_mutex_unlock(&chopsticks[n - 1]);
		pthread_mutex_unlock(&chopsticks[n % NUM_PHILOS]);
		if((checkL != 0) || (checkR != 0)) {
				perror("philosopher: pthread_mutex_unlock");
				exit(1);
		}
		printAction(PUT, n, left, right);
	}
	printAction('c', n, left, right); //feed dummy inputs
	return NULL;
}

//This is a simple function to get the random wait value or 
//if the value is negative, then wait is set to 0
int getWait(int a, int b){
	int ranNum = randomGaussian(a, b);
	int ret = (ranNum > 0) ? ranNum : 0;
	return ret;
}

//This takes in an option and data and prints messages 
//about what the philosophers are doing
void printAction(char action, int n, int a, int b) {
	switch(action) {
		case 't':
			printf(KWHT "Philosopher %d is thinking for %d seconds (%d)\n" KNRM, n, a, b);
			break;
		case 'e':
			printf(KGRN "Philosopher %d is eating for %d seconds (%d)\n" KNRM, n, a, b);
			break;
		case 'r':
			printf(KBLK "Philosopher %d reaches for left: %d, right: %d\n" KNRM, n, a, b);
			break;
		case 'p':
			printf(KRED "Philosopher %d puts back left back: %d, right: %d\n" KNRM, n, a, b);
			break;
		default:
			printf (KNRM "Philosopher %d finished eating\n", n);
			break;
	}
	return;
}

/* successive calls to randomGaussian produce integer return values	*/
/* having a gaussian distribution with the given mean and standard	*/
/* deviation.  Return values may be negative.   - Ben McCamish		*/
//Ben McCamish's random number function - ask him about it ;) or see comment above
int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}
