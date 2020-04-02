//
// Created by Alon zrihan on 2019-05-19.
//

#include "Semaphore.hpp"

Semaphore::Semaphore() {
    this->counter=0;
    pthread_cond_init(&this->c,NULL);
    pthread_mutex_init(&this->m,NULL);
}

Semaphore::Semaphore(unsigned val) {
    this->counter=val;
    pthread_cond_init(&this->c,NULL);
    pthread_mutex_init(&this->m,NULL);
}

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->c);
}

void Semaphore::down() {
    pthread_mutex_lock(&this->m);
    while(this->counter<=0){
        pthread_cond_wait(&this->c,&this->m);
    }
    this->counter--;
    pthread_mutex_unlock(&this->m);
}

void Semaphore::up() {
    pthread_mutex_lock(&this->m);
    this->counter++;
    pthread_cond_signal(&this->c);
    pthread_mutex_unlock(&this->m);

}