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

bool getCc(const UndirectedGraph& g){
    int n = boost::num_vertices(g);
    visited = vector<bool>(n,false); // Inicialitzem el vector de vertexos visitats
    cc = vector<int>(n,0); // Inicialitzem el vector de components conexes a 0
    vector<int> vertices = getVertices(g); // Vector de vertexos
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
    return nCc <= 2;
}

/* ---------------------------------------------------------- */


/* ----------------------------------------------------------
   ------------------ PERCOLATION FUNCS ---------------------
   ---------------------------------------------------------- */

UndirectedGraph vertex_percolation(const UndirectedGraph& g, const double p){
    UndirectedGraph g_per = g;
    vector<int> vertices = getVertices(g_per);
    srand((unsigned)time( NULL )); // Random seed
    for (int i = 0; i < vertices.size(); ++i){
        double p1 = (double)(rand()) / ((double)RAND_MAX);
        if (p1 <= p) boost::remove_vertex (i, g_per);
    }
    return g_per;
}


UndirectedGraph edge_percolation(const UndirectedGraph& g, const double p){
    UndirectedGraph g_per = g;
    vector<int> vertices = getVertices(g_per);
    srand((unsigned)time( NULL )); // Random seed
    int v;
    for (int i = 0; i < vertices.size(); ++i){
        v = vertices[i];
        adjacency_iterator ai, ai_end;
        boost::tie(ai, ai_end) = boost::adjacent_vertices(v,g);
        for (adjacency_iterator it = ai; it != ai_end; ++it){
            double p1 = (double)(rand()) / ((double)RAND_MAX);
            if (p1 <= p){
                int w = *it;
                boost::remove_edge (v, w, g_per);
            }
        }
    }
    return g_per;
}

/* ---------------------------------------------------------- */

/* ----------------------------------------------------------
   ------------------ GRPAH GENERATORS ----------------------
   ---------------------------------------------------------- */

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
    //Inicialitzem graella de nxn a -1
    memset(graella, -1, sizeof(graella[0][0]) * n * n);

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

    // Calculem distàncies entre punts, en el cas que la
    // distància sigui < r afegim edge
    for(int i = 0; i < n ; i++){
        for(int j = i+1; j < n ; j++){
            float dist = distEucl(posicions[i], posicions[j]);
            // Si volem mantenir el pes podriem posar aquí la distància
            if(dist <= r)boost::add_edge(i,j,g);
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

UndirectedGraph graellaGenerator(const int& n){
    int maxRange = pow(n,2);
    UndirectedGraph g(maxRange);
    vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(g);
    for(vertex_iterator it = vi; it != vi_end; ++it) {
        if (*it%n != n - 1) boost::add_edge(*it, (*it + 1), g);
        if (*it + n < maxRange) {
            boost::add_edge(*it, (*it + n), g);
        }
    }
    return g;
}

/* ---------------------------------------------------------- */

int main(){

    // Definició del graf
    int n;
    float p,r;
    string tria;
    string sortir = "n";

    UndirectedGraph g;

    while (sortir == "n") {
        do{
            cout <<
                "Quin graf vols crear? 1.Graella, 2.Binomial o 3.Geometric [1/2/3]: ";
            cin >> tria;
            cout << "\nHas escrit: " << tria << endl;
        }while( tria != "1" and tria != "2" and tria != "3" );

        cout << endl;
        if(tria == "2"){
            cout << "Entra el valor de n i p (natural i real): ";
            cin >> n >> p;
            g = binomialGenerador(n,p);
        } else if (tria == "3"){
            cout << "Entra el valor de n i r (natural i real): ";
            cin >> n >> r;
            g = geometricGenerador(n,r);
        } else if (tria == "1"){
            cout <<"Entra el valor de n (natural): ";
            cin >> n;
            g = graellaGenerator(n);
        }
        cout << endl;

        bool isConnex;

        // Prints del graf resultant
        printEdges(g);
        printVertices(g);
        isConnex = getCc(g);
        if (isConnex) cout << "El graf és connex!\n" << endl;
        else cout << "El graf no és connex.\n" << endl;

        float q = 0.05; // Factor de percolació
        cout << "Introdueix el factor de percolació (real): ";
        cin >> q;
        cout << endl;

        UndirectedGraph gNodePer = vertex_percolation(g, q);
        // Prints del graf resultant amb percolació de nodes
        cout << "##########################################" << endl;
        cout << "Graf resultant de la percolació per nodes:" << endl;
        cout << "##########################################" << endl;
        printEdges(gNodePer);
        printVertices(gNodePer);
        isConnex = getCc(gNodePer);
        if (isConnex) cout << "El graf és connex!" << endl;
        else cout << "El graf no és connex." << endl;
        cout << "##########################################\n" << endl;

        UndirectedGraph gEdgePer = edge_percolation(g, q);
        // Prints del graf resultant amb percolació d'arestes
        cout << "############################################" << endl;
        cout << "Graf resultant de la percolació per arestes:" << endl;
        cout << "############################################" << endl;
        printEdges(gEdgePer);
        printVertices(gEdgePer);
        isConnex = getCc(gEdgePer);
        if (isConnex) cout << "El graf és connex!" << endl;
        else cout << "El graf no és connex." << endl;
        cout << "############################################\n" << endl;

        cout << "Vols sortir i deixar de crear grafs? (s/n): ";
        cin >> sortir;
        cout << endl;
    }

    return 0;
}
