#include <iostream>
#include <climits>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <set>

// GRAPH INTERFACE / IMPLEMENTATION ///////////////////////////////////////////

class Graph
{
private:
  class Connection
  {
  public:
    Connection(int v, int w) : vertex(v), weight(w) {}
    int vertex;
    int weight;
  };
  class Vertex
  {
  public:
    Vertex(int id) : id(id) {}
    int id;
    std::vector<Connection> connections;
  };
public:
  Graph(const std::vector<std::vector<int> > & am);
  void Print();
  bool IsConnected(std::vector<std::vector<int> > * disjoint_sets);
  bool Dijkstra(int a, int z, std::vector<int> * reverse_path);
  static bool ReadGraphFile(std::vector<std::vector<int> > * am,
    const std::string & filename);
  static void PrintAdjacencyMatrix(const std::vector<std::vector<int> > & am);
private:
  void FillDisjointSet(int vertex_id, std::set<int> * unvisited_vertices,
    std::vector<int> * disjoint_set);
  int FindMiniumumDistance(const std::vector<int> & distance,
    const std::vector<int> & available_vertices);
  std::vector<Vertex> _vertices;
};

Graph::Graph(const std::vector<std::vector<int> > & am)
{
  int num_verts = am.size();
  for(int i = 0; i < num_verts; ++i){

    _vertices.push_back(Vertex(i));
    Vertex & vertex = _vertices.back();

    const std::vector<int> & va = am[i];
    for(int v = 0; v < num_verts; ++v){
      int weight = va[v];
      if(weight != 0)
        vertex.connections.push_back(Connection(v, va[v]));
    }
  }
}

void Graph::Print()
{
  for(const Vertex & vertex : _vertices){
    std::cout << vertex.id << " | ";
    for(const Connection & c : vertex.connections){
      std::cout << "{" << c.vertex << ":" << c.weight << "} ";
    }
    std::cout << std::endl;
  }
}

bool Graph::IsConnected(std::vector<std::vector<int> > * disjoint_sets)
{
  std::set<int> unvisited_vertices;
  // adding all unvisited vertices to set
  for(const Vertex & vertex : _vertices)
    unvisited_vertices.insert(vertex.id);
  // visiting all vertices
  for(const Vertex & vertex : _vertices){
    // don't visit a vertex that has already been visited
    std::set<int>::iterator it = unvisited_vertices.find(vertex.id);
    if(it == unvisited_vertices.end())
      continue;
    // we have a vertex that has not been visited
    // creating a new disjoint set starting with this vertex
    disjoint_sets->push_back(std::vector<int>());
    std::vector<int> & connected_set = disjoint_sets->back();
    FillDisjointSet(vertex.id, &unvisited_vertices, &connected_set);
  }
  if(disjoint_sets->size() > 1)
    return false;
  return true;
}

bool Graph::Dijkstra(int a, int z, std::vector<int> * reverse_path)
{
  // The distance the every vertex
  std::vector<int> distances;
  // The previous vert visited before a vertex
  std::vector<int> previous;
  int num_verts = _vertices.size();
  distances.resize(num_verts, INT_MAX);
  previous.resize(num_verts, -1);

  // put all unvisted verts in a set
  std::set<int> unvisited_vertices;
  for(const Vertex & vertex : _vertices)
    unvisited_vertices.insert(vertex.id);

  std::vector<int> verticies_to_visit;
  verticies_to_visit.push_back(a);
  while(verticies_to_visit.size() < 0){

  }
}

bool Graph::ReadGraphFile(std::vector<std::vector<int> > * am,
  const std::string & filename)
{
  // opening file
  std::fstream file;
  file.open(filename);
  if(!file.is_open()){
    std::cout << "Failed to open " << filename << "." << std::endl;
    return false;
  }

  int num_verts;
  file >> num_verts;

  for(int i = 0; i < num_verts; ++i){
    // creat vector for vertex adjacencies
    am->push_back(std::vector<int>());
    std::vector<int> & va = am->back();
    // reading weights for each edge
    for(int j = 0; j < num_verts; ++j){
      int weight;
      file >> weight;
      va.push_back(weight);
    }
  }

  return true;
}

