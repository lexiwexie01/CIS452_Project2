#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <memory.h>
#include <stdbool.h>

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
	
char* ingredients[] = {"Flour", "Sugar", "Yeast", "Baking Soda", "Salt", "Cinnamon", "Eggs", "Milk", "Butter"};

struct Recipe {
    char* recipeName;
    int* requiredIngredients;
    int numOfIngredients;
};

struct Recipe cookies = {
        "Cookies",
        (int[]){0, 1, 7, 8},
        4
};

struct Recipe pancakes = {
        "Pancakes",
        (int[]){0, 1, 3, 4, 6, 7, 8},
        7
};

struct Recipe pizza_dough = {
        "Homemade pizza dough",
        (int[]){2, 3, 4},
        3
};

struct Recipe soft_pretzels = {
        "Soft Pretzels",
        (int[]){0, 1, 2, 3, 4, 6},
        6
};

struct Recipe cinnamon_rolls = {
        "Cinnamon rolls",
        (int[]){0, 1, 4, 5, 6},
        5
};


void* bake(void* argument);

void getIngredient(int id);

void prepareRecipe();

void bakeRecipe();

int main() {
	pthread_t* bakerThreads;
	int numBakers;
	int threadStatus;

	// initialize the semaphores
	sem_init(&pantrySem, 0, numPantries);
	sem_init(&fridgeSem, 0, numFridges);
	sem_init(&mixerSem, 0, numMixers);
	sem_init(&bowlSem, 0, numBowls);
	sem_init(&spoonSem, 0, numSpoons);
	sem_init(&ovenSem, 0, numOvens);

	// get user input
	printf("Please enter the number of bakers to compete: ");
	scanf("%d", &numBakers);
	
	// allocate memory
	bakerThreads = (pthread_t*) malloc(numBakers * sizeof(pthread_t));
	
	// we are using "i" as the identifier for our bakers/pthreads
	for (int i = 0; i < numBakers; i++) {

		threadStatus = pthread_create(&bakerThreads[i], NULL, &bake, (void*)(intptr_t)i);
		
		if (threadStatus != 0) {
			printf("Thread create error: %d: %s\n", threadStatus, strerror(threadStatus));
			exit(1);
		}
	}

	for (int i = 0; i < numBakers; i++) {
	    threadStatus = pthread_join(bakerThreads[i], NULL);

	    if (threadStatus != 0) {
	        printf("Thread join error: %d: %s\n", threadStatus, strerror(threadStatus));
	        exit(1);
	    }
	}

	sem_destroy(&pantrySem);
	sem_destroy(&fridgeSem);
	sem_destroy(&mixerSem);
	sem_destroy(&bowlSem);
	sem_destroy(&spoonSem);
	sem_destroy(&ovenSem);

	free(bakerThreads);

	return 0;
}

void* bake(void* arg) {
//    int bakerId = *(int*) argument;
    int bakerId = (int)(intptr_t)arg;
    struct Recipe bakingList[] = {cookies, pancakes, pizza_dough, soft_pretzels, cinnamon_rolls};
    for (int recipeIndex = 0; recipeIndex < 5; recipeIndex++) {
        printf("[%d] started baking %s\n", bakerId, bakingList[recipeIndex].recipeName);
        // baker gets ingredients for the recipe
        for (int i = 0; i < bakingList[recipeIndex].numOfIngredients; i++) {
            // Retrieve the ingredient
            getIngredient(bakingList[recipeIndex].requiredIngredients[i]);
            printf("[%d] grabbed %s\n", bakerId, ingredients[i]);
        }
        // baker prepares the recipe
        prepareRecipe();

        // baker bakes recipe
        bakeRecipe();

        printf("[%d] finished baking %s\n", bakerId, bakingList[recipeIndex].recipeName);
    }
    printf("[%d] is done!\n", bakerId);
    return NULL;
}

void prepareRecipe() {
    // Retrieve tools
    sem_wait(&bowlSem);
    sem_wait(&spoonSem);
    sem_wait(&mixerSem);

    // Mix ingredients
    sleep(1);

    // Return tools
    sem_post(&bowlSem);
    sem_post(&spoonSem);
    sem_post(&mixerSem);
}

void bakeRecipe() {
    // Bake in oven
    sem_wait(&ovenSem);
    sleep(1);

    // Release the oven
    sem_post(&ovenSem);
}

void getIngredient(int id) {
    if (id <= 5) {
        // Pantry ingredient
        // pantry semaphore call
        sem_wait(&pantrySem);
        // pantry release
        sem_post(&pantrySem);
    } else {
        // Refrigerator ingredient
        // Refrigerator semaphore call
        sem_wait(&fridgeSem);
        // Refrigerator semaphore release
        sem_post(&fridgeSem);
    }
}