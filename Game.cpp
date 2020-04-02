#include "Game.hpp"
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	bool_mat temp = (*this->current_board);
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();

		_step(i); // Iterates a single generation
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}
Game::Game(game_params g) {
	this->file_name=g.filename;
	this->m_gen_num=g.n_gen;
	this->m_thread_num=g.n_thread;
	this->interactive_on=g.interactive_on;
	this->print_on=g.print_on;
	this->task_done=0;
	this->tasks=new PCQueue<Task>();
	pthread_mutex_init(&this->inc_lock,NULL);
	pthread_mutex_init(&this->m,NULL);
	pthread_cond_init(&this->finish_step,NULL);
	this->current_board=new bool_mat();
	this->next_board=new bool_mat();
}
Game::~Game() {
    pthread_mutex_destroy(&this->inc_lock);
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->finish_step);
    for (int i = 0; i < (int)m_thread_num; ++i) {
        delete this->m_threadpool[i];
    }
    delete this->current_board;
    delete this->next_board;
    delete this->tasks;
}
void Game::_init_game() {
	// Create game fields - Consider using utils:read_file, utils::split
	// Create & Start threads
	// Testing of your implementation will presume all threads are started here
	creatBoardFromFile();
	if((int)this->m_thread_num>this->rows)
		this->m_thread_num=(uint)this->rows;
	this->m_tile_hist.resize(this->m_thread_num*this->m_gen_num);
	for (int i = 0; i < (int)this->m_thread_num; ++i) {
		Tile* to_add=new Tile(this,i);
		this->m_threadpool.push_back(to_add);
		this->m_threadpool[i]->start();
	}

}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	// Wait for the workers to finish calculating 
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade
	int diff=this->rows/this->m_thread_num;
	int index=0;
	for (int i = 0; i < ((int)this->m_thread_num*diff); i+=diff) {
		Task to_add;
		if(i==(((int)this->m_thread_num*diff)-diff))
			to_add.setTask(index,curr_gen*this->m_thread_num,this->rows,i,0);
		else
			to_add.setTask(index,curr_gen*this->m_thread_num,i+diff,i,0);
        index++;
		this->tasks->push(to_add);
	}

	pthread_mutex_lock(&this->m);
	if(this->task_done<(int)this->m_thread_num)
		pthread_cond_wait(&this->finish_step,&this->m);
	this->task_done=0;
	pthread_mutex_unlock(&this->m);

	bool_mat* temp=this->current_board;
	this->current_board=this->next_board;
	this->next_board=temp;
	//cleanNewBoard();
	//print_board("step");

}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
    for (int j = 0; j < (int)this->m_thread_num; ++j) {
        Task poison;
        poison.setTask(0,0,0,0,1);
        this->tasks->push(poison);
    }
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
    }
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
		
		// TODO: Print the board
		cout << u8"╔" << string(u8"═") * this->cols << u8"╗" << endl;
		for (int i = 0; i < this->rows; ++i) {
			cout << u8"║";
			for (int j = 0; j < (int)this->cols; ++j) {
				cout << ((*(this->current_board))[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * this->cols << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}

void Game::creatBoardFromFile() {
    using namespace utils;
    vector<string> rows=read_lines(this->file_name);
    vector<string> line=split(rows[0],' ');
	this->rows=rows.size();
	this->cols=line.size();
	vector<bool>to_add_current;
	vector<bool>to_add_next;
	for (int i = 0; i < this->rows; ++i) {
		(*(this->current_board)).push_back(to_add_current);
		(*(this->next_board)).push_back(to_add_next);
	}
	for (int i = 0; i < (int)rows.size(); ++i) {
		line=split(rows[i],' ');
		for (int j = 0; j < (int)line.size(); ++j) {
			(*(this->next_board))[i].push_back(false);
			if(line[j].compare("0"))
				(*(this->current_board))[i].push_back(true);
			else
				(*(this->current_board))[i].push_back(false);
		}
	}
}

void Game::cleanNewBoard() {
	for (int i = 0; i < this->rows; ++i) {
		for (int j = 0; j < this->cols; ++j) {
			(*(this->next_board))[i][j]=false;
		}
	}
}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/

void Game::alive(int row,int col){
	int neighbors=0;
	if(row>0 && (*(this->current_board))[row-1][col]==1)//up
		neighbors++;
	if(row<this->rows-1 && (*(this->current_board))[row+1][col])//down
		neighbors++;
	if(col>0 && (*(this->current_board))[row][col-1])//left
		neighbors++;
	if(col<this->cols-1 && (*(this->current_board))[row][col+1])//right
		neighbors++;
	if(row>0 && col>0 && (*(this->current_board))[row-1][col-1]==1)//up_left
		neighbors++;
	if(row<this->rows-1 && col>0 && (*(this->current_board))[row+1][col-1]==1)//down_left
		neighbors++;
	if(row>0 && col<this->cols-1 && (*(this->current_board))[row+-1][col+1]==1)//up_right
		neighbors++;
	if(row<this->rows-1 && col<this->cols-1 && (*(this->current_board))[row+1][col+1]==1)//down_right
		neighbors++;
	if((*(this->current_board))[row][col]==0&&neighbors==3) {
		(*(this->next_board))[row][col] = 1;
		return;
	}
	if((*(this->current_board))[row][col]==1&&(neighbors==2||neighbors==3)) {
		(*(this->next_board))[row][col] = 1;
		return;
	}
	(*(this->next_board))[row][col] = 0;
}

Tile::Tile(Game* g, uint thread_id){
	this->g=g;
	m_thread_id=thread_id;
}

void Tile::thread_workload() {
	while(1) {
		PCQueue<Task>* t_tasks=this->g->tasks;
		Task to_do=t_tasks->pop();
		if(to_do.flag)
            break;
        auto gen_start = std::chrono::system_clock::now();
		for (int i = to_do.lower; i < to_do.upper; ++i) {
			for (int j = 0; j < this->g->cols; ++j) {
				this->g->alive(i, j);
			}
		}
        auto gen_end = std::chrono::system_clock::now();
		vector<tile_record>* temp=&this->g->m_tile_hist;
        (*(temp))[to_do.offset+to_do.tile_id].tile_id=to_do.tile_id;
        (*(temp))[to_do.offset+to_do.tile_id].thread_id=this->thread_id();
        (*(temp))[to_do.offset+to_do.tile_id].tile_compute_time=((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		pthread_mutex_lock(&this->g->inc_lock);
		this->g->incTask();
		pthread_mutex_unlock(&this->g->inc_lock);
		if(this->g->doneTasks())
			pthread_cond_signal(&this->g->finish_step);
	}
}
