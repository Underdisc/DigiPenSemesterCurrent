/*****************************************************************************/
/*!
\file main_part3.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par DigiPen login: connor.deakin
\par Course: CS 355
\par Assignment: 1 Game of Life
\date 04/02/2018
\brief
  Contains the implementation of the third part of the assignemt. Allows user
  to choose the number of threads. These threads will pull data from a shared
  stack in order to find out which cell they need to update.
*/
/*****************************************************************************/

#include <fstream>     /* ifstream */
#include <cstdio>      /* sscanf */
#include <iostream>    /* cout */
#include <stdexcept>   /* runtime_error */
#include <string>      /* string */
#include <string.h>    /* memset */
#include <utility>     /* pair */
#include <vector>      /* vector */
#include <stack>       /* stack */
#include <pthread.h>   /* threads */
#include <semaphore.h> /* semaphores */

// Value of alive and dead cell
#define ALIVE 1
#define DEAD 0
#define NUM_NEIGHBOURS 8

// read file buffer size
#define FILE_BUFFSIZE 512
#define NUMBER_BUFFSIZE 10

typedef unsigned int uint;

//-----// GRID //-------------------------------------------------------------//

// A singleton class used for managing and updating cells within a game of life
// grid
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
  void PrintBuffer(Buffer buffer_name) const;
  void WriteFrontBuffer(const char * fname) const;
  uint Size() const;
private:
  Grid(uint width, uint height);
  Grid(const char * fname);
  ~Grid();
  void Allocate();
  char * _frontBuffer;
  char * _backBuffer;
  std::vector<int *> _cellNeighbours;
  int _width;
  int _height;
  int _maxCol;
  int _maxRow;
  uint _size;
private:
  static Grid * _instance;
};

// static initializations
Grid * Grid::_instance = NULL;

// singleton instance functions
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
  int * neighbours = _cellNeighbours[center];
  char neighbour_values[NUM_NEIGHBOURS];
  for(uint i = 0; i < NUM_NEIGHBOURS; ++i)
    neighbour_values[i] = _frontBuffer[neighbours[i]];
  // finding how many neighbours are alive
  uint alive_neighbours = 0;
  for(uint i = 0; i < NUM_NEIGHBOURS; ++i)
  {
    if(neighbour_values[i])
      ++alive_neighbours;
  }
  // find out if the cell should be alive or dead in the next state
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

// swap the front and back buffer so next step can be calculated
void Grid::SwapBuffers()
{
  char * temp = _frontBuffer;
  _frontBuffer = _backBuffer;
  _backBuffer = temp;
}

// get the value at an (x, y) coordinate
// TODO: Create char * array so these can be accessed without math opaerations
// only used for file io currently
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

// print the buffer to console
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

// Write buffer to file
void Grid::WriteFrontBuffer(const char * fname) const
{
  std::ofstream out_file;
  out_file.open(fname, std::ofstream::out);
  if(!out_file.is_open())
  {
    std::string error("Failed to open: ");
    error.append(fname);
    throw std::runtime_error(error);
  }
  for ( int i=0; i<_width; ++i )
  {
    for ( int j=0; j<_height; ++j )
    {
      out_file << (ValueAt(BUFFER_FRONT, i, j)%2?"#":".") << " ";
    }
    out_file << std::endl;
  }
  out_file.close();
}

uint Grid::Size() const
{
  return _size;
}

// Grid constructors / destructors
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
    std::string error("Failed to open file for writing: ");
    error.append(fname);
    throw std::runtime_error(error);
  }
  // reading in widthf and height
  in_file >> _width >> _height;
  _size = _width * _height;
  _maxCol = _width - 1;
  _maxRow = _height - 1;
  Allocate();
  // reading in which indicies start out as alive
  while(!in_file.eof())
  {
    int x, y;
    in_file >> x >> y;
    ValueAt(BUFFER_FRONT, x, y) = 1;
  }
}

// free buffers
Grid::~Grid()
{
  delete [] _frontBuffer;
  delete [] _backBuffer;
  for(uint i = 0; i < _cellNeighbours.size(); ++i)
    delete [] _cellNeighbours[i];
}