void Graph::PrintAdjacencyMatrix(const std::vector<std::vector<int> > & am)
{
  int num_verts = am.size();
  for(int i = 0; i < num_verts; ++i){
    const std::vector<int> & va = am[i];
    for(int j = 0; j < num_verts; ++j){
      std::cout << va[j] << " ";
    }
    std::cout << std::endl;
  }
}

void Graph::FillDisjointSet(int vertex_id, std::set<int> * unvisited_vertices,
  std::vector<int> * disjoint_set)
{
  std::stack<int> verticies_to_visit;
  verticies_to_visit.push(vertex_id);
  // visit connected vertices until there are no connected vertices left
  while(verticies_to_visit.size() > 0){
    // visit vertex
    const Vertex & vertex = _vertices[verticies_to_visit.top()];
    unvisited_vertices->erase(vertex.id);
    disjoint_set->push_back(vertex.id);
    verticies_to_visit.pop();
    // add unvisted vertex connections to verticies_to_visit
    for(const Connection & c : vertex.connections){
      std::set<int>::iterator it = unvisited_vertices->find(c.vertex);
      if(it == unvisited_vertices->end())
        continue;
      verticies_to_visit.push(c.vertex);
    }
  }
}

void

// ASSIGNMENT CODE ////////////////////////////////////////////////////////////

void problem1();
void problem2();
void problem3();
void problem4();

int main()
{
  bool running = true;
  while(running){
    unsigned problem = 0;
    std::cout << "Input problem number: ";
    std::cin >> problem;
    std::cout << "---------------------" << std::endl;
    switch (problem) {
    case 1:
      problem1();
      break;
    case 2:
      problem2();
      break;
    case 3:
      problem3();
      break;
    case 4:
      problem4();
      break;
    default:
      std::cout << std::endl << "Not an existing problem" << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    char choice = 0;
    std::cout << "Run another problem [y] [n]: ";
    std::cin >> choice;
    switch(choice){
    case 'y':
      break;
    case 'n':
      running = false;
      break;
    default:
      std::cout << "Not an option. Exiting.";
      running = false;
      break;
    }
  }
}

// PROBLEM 1 CODE /////////////////////////////////////////////////////////////

void problem1()
{
  std::cout << "<Problem 1>" << std::endl;
  std::vector<std::vector<int> > adjacency_matrix;
  bool success = Graph::ReadGraphFile(&adjacency_matrix, "problem1.graph");
  if(!success)
    return;
  Graph graph(adjacency_matrix);
  // finding disjoint sets
  std::vector<std::vector<int> > disjoint_sets;
  bool connected = graph.IsConnected(&disjoint_sets);

  std::cout << "- Adjacency Matrix -" << std::endl;
  Graph::PrintAdjacencyMatrix(adjacency_matrix);
  std::cout << "- Vertex Adjacencies -" << std::endl;
  graph.Print();
  std::cout << "- Is Graph Connected -" << std::endl;
  if(connected)
    std::cout << "Yes" << std::endl;
  else
    std::cout << "No" << std::endl;
  std::cout << "- Disjoint Sets -" << std::endl;
  for(const std::vector<int> & disjoint_set: disjoint_sets){
    std::cout << "{";
    for(int vertex : disjoint_set)
      std::cout << vertex << ",";
    std::cout << "}" << std::endl;
  }
}

// PROBLEM 2 CODE /////////////////////////////////////////////////////////////

void problem2()
{
  std::cout << "<Problem 2>" << std::endl;
  std::vector<std::vector<int> > adjacency_matrix;
  bool success = Graph::ReadGraphFile(&adjacency_matrix, "problem2.graph");
  if(!success)
    return;
  // testing for connections
  Graph graph(adjacency_matrix);
}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

void problem3()
{
  std::cout << "<Problem 3>" << std::endl;
}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

void problem4()
{
  std::cout << "<Problem 4>" << std::endl;
}
