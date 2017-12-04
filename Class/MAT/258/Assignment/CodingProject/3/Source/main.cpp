#include <iostream>
#include <climits>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>

// GRAPH INTERFACE / IMPLEMENTATION ///////////////////////////////////////////

class Graph
{
public:
  class Connection
  {
  public:
    Connection(int a, int b, int w) : a(a), b(b), weight(w) {}
    int a;
    int b;
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
  Graph();
  Graph(const std::vector<std::vector<int> > & am);
  void AddVert(int id);
  void Print();
  bool IsConnected(std::vector<std::vector<int> > * disjoint_sets);
  int Dijkstra(int a, int z, std::vector<int> * reverse_path);
  int Prim(std::vector<std::vector<int> > * am);
  int Kruskal(std::vector<std::vector<int> > * am);
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

Graph::Graph() : _vertices()
{}

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
        vertex.connections.push_back(Connection(i, v, va[v]));
    }
  }
}

void Graph::Print()
{
  for(const Vertex & vertex : _vertices){
    std::cout << vertex.id << " | ";
    for(const Connection & c : vertex.connections){
      std::cout << "{" << c.b << ":" << c.weight << "} ";
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

// Dijstra's Algorithm Implementation //====================================//

struct DVInfo
{
  DVInfo(int id, int distance) :
    id(id), distance(distance), previous(-1)
  {}
  int id;
  int distance;
  int previous;
};
struct DVInfoCompare
{
  bool operator()(const DVInfo * a, const DVInfo * b)
  {
    if(a->distance > b->distance)
      return true;
    return false;
  }
};

int Graph::Dijkstra(int a, int z, std::vector<int> * reverse_path)
{
  std::vector<DVInfo> vertex_infos;
  for(const Vertex & vertex : _vertices){
    if(vertex.id == a)
      vertex_infos.push_back(DVInfo(a, 0));
    else
      vertex_infos.push_back(DVInfo(vertex.id, INT_MAX));
  }
  // put all unvisted verts in a priority queue
  std::priority_queue<DVInfo *, std::vector<DVInfo *>, DVInfoCompare>
    unvisited_vertices;
  for(DVInfo & v_info : vertex_infos){
    unvisited_vertices.push(&v_info);
  }

  while(!unvisited_vertices.empty()){
    const DVInfo & vinfo = *unvisited_vertices.top();
    // The shortest path is to z
    if(vinfo.id == z)
      break;
    const Vertex & vertex = _vertices[vinfo.id];
    for(const Connection & connection : vertex.connections){
      int new_distance = vinfo.distance + connection.weight;
      DVInfo & connected_vert_vinfo = vertex_infos[connection.b];
      if(new_distance < connected_vert_vinfo.distance){
        connected_vert_vinfo.distance = new_distance;
        connected_vert_vinfo.previous = vinfo.id;
      }
    }
    unvisited_vertices.pop();
  }

  DVInfo & current_vinfo = vertex_infos[z];
  if(current_vinfo.previous == -1)
    return -1;
  int total_distance = current_vinfo.distance;
  while(current_vinfo.previous != -1){
    reverse_path->push_back(current_vinfo.id);
    current_vinfo = vertex_infos[current_vinfo.previous];
  }
  return total_distance;

}

// Prim's Algorithm Implementation //=======================================//

struct ConnectionCompare
{
  bool operator()(const Graph::Connection * a, const Graph::Connection * b)
  {
    if(a->weight > b->weight)
      return true;
    return false;
  }
};

#define INVALID_VERTEX  -1

int Graph::Prim(std::vector<std::vector<int> > * am)
{
  // vert a / vert b pair of the edge with the least weight
  int vert_a = INVALID_VERTEX;
  int vert_b = INVALID_VERTEX;
  int minimum_edge_weight = INT_MAX;
  // finding the edge with the least weight
  for(int i = 0; i < (int)_vertices.size(); ++i){
    const Vertex & vert = _vertices[i];
    for(int j = 0; j < (int)vert.connections.size(); ++j){
      const Connection & connection = vert.connections[j];
      if(connection.weight < minimum_edge_weight){
        vert_a = i;
        vert_b = connection.b;
        minimum_edge_weight = connection.weight;
      }
    }
  }
  // prepparing new adjacency matrixx
  am->resize(_vertices.size());
  for(std::vector<int> & adjacencies : *am)
    adjacencies.resize(_vertices.size(), 0);
  // filling first connection in adjacency matrix
  int total_weight = minimum_edge_weight;
  (*am)[vert_a][vert_b] = minimum_edge_weight;
  (*am)[vert_b][vert_a] = minimum_edge_weight;
  // first 2 verts are no longer usable
  std::set<int> added_verts;
  added_verts.insert(vert_a);
  added_verts.insert(vert_b);
  // adding all connections from a and b
  std::priority_queue<Connection *, std::vector<Connection *>,
    ConnectionCompare> available_connections;
  std::vector<Connection> & a_connections = _vertices[vert_a].connections;
  std::vector<Connection> & b_connections = _vertices[vert_b].connections;
  for(Connection & connection : a_connections)
    available_connections.push(&connection);
  for(Connection & connection : b_connections)
    available_connections.push(&connection);
  // finding all other edges
  while(!available_connections.empty()){
    Connection * c = available_connections.top();
    std::set<int>::iterator it = added_verts.find(c->b);
    // introduced vertex is already connected
    if(it != added_verts.end()){
      available_connections.pop();
      continue;
    }
    // introduced vertex in not already connected
    (*am)[c->a][c->b] = c->weight;
    (*am)[c->b][c->a] = c->weight;
    total_weight += c->weight;
    added_verts.insert(c->b);
    // adding connections from new vert as available connections
    std::vector<Connection> & new_connections = _vertices[c->b].connections;
    for(Connection & connection : new_connections)
      available_connections.push(&connection);
  }
  return total_weight;
}

// Kruskal's Algorithm Implementation //====================================//

#define NO_TREE -1

int Graph::Kruskal(std::vector<std::vector<int> > * am)
{
  // prep new minimum spanning tree adjacency matrix
  am->resize(_vertices.size());
  for(std::vector<int> & adjancencies : *am)
    adjancencies.resize(_vertices.size(), 0);
  // add every connection to min priority queue
  std::priority_queue<Connection *, std::vector<Connection *>,
    ConnectionCompare> available_connections;
  for(Vertex & vert : _vertices){
    for(Connection & c : vert.connections)
      available_connections.push(&c);
  }
  // main algorithm start
  int total_weight = 0;
  std::vector<std::set<int> > trees;
  // go through all available connections
  while(!available_connections.empty()){
    // get connection with least weight
    Connection * c = available_connections.top();
    // find which tree a and b are in
    int a_tree = NO_TREE;
    int b_tree = NO_TREE;
    for(int t = 0; t < (int)trees.size(); ++t){
      std::set<int> & tree = trees[t];
      std::set<int>::iterator it_a = tree.find(c->a);
      std::set<int>::iterator it_b = tree.find(c->b);
      if(it_a != tree.end())
        a_tree = t;
      if(it_b != tree.end())
        b_tree = t;
    }
    // verts are in the same tree
    if(a_tree != NO_TREE && a_tree == b_tree){
      available_connections.pop();
      continue;
    }
    // verts are not in any tree
    if(a_tree == NO_TREE && a_tree == b_tree){
      trees.push_back(std::set<int>());
      trees.back().insert(c->a);
      trees.back().insert(c->b);
    }
    // vert a is not in a tree but b is
    else if(a_tree == NO_TREE && a_tree != b_tree)
      trees[b_tree].insert(c->a);
    // vert b is not in a tree but a is
    else if(b_tree == NO_TREE && a_tree != b_tree)
      trees[a_tree].insert(c->b);
    // verts are in different trees (combine trees together)
    else{
      std::set<int> & other_tree = trees[b_tree];
      trees[a_tree].insert(other_tree.begin(), other_tree.end());
      trees.erase(trees.begin() + b_tree);
    }
    // add edge to adjacency matrix
    (*am)[c->a][c->b] = c->weight;
    (*am)[c->b][c->a] = c->weight;
    total_weight += c->weight;
    // move to next connection
    available_connections.pop();
  }
  return total_weight;
}

//==========================================================================//

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
      std::set<int>::iterator it = unvisited_vertices->find(c.b);
      if(it == unvisited_vertices->end())
        continue;
      verticies_to_visit.push(c.b);
    }
  }
}

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
  std::cout << "- Is Graph Connected -" << std::endl;
  if(connected){
    std::cout << "Yes" << std::endl;
    return;
  }
  else
    std::cout << "No" << std::endl;
  std::cout << "- Disjoint Sets -" << std::endl;
  for(const std::vector<int> & disjoint_set: disjoint_sets){
    std::cout << "{";
    for(int vertex : disjoint_set)
      std::cout << vertex + 1 << ",";
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
  std::cout << "- Adjacency Matrix -" << std::endl;
  graph.PrintAdjacencyMatrix(adjacency_matrix);

  int a;
  int z;
  std::cout << "Input Starting Vertex (a): ";
  std::cin >> a; --a;
  std::cout << "Input Ending Vertex (z): ";
  std::cin >> z; --z;
  std::vector<int> reverse_path;
  int distance = graph.Dijkstra(a, z, &reverse_path);
  std::cout << "- Path Found -" << std::endl;
  if(distance != -1){
    std::cout << "Yes" << std::endl;
    std::cout << "- Path -" << std::endl;
    std::vector<int>::reverse_iterator rit = reverse_path.rbegin();
    std::cout << a + 1;
    while(rit != reverse_path.rend()){
      std::cout << " -> ";
      std::cout << *rit + 1;
      ++rit;
    }
    std::cout << std::endl;
    std::cout << "- Distance -" << std::endl
      << distance << std::endl;
  }
  else{
    std::cout << "No" << std::endl;
  }

}

