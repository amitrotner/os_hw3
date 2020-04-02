#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
    PCQueue();
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	~PCQueue(){delete this->q;}
	T pop(); 

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item); 


private:
	// Add your class memebers here
	queue<T>* q;
	int producer_inside;
	pthread_cond_t consumer_allow;
	pthread_mutex_t global_lock;
	Semaphore s;
};

template <class T> // Add definition in PCQueue.hpp if you need this constructor
PCQueue<T>::PCQueue():producer_inside(0){
    this->q=new queue<T>();
    pthread_mutex_init(&this->global_lock,NULL);
    pthread_cond_init(&this->consumer_allow,NULL);
}

template <class T>
T PCQueue<T>::pop(){
    this->s.down();
    pthread_mutex_lock(&this->global_lock);
    while(producer_inside==1) {
        pthread_cond_wait(&this->consumer_allow, &this->global_lock);
    }
    T to_return=this->q->front();
    this->q->pop();
    pthread_mutex_unlock(&this->global_lock);
    return to_return;
}

template <class T>
void PCQueue<T>::push(const T& item){
    this->producer_inside=1;
    pthread_mutex_lock(&this->global_lock);
    this->q->push(item);
    this->s.up();
    this->producer_inside=0;
    pthread_cond_broadcast(&this->consumer_allow);
    pthread_mutex_unlock(&this->global_lock);

}
// Recommendation: Use the implementation of the std::queue for this exercise
#endif