// allocate buffers
void Grid::Allocate()
{
  _frontBuffer = new char[_size];
  _backBuffer = new char[_size];
  memset(_frontBuffer, 0, _size);
  memset(_backBuffer, 0, _size);
  // finding the indicies of neighbors for each cell
  for(int center = 0; center < static_cast<int>(_size); ++center)
  {
    // finding deltas for each direction
    int d_right, d_left, d_up, d_down;
    // calculating left / right index shifts
    int col = center % _width;
    if(col == 0)
    {
      // wraps on the left
      d_right = 1;
      d_left = _maxCol;
    }
    else if(col ==  _maxCol)
    {
      // wraps on the right
      d_right = -_maxCol;
      d_left = -1;
    }
    else
    {
      // no left / right wrapping
      d_right = 1;
      d_left = -1;
    }
    // calculating up / down index shifts
    int row = center / _width;
    if(row == 0)
    {
      // wraps above
      d_up = _maxRow * _width;
      d_down = _width;
    }
    else if(row == _maxRow)
    {
      // wraps below
      d_up = -_width;
      d_down = -(_width * _maxRow);
    }
    else
    {
      // no wrapping
      d_up = -_width;
      d_down = _width;
    }
    // finding indicies of cells around center
    // grid below shows which the indicies of neighbor_values
    // refers to in relation to the center index c.
    //*******/
    // 0 1 2
    // 3 c 4
    // 5 6 7
    //*******/
    int * neighbouring_cells = new int[NUM_NEIGHBOURS];
    neighbouring_cells[0] = center + d_left + d_up;
    neighbouring_cells[1] = center + d_up;
    neighbouring_cells[2] = center + d_right + d_up;
    neighbouring_cells[3] = center + d_left;
    neighbouring_cells[4] = center + d_right;
    neighbouring_cells[5] = center + d_left + d_down;
    neighbouring_cells[6] = center + d_down;
    neighbouring_cells[7] = center + d_right + d_down;
    _cellNeighbours.push_back(neighbouring_cells);
  }
}

//-----// GAME OF LIFE THREAD //----------------------------------------------//

struct ThreadData
{
  std::stack<uint> _toUpdate;
  uint _totalIterations;
  uint _iterationsComplete;
  sem_t _mutex;
};

void FillCellStack(std::stack<uint> * cells_to_update)
{
  uint num_cells = Grid::GetInstance()->Size();
  for(uint c = 0; c < num_cells; ++c)
    cells_to_update->push(c);
}

// each thread will run this code
void * RunCell(void * data)
{
  ThreadData & thread_data = *reinterpret_cast<ThreadData *>(data);
  while(thread_data._iterationsComplete < thread_data._totalIterations)
  {
    sem_wait(&thread_data._mutex);
    if(thread_data._toUpdate.empty())
    {
      if(thread_data._iterationsComplete >= thread_data._totalIterations)
        break;
      FillCellStack(&thread_data._toUpdate);
      Grid::GetInstance()->SwapBuffers();
      ++thread_data._iterationsComplete;
    }
    uint index_to_update = thread_data._toUpdate.top();
    thread_data._toUpdate.pop();
    sem_post(&thread_data._mutex);
    Grid::GetInstance()->UpdateCell(index_to_update);
  }
  return NULL;
}

//-----// GAME OF LIFE //-----------------------------------------------------//
void RunGameofLife(unsigned num_threads, const char * read_file,
  unsigned iterations, const char * write_file)
{
  // create grid
  Grid::CreateInstance(read_file);
  // create space for thread ids
  std::vector<pthread_t> threads;
  threads.reserve(num_threads);
  // data to be passed to the thread
  ThreadData thread_data;
  FillCellStack(&thread_data._toUpdate);
  thread_data._totalIterations = iterations;
  thread_data._iterationsComplete = 0;
  sem_init(&thread_data._mutex, 0, 1);
  // Begin creating all needed threads
  uint threads_left = num_threads;
  while(threads_left)
  {
    // create the thread for that range
    pthread_t thread_id;
    int error = pthread_create(&thread_id, NULL, RunCell,
      reinterpret_cast<void *>(&thread_data));
    if(error)
      throw std::runtime_error("A thread was not created.");
    threads.push_back(thread_id);
    // end of previous range is the start of the next
    --threads_left;
  }
  // wait for all threads to finish
  for(uint t = 0; t < num_threads; ++t)
  {
    // return value is not used
    void * return_value;
    int error = pthread_join(threads[t], &return_value);
    if(error)
      throw std::runtime_error("A thread failed to join");
  }
  sem_destroy(&thread_data._mutex);
  Grid::GetInstance()->WriteFrontBuffer(write_file);
  Grid::DestroyInstance();
}

//-----// MAIN //-------------------------------------------------------------//
int main( int argc, char ** argv )
{
    if ( argc != 5 ) {
        std::cout << "<- Expected 4 parameters -> " << std::endl
          << "1. Number of threads to process with" << std::endl
          << "2. Initial population file to read" << std::endl
          << "3. Number of iterations" << std::endl
          << "4. Final population file to write to" << std::endl;
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

    // Try running game of life with given parameters
    try
    {
      RunGameofLife(threads, read_file, iterations, write_file);
    }
    catch(const std::runtime_error & error)
    {
      std::cout << error.what();
    }
}
