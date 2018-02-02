#include <fstream>   /* ifstream */
#include <cstdio>    /* sscanf */
#include <iostream>  /* cout */
#include <string.h>  /* memset */
#include <utility>   /* pair */
#include <vector>    /* vector */
#include <pthread.h> /* thread stuff */
#include <semaphore.h> /* semaphores */

#define ALIVE 1
#define DEAD 0

#define FILE_BUFFSIZE 512

typedef unsigned int uint;

//-----// GRID //-------------------------------------------------------------//

class Grid
{
public:
  enum Buffer
  {
    BUFFER_FRONT,
    BUFFER_BACK
  };
public:
  static void CreateInstance(const char * fname);
  static Grid * GetInstance();
  static void DestroyInstance();
  void UpdateCell(int i);
  void SwapBuffers();
  char ValueAt(Buffer buffer_name, uint x, uint y) const;
  char & ValueAt(Buffer buffer_name, uint x, uint y);
  char ValueAt(Buffer buffer_name, uint i) const;
  char & ValueAt(Buffer buffer_name, uint i);
  void PrintBuffer(Buffer buffer_name) const;
  uint Size() const;
private:
  Grid(uint width, uint height);
  Grid(const char * fname);
  ~Grid();
  void Allocate();
  char * _frontBuffer;
  char * _backBuffer;
  int _width;
  int _height;
  int _maxCol;
  int _maxRow;
  uint _size;
private:
  // a pointer to the last grid created
  static Grid * _instance;
};

// static initializations
Grid * Grid::_instance = NULL;

void Grid::CreateInstance(const char * fname)
{
  if(!_instance)
    _instance = new Grid(fname);
}

Grid * Grid::GetInstance()
{
  return _instance;
}

void Grid::DestroyInstance()
{
  if(_instance)
  {
    delete _instance;
    _instance = NULL;
  }
}

void Grid::UpdateCell(int center)
{
  // TODO: Precompute indicies of neighbour cells and store them
  // calculating the delta that the center index needs to be
  // shifted inorder to be at a neighbouring cell. This is complicated
  // due to the fact that we are wrapping around the grid.
  int d_right, d_left, d_up, d_down;
  // calculating left / right index shifts
  int col = center % _width;
  if(col == 0)
  {
    d_right = 1;
    d_left = _maxCol;
  }
  else if(col ==  _maxCol)
  {
    d_right = -_maxCol;
    d_left = -1;
  }
  else
  {
    d_right = 1;
    d_left = -1;
  }
  // calculating up / down index shifts
  int row = center / _width;
  if(row == 0)
  {
    d_up = _maxRow * _width;
    d_down = _width;
  }
  else if(row == _maxRow)
  {
    d_up = -_width;
    d_down = -(_width * _maxRow);
  }
  else
  {
    d_up = -_width;
    d_down = _width;
  }
  // finding the values of neighbouring cells
  // Numbers below show what the indicies of neighbour_values
  // refers to in relation to the center index c.
  //*******/
  // 0 1 2
  // 3 c 4
  // 5 6 7
  //*******/
  char neighbour_values[8];
  neighbour_values[0] = _frontBuffer[center + d_left + d_up];
  neighbour_values[1] = _frontBuffer[center + d_up];
  neighbour_values[2] = _frontBuffer[center + d_right + d_up];
  neighbour_values[3] = _frontBuffer[center + d_left];
  neighbour_values[4] = _frontBuffer[center + d_right];
  neighbour_values[5] = _frontBuffer[center + d_left + d_down];
  neighbour_values[6] = _frontBuffer[center + d_down];
  neighbour_values[7] = _frontBuffer[center + d_right + d_down];
  // finding how many neighbours are alive
  uint alive_neighbours = 0;
  for(uint i = 0; i < 8; ++i)
  {
    if(neighbour_values[i])
      ++alive_neighbours;
  }
  // finding out if the cell is alive or dead depending on
  // its current state and the number of surrounding alive
  // neighbours
  if(_frontBuffer[center] == ALIVE)
  {
    if(alive_neighbours < 2 || alive_neighbours > 3)
      _backBuffer[center] = DEAD;
    else
      _backBuffer[center] = ALIVE;
  }
  else
  {
    if(alive_neighbours == 3)
      _backBuffer[center] = ALIVE;
    else
      _backBuffer[center] = DEAD;
  }
}

void Grid::SwapBuffers()
{
  char * temp = _frontBuffer;
  _frontBuffer = _backBuffer;
  _backBuffer = temp;
}

inline char Grid::ValueAt(Buffer buffer_name, uint x, uint y) const
{
  char * buffer;
  (buffer_name == BUFFER_FRONT) ? buffer = _frontBuffer : buffer = _backBuffer;
  uint index = y * _width + x;
  return buffer[index];
}