// PROBLEM 3 CODE /////////////////////////////////////////////////////////////

void problem3()
{
  // set up
  std::cout << "<Problem 3>" << std::endl;
  std::vector<std::vector<int> > adjacency_matrix;
  bool success = Graph::ReadGraphFile(&adjacency_matrix, "problem3.graph");
  if(!success)
    return;
  // performing operations
  Graph graph(adjacency_matrix);
  std::vector<std::vector<int> > mst_am;
  int total_weight = graph.Prim(&mst_am);
  // printing results
  std::cout << "- Original Adjacency Matrix -" << std::endl;
  Graph::PrintAdjacencyMatrix(adjacency_matrix);
  std::cout << "- Minimum Spanning Tree Adjacency Matrix -" << std::endl;
  Graph::PrintAdjacencyMatrix(mst_am);
  std::cout << "- Total Weight -" << std::endl << total_weight << std::endl;
}

// PROBLEM 4 CODE /////////////////////////////////////////////////////////////

void problem4()
{
  // set up
  std::cout << "<Problem 4>" << std::endl;
  std::vector<std::vector<int> > am;
  bool success = Graph::ReadGraphFile(&am, "problem4.graph");
  if(!success)
    return;
  // performing operations
  Graph graph(am);
  std::vector<std::vector<int> > mst_am;
  int total_weight = graph.Kruskal(&mst_am);
  // printing results
  std::cout << "- Original Adjacency Matrix -" << std::endl;
  Graph::PrintAdjacencyMatrix(am);
  std::cout << "- Minimum Spanning Tree Adjacency Matrix -" << std::endl;
  Graph::PrintAdjacencyMatrix(mst_am);
  std::cout << "- Total Weight -" << std::endl << total_weight << std::endl;
}
