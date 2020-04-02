#ifndef __GAMERUN_H
#define __GAMERUN_H
#include "Headers.hpp"
#include "Thread.hpp"
#include "utils.hpp"
#include "PCQueue.hpp"
/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {
	// All here are derived from ARGV, the program's input parameters. 
	uint n_gen;
	uint n_thread;
	string filename;
	bool interactive_on; 
	bool print_on; 
};

struct tile_record {
	double tile_compute_time; // Compute time for the tile
	uint thread_id; // The thread responsible for the compute
	int tile_id;
};
/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/
class Game {
public:

	Game(game_params g);
	~Game();
	void run(); // Runs the game
	const vector<double> gen_hist() const{ return this->m_gen_hist;} // Returns the generation timing histogram
	const vector<tile_record> tile_hist() const{ return this->m_tile_hist;} // Returns the tile timing histogram
	uint thread_num() const{ return this->m_thread_num;} //Returns the effective number of running threads = min(thread_num, field_height)
	void creatBoardFromFile();
	void cleanNewBoard();
    void alive(int row,int col);
    int getRows(){ return this->rows;}
    int getCols(){ return this->cols;}
    PCQueue<Task>* getTaks(){ return this->tasks;}
    void incTask(){this->task_done++;}
    bool doneTasks(){ return (this->task_done==(int)this->m_thread_num);}
    pthread_cond_t* getCond(){ return &this->finish_step;}
    pthread_mutex_t* getIncLOck(){ return &this->inc_lock;}
    vector<tile_record>* getTileVector(){ return &this->m_tile_hist;}


protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game(); 
	void _step(uint curr_gen); 
	void _destroy_game(); 
	inline void print_board(const char* header);

	uint m_gen_num; 			 		// The number of generations to run
	uint m_thread_num; 			 		// Effective number of threads = min(thread_num, field_height)
	vector<tile_record> m_tile_hist; 	// Shared Timing history for tiles: First m_thread_num cells are the calculation durations for tiles in generation 1 and so on. 
							   	 		// Note: In your implementation, all m_thread_num threads must write to this structure. 
	vector<double> m_gen_hist;  	 	// Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool;	// A storage container for your threads. This acts as the threadpool.

	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT 
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)

	
	// TODO: Add in your variables and synchronization primitives
	string file_name;
	bool_mat* current_board;
	bool_mat* next_board;
	int rows,cols;
	PCQueue<Task>* tasks;
	int task_done;
	pthread_mutex_t m;
	pthread_cond_t finish_step;
    pthread_mutex_t inc_lock;

    friend class Tile;

};

class Tile:public Thread{
public:
	Tile(Game* g,uint thread_id);
	~Tile()= default;
	void thread_workload() override;

private:
	Game* g;

};

#endif
