#include <boost/graph/adjacency_list.hpp>
#include <random>
#include <iostream>
#include <iomanip>
using namespace std;

/* --------------------- TIPOS ---------------------- */
struct vertex_percolation_t {
  typedef boost::vertex_property_tag kind;
};

struct edge_percolation_t {
  typedef boost::edge_property_tag kind;
};

typedef boost::property<vertex_percolation_t, float> VertexPercolationProperty;
typedef boost::property<edge_percolation_t, float> EdgePercolationProperty;
typedef boost::adjacency_list<
                      boost::listS,
                      boost::vecS,
                      boost::undirectedS,
                      VertexPercolationProperty,
                      EdgePercolationProperty > UndirectedGraph;
typedef boost::graph_traits<UndirectedGraph>::edge_iterator edge_iterator;
typedef boost::graph_traits<UndirectedGraph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<UndirectedGraph>::adjacency_iterator adjacency_iterator;

vector<bool> visited;
vector<int> cc;
int n = 1000;
/* -------------------------------------------------- */

/* ------------ FUNCIONS I PROCESSOS ---------------- */
void addEdges(UndirectedGraph& g, const double& p ){
    std::srand(std::time(nullptr));
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < n; ++j){
            if(j != i) {
                double p1 = (double)(rand()) / ((double)RAND_MAX);
                if (p1 <= p) boost::add_edge (i, j, g);
            }
        }
    }
    
}

void printEdges(const UndirectedGraph& g){
    edge_iterator ei, ei_end;
    boost::tie(ei,ei_end) = boost::edges(g);
    cout << "Number of edges = " << boost::num_edges(g) << "\n"
         << "Edge list:\n";
    for (edge_iterator it = ei; it != ei_end; ++it) {
        cout << *it << endl;
    }
    cout << endl;
}

void printVertices(const UndirectedGraph& g){
    vertex_iterator vi, vi_end;
    cout << "Number of vertices = " << n << "\n"
         << "Vertex list: \n";
    boost::tie(vi, vi_end) = boost::vertices(g);
    for (vertex_iterator it = vi; it != vi_end; ++it) {
        cout << *it << endl;
    }
    cout << endl;
}

void printCc(){
    cout << "Connected components:\n";
    for (int i = 0; i < cc.size(); ++i){
        cout << "Vertex " << i << " is in cc " << cc[i] << endl;
    }
    cout << endl;
}

vector<int> getVertices(const UndirectedGraph& g){
    vector<int> vertices;
    vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(g);
    for(vertex_iterator it = vi; it != vi_end; ++it) {
        vertices.push_back(*it);
    }
    return vertices;
}

bool noCcLeft(){
    bool noneLeft = true;
    for (const int& i : cc) {
        noneLeft = i!=0;
    }
    return noneLeft;
}

void dfsCc(const UndirectedGraph& g, const int& v,
           const int& father, const int& nCc){
    visited[v] = true;
    cc[v] = nCc;
    adjacency_iterator ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(v,g);
    for (adjacency_iterator it = ai; it != ai_end; ++it){
        int w = *it;
        if (!visited[w]) dfsCc(g,w,v,nCc);
    }
}

void getCc(const UndirectedGraph& g){
     // Inicialitzem el vector de vertexos visitats
    visited = vector<bool>(n,false); // Inicialitzem el vector de components conexes
    cc = vector<int>(n,0); // Inicialitzem el vector de components conexes a 0
    vector<int> vertices = getVertices(g); //
    int nCc = 1; // Primera component conexa
    int v;
    for (int i = 0; i < vertices.size(); ++i){
        v = vertices[i];
        if (!visited[v]) {
            dfsCc(g,v,v,nCc);
            ++nCc;
        };
    }
    
    printCc();
}
/* -------------------------------------------------- */

int main(){
    // Definició del graf
    UndirectedGraph g(n);
    // Addició les arestes del graf
    double p = 0.03;
    addEdges(g, p);
    // Recorregut per les arestes del graf
    printEdges(g);
    // Recorregut pels vertexos del graf
    printVertices(g);

    getCc(g);

    cout << "Number of edges = " << boost::num_edges(g) << endl;
    return 0;
}
