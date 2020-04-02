#ifndef __THREAD_H
#define __THREAD_H
#include "Headers.hpp"
class Thread
{
public:
	Thread()= default;

	Thread(uint thread_id) 
	{
		// Only places thread_id
		this->m_thread_id=thread_id;
	} 
	virtual ~Thread() {} // Does nothing 

	// Returns true if the thread was successfully started, false if there was an error starting the thread 
	// Creates the internal thread via pthread_create 
	bool start()
	{
		return (pthread_create(&this->m_thread,NULL,this->entry_func,(void*)this)==0 ? true : false);
	}

	// Will not return until the internal thread has exited. 
	void join()
	{
          pthread_join(this->m_thread,NULL);
	}

	// Returns the user identifier
	uint thread_id()
	{
		return m_thread_id; 
	}
protected:
	// Implement this method in your subclass with the code you want your thread to run. 
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

class Task{
	int upper;
	int lower;
	int flag;
	int tile_id;
	int offset;
	friend class Tile;
public:
	Task():upper(0),lower(0){}
	~Task()= default;
	void setTask(int tile_id,int offset,int upper,int lower,int flag){
	    this->tile_id=tile_id;
	    this->offset=offset;
		this->upper=upper;
		this->lower=lower;
		this->flag=flag;
	}
	int getTileId(){ return tile_id;}
	int getUpper(){ return upper;}
	int getLower(){ return lower;}
	bool getFlag(){ return flag==1;}
	int getOffset() {return this->offset;}
};


#endif
