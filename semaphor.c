#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BRIDGE_LENGTH 5
#define NUM_VEHICLES 10

sem_t mutex;
sem_t bridge;

int vehicles_on_bridge = 0;
int vehicles_waiting = 0;

struct Vehicle {
    int id;
    int direction;
};

void* vehicle(void* vehicle_data) {
    struct Vehicle* v = (struct Vehicle*)vehicle_data;

    while (1) {
        sem_wait(&mutex);
        if ((v->direction == 1 && vehicles_on_bridge == 0) || (v->direction == -1 && vehicles_on_bridge == 0)) {
            printf("Vehicle %d from direction %d is crossing the bridge.\n", v->id, v->direction);
            vehicles_on_bridge++;
            sem_post(&mutex);
            sem_wait(&bridge);
            usleep(1000000); // Simulate the time taken to cross the bridge
            sem_post(&bridge);
            printf("Vehicle %d from direction %d has crossed the bridge.\n", v->id, v->direction);
            sem_wait(&mutex);
            vehicles_on_bridge--;
        } else if (vehicles_waiting < BRIDGE_LENGTH) {
            printf("Vehicle %d from direction %d is waiting to cross the bridge.\n", v->id, v->direction);
            vehicles_waiting++;
            sem_post(&mutex);
            sem_wait(&bridge);
            usleep(1000000); // Simulate the time taken to cross the bridge
            sem_post(&bridge);
            printf("Vehicle %d from direction %d has crossed the bridge.\n", v->id, v->direction);
            sem_wait(&mutex);
            vehicles_waiting--;
        } else {
            sem_post(&mutex);
            usleep(1000000); // Simulate vehicles waiting
        }
    }
}

int main() {
    pthread_t vehicles[NUM_VEHICLES];
    struct Vehicle vehicle_data[NUM_VEHICLES];
    int directions[NUM_VEHICLES] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1}; // Example directions

    sem_init(&mutex, 0, 1);
    sem_init(&bridge, 0, 1);

    for (int i = 0; i < NUM_VEHICLES; i++) {
        vehicle_data[i].id = i + 1;
        vehicle_data[i].direction = directions[i];
        pthread_create(&vehicles[i], NULL, vehicle, &vehicle_data[i]);
    }

    for (int i = 0; i < NUM_VEHICLES; i++) {
        pthread_join(vehicles[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&bridge);

    return 0;
}