inline char & Grid::ValueAt(Buffer buffer_name, uint x, uint y)
{
  char * buffer;
  (buffer_name == BUFFER_FRONT) ? buffer = _frontBuffer : buffer = _backBuffer;
  uint index = y * _width + x;
  return buffer[index];
}

void Grid::PrintBuffer(Buffer buffer_name) const
{
  char * buffer;
  (buffer_name == BUFFER_FRONT) ? buffer = _frontBuffer : buffer = _backBuffer;
  for(uint i = 0; i < _size; ++i)
  {
    if(i % _width == 0 && i != 0)
      std::cout << std::endl;
    if (buffer[i] == 1)
      std::cout << "1";
    else
      std::cout << "0";
    std::cout << " ";
  }
  std::cout << std::endl;
}

uint Grid::Size() const
{
  return _size;
}

Grid::Grid(uint width, uint height) : _width(width), _height(height),
  _maxCol(width - 1), _maxRow(height - 1), _size(width * height)
{
  Allocate();
}

Grid::Grid(const char * fname)
{
  std::ifstream in_file;
  in_file.open(fname, std::ifstream::in);
  if(!in_file.is_open())
  {
    //TODO: THROW EXCEPTION HERE
  }
  in_file >> _width >> _height;
  _size = _width * _height;
  _maxCol = _width - 1;
  _maxRow = _height - 1;
  Allocate();
  while(!in_file.eof())
  {
    int x, y;
    in_file >> x >> y;
    ValueAt(BUFFER_FRONT, x, y) = 1;
  }
}

Grid::~Grid()
{
  delete [] _frontBuffer;
  delete [] _backBuffer;
}

void Grid::Allocate()
{
  _frontBuffer = new char[_size];
  _backBuffer = new char[_size];
  memset(_frontBuffer, 0, _size);
  memset(_backBuffer, 0, _size);
}

//-----// THREADS //----------------------------------------------------------//
// Contains a thread id and the range of indices that the
// thread is supposed to update. The thread will update
// the cells [_start, _end)

// A barrier type that can be used multiple times in order to control thread
// flow. The number of flow barriers must be known beforehand.

//-----// CANAL //------------------------------------------------------------//
// A canal is a way to make a group of threads (travelers) travel through a
// certain number of locks (barriers) before the reach the end of the canal.
// Each thread must be within a lock before they can move to the next.
// This will continue until all threads have been through all locks.
class Canal
{
public:
  // Create travelers when a traveler thread is created. This will synchronize
  // all threads so that they approach and wait at the same locks.
  class Traveler
  {
  public:
    Traveler();
  private:
    sem_t * _currentLock;
    uint * _threadsAtCurrentLock;
    friend Canal;
  };
public:
  static void Initialize(uint total_threads, uint total_locks);
  static void Lock(Traveler * traveler);
  static bool AllLocksPassed();
  static bool GateStop();
  static bool GatePass();
  static void Purge();
private:
  // semaphores for lock control
  static sem_t _mutex;
  static sem_t _lockA;
  static sem_t _lockB;
  // tracks the number of threads at each lock
  static uint _threadsAtLockA;
  static uint _threadsAtLockB;
  // totals for tracking when locks are full and when the whole
  // canal has been passed
  static uint _totalThreads;
  static uint _totalLocks;
  static uint _locksComplete;
};

// static initializations
sem_t Canal::_mutex;
sem_t Canal::_lockA;
sem_t Canal::_lockB;
uint Canal::_threadsAtLockA;
uint Canal::_threadsAtLockB;
uint Canal::_totalThreads;
uint Canal::_totalLocks;
uint Canal::_locksComplete;

void Canal::Initialize(uint total_threads, uint total_locks)
{
  // prep canal
  sem_init(&_mutex, 0, 1);
  sem_init(&_lockA, 0, 0);
  sem_init(&_lockB, 0, 0);
  _threadsAtLockA = 0;
  _threadsAtLockB = 0;
  _totalThreads = total_threads;
  _totalLocks = total_locks;
  _locksComplete = 0;
}

void Canal::Lock(Traveler * traveler)
{
  // traveler entering lock
  sem_wait(&_mutex);
  ++(*traveler->_threadsAtCurrentLock);
  // once all travelers are in the lock, travelers can progress to the next
  if(*traveler->_threadsAtCurrentLock == _totalThreads)
  {
    ++_locksComplete;
    // Realistically, this should be a generic function call that is performed
    // once all travelers are in the lock
    // start SNBH
    Grid::GetInstance()->SwapBuffers();
    // end SNBH
    sem_post(traveler->_currentLock);
  }
  sem_post(&_mutex);
  // traveler waits here until final traveler reaches lock
  sem_wait(traveler->_currentLock);
  sem_post(traveler->_currentLock);
  // traveler exits lock
  sem_wait(&_mutex);
  --(*traveler->_threadsAtCurrentLock);
  // once all travelers exit lock, reset lock semaphore to 0 so it can be
  // used as the next lock
  if(*traveler->_threadsAtCurrentLock == 0)
    sem_wait(traveler->_currentLock);
  sem_post(&_mutex);
  // set the traveler's next lock that they will wait at
  if(traveler->_currentLock == &_lockA)
  {
    traveler->_currentLock = &_lockB;
    traveler->_threadsAtCurrentLock = &_threadsAtLockB;
  }
  else
  {
    traveler->_currentLock = &_lockA;
    traveler->_threadsAtCurrentLock = &_threadsAtLockA;
  }
}

