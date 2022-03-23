#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <utility>
#include <time.h>
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
/* -------------------------------------------------- */

/* ------------ FUNCIONS I PROCESSOS ---------------- */

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
    cout << "Number of vertices = " << boost::num_vertices(g) << "\n"
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
    int n = boost::num_vertices(g); // Inicialitzem el vector de vertexos visitats
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

/*-----------------------------------------------------------*/


UndirectedGraph binomialGenerador(int n, float p){
    UndirectedGraph g(n); 
    srand((unsigned)time( NULL ));
    for(int i = 0; i < n ; i++){
        for(int j = i+1; j < n ;j++){
            float prob = (float) rand()/RAND_MAX;
            if(prob <= p)boost::add_edge(i,j,g);
        }
    }
    return g;
}


float distEucl(pair<int, int> punt1, pair<int, int> punt2){
    
    float varX = (punt1.first)-(punt2.first);
    float varY = (punt1.second)-(punt2.second);
    
    float dist = sqrt( varX*varX + varY*varY );
        
    return dist; 
}


UndirectedGraph geometricGenerador(int n,float r){
    
    UndirectedGraph g(n); 
    srand((unsigned)time( NULL ));
    int graella[n][n];
    vector< pair<int, int> > posicions(n); 
    memset(graella, -1, sizeof(graella[0][0]) * n * n);  //Inicialitzem graella de nxn a -1
    
    //Assignem de forma random les posicions dels punts
    for( int i = 0 ; i < n ; i++ ){
        
        int pos_x, pos_y;
        
        do{
            float prob = (float) rand()/RAND_MAX;
            pos_x = n*prob;
            prob = (float) rand()/RAND_MAX;
            pos_y = n*prob;
            
        } while(graella[pos_x][pos_y] != -1);
        //Aquest while el faig per si la posició que assignem ja està assignada
        graella[pos_x][pos_y] = i; 
        posicions[i] = pair<int,int>(pos_x,pos_y);
        
    }
    
    // Calculem distàncies entre punts, en el cas que la distància sigui < r afegim edge
    for(int i = 0; i < n ; i++){
        for(int j = i+1; j < n ; j++){
            float dist = distEucl(posicions[i], posicions[j]);
            if(dist <= r)boost::add_edge(i,j,g); // Si volem mantenir el pes podriem posar aquí la distància
        }
    }
    
    
    //Això es pot borrar, vaig a dibuixar la graella per veure que funcioni
    
    cout << "GRAELLA FINAL" <<endl; 
    for(int i = 0; i<n; i++){
        for(int j=0; j<n; j++){
            if(graella[i][j] != -1){
                cout<<" " << graella[i][j];
            }else cout << " X";
        }
        cout << endl;
    }
    
    
    return g;
}


/* -------------------------------------------------- */

int main(){
    
    // Definició del graf
    int n; 
    float p,r; 
    char tria;
    
    UndirectedGraph g; 
    
    do{
        cout <<"Amb què vols calcular? Binomial o Geometric [b/g]"<<endl;
        cin >> tria; 
        cout <<"Has escrit" << tria; 
    }while( tria != 'g' and tria != 'b' );

    if(tria == 'b'){
        cout <<"Entra el valor de n i p"<<endl;
        cin >> n >> p;
        g = binomialGenerador(n,p);
    } else if (tria == 'g'){
        cout <<"Entra el valor de n i r"<<endl;
        cin >> n >> r;
        g = geometricGenerador(n,r);
    }
    
    // Recorregut per les arestes del graf
    printEdges(g);
    // Recorregut pels vertexos del graf
    printVertices(g);

    getCc(g);
    return 0;
}
