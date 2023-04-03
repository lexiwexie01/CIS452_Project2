#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

/* Each baker will run as its own thread. Access to each resource will require a counting/binary
semaphore as appropriate. 
	So 1 thread per baker, 1 semaphore per resource for access
*/

// semaphores for resources
sem_t pantrySem;
sem_t fridgeSem;
sem_t mixerSem;
sem_t bowlSem;
sem_t spoonSem;
sem_t ovenSem;

// amount of each resource
int numMixers = 2, numPantries = 1, numFridges = 2, numBowls = 3, numSpoons = 5, numOvens = 1;
	
char* pantryIngredients[] = {"Flour", "Sugar", "Yeast", "Baking Soda", "Salt", "Cinnamon"};
char* fridgeIngredients[] = {"Eggs", "Milk", "Butter"};


int main() {
	pthread_t* bakerThreads
	int numBakers;
	int threadStatus;
	
	
	// get user input
	printf("Please enter the number of bakers to compete: \n");
	scanf("%d", &numBakers);
	
	// allocate memory
	bakerThreads = (pthread_t*) malloc(numBakers * sizeof(pthread_t));
	
	// we are using "i" as the identifier for our bakers/pthreads
	for (int i = 0; i < numBakers; i++) {
		threadStatus = pthread_create(&bakerThreads[i], NULL, &bake, (void*) i);
		
		if (threadStatus != 0) {
			printf("Thread create error: %d: %s", threadStatus, strerror(threadStatus));
			exit(1);
		}
	}
}

void* bake(void argument) {
	int bakerId = (int) argument;
	
	// baker does stuff
}