bool Canal::AllLocksPassed()
{
  if(_locksComplete < _totalLocks)
    return false;
  return true;
}

void Canal::Purge()
{

}

Canal::Traveler::Traveler()
{
  _currentLock = &Canal::_lockA;
  _threadsAtCurrentLock = &Canal::_threadsAtLockA;
}

//-----// GAME OF LIFE THREAD //----------------------------------------------//

struct GridRange
{
  GridRange(Grid * grid, uint start, uint end) :
    _grid(grid), _start(start), _end(end) {}
  Grid * _grid;
  uint _start, _end;
};


void * RunCell(void * range)
{
  GridRange * grid_range = reinterpret_cast<GridRange *>(range);
  // TODO: 1 WILL NEED TO BE REPLACED BY ITERATIONS REMAINING
  // YOU REALLY JUST NEED TO THINK ABOUT WHERE YOU ARE GOING TO PUT IT.
  Canal::Traveler traveler;
  while(!Canal::AllLocksPassed())
  {
    for(uint i = grid_range->_start; i < grid_range->_end; ++i)
    {
      grid_range->_grid->UpdateCell(i);
    }
    Canal::Lock(&traveler);
  }
}

//-----// GAME OF LIFE //-----------------------------------------------------//
void RunGameofLife(unsigned num_threads, const char * read_file,
  unsigned iterations, const char * write_file)
{
  // create grid
  Grid::CreateInstance(read_file);
  // intialize the canal
  Canal::Initialize(num_threads, iterations);
  // create space for thread ids and thread data
  std::vector<pthread_t> threads;
  std::vector<GridRange> ranges;
  threads.reserve(num_threads);
  ranges.reserve(num_threads);
  // Begin creating all needed threads
  uint threads_left = num_threads;
  uint cells_left = Grid::GetInstance()->Size();
  uint range_start = 0;
  while(threads_left)
  {
    // find the range the cell will manage
    uint num_cells = cells_left / threads_left;
    uint range_end = range_start + num_cells;
    ranges.push_back(GridRange(Grid::GetInstance(),range_start, range_end));
    // create the thread for that range
    pthread_t thread_id;
    int error = pthread_create(&thread_id, NULL, RunCell,
      (void *)&ranges.back());
    if(error)
    {
      // TODO: THROW ERROR ON THREAD CREATION FAIL
      std::cout << "Thread create error: " << std::endl;
    }
    threads.push_back(thread_id);
    /*std::cout << "Thread " << threads_left << " created. Range: ["
      << range_start << ", " << range_end << ")" << std::endl; */
    // end of previous range is the start of the next
    range_start = range_end;
    // decrease threads and cells left
    cells_left -= num_cells;
    --threads_left;
  }

  for(uint t = 0; t < num_threads; ++t)
  {
    void * return_value;
    int error = pthread_join(threads[t], &return_value);
    if(error)
    {
      // TODO: THROW ERROR ON THREAD JOIN FAIL
      std::cout << "Thread join error: " << threads[t] << std::endl;
    }
  }

  std::cout << "Front Buffer" << std::endl;
  Grid::GetInstance()->PrintBuffer(Grid::BUFFER_FRONT);
  Grid::DestroyInstance();
}

//-----// MAIN //-------------------------------------------------------------//
int main( int argc, char ** argv )
{
    if ( argc != 5 ) {
        std::cout << "<- Expected 4 parameters -> " << std::endl
          << "1. Number of threads to process with" << std::endl
          << "1. Initial population file to read" << std::endl
          << "2. Number of iterations" << std::endl
          << "3. Final population file to write to" << std::endl;
        return 1;
    }
    int threads = 1;
    char read_file[FILE_BUFFSIZE];
    int iterations = 0;
    char write_file[FILE_BUFFSIZE];
    std::sscanf(argv[1],"%i",&threads);
    std::sscanf(argv[2],"%s",read_file);
    std::sscanf(argv[3],"%i",&iterations);
    std::sscanf(argv[4],"%s",write_file);

    RunGameofLife(threads, read_file, iterations, write_file);


}
