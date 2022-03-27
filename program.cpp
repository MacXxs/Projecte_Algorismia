#include <boost/graph/adjacency_list.hpp>
#include <iostream>
using namespace std;

struct VertexId {
    int id;
};

typedef boost::adjacency_list<boost::setS,
                              boost::setS,
                              boost::undirectedS,
                              VertexId> UndirectedGraph;
typedef boost::graph_traits<UndirectedGraph>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<UndirectedGraph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<UndirectedGraph>::adjacency_iterator adjacency_iterator;

//vector<bool> visited;
//vector<int> cc;

/* ----------------------------------------------------------
   --------------- GENERIC FUNCS & PROCS --------------------
   ---------------------------------------------------------- */

   void printVertices(const UndirectedGraph& g){
       vertex_iterator vi, vi_end, it;
       cout << "Number of vertices = " << boost::num_vertices(g) << "\n"
            << "Vertex list: \n";
       boost::tie(vi, vi_end) = boost::vertices(g);
       it = vi;
       cout << g[*it].id;
       ++it;
       for (; it != vi_end; ++it) {
           cout  << ", " << g[*it].id;
       }
       cout << "\n" << endl;
   }

   void printEdges(const UndirectedGraph& g){
       cout << "Number of edges = " << boost::num_edges(g) << "\n"
            << "Edge list:\n";
       vertex_iterator vi, vi_end;
       boost::tie(vi, vi_end) = boost::vertices(g);
       for (vertex_iterator itV = vi; itV != vi_end; ++itV) {
           adjacency_iterator ai, ai_end;
           boost::tie(ai, ai_end) = boost::adjacent_vertices(*itV,g);
           for (adjacency_iterator itW = ai; itW != ai_end; ++itW){
               if (g[*itW].id > g[*itV].id) cout << '(' << g[*itV].id << ',' << g[*itW].id << ')' << endl;
           }
       }
       cout << "\n" << endl;
   }

   vertex_descriptor getDescriptor(const int& id, const UndirectedGraph& g){
       vertex_descriptor v;
       bool exit = false;
       vertex_iterator vi, vi_end, next;
       boost::tie(vi, vi_end) = boost::vertices(g);
       next = vi;
       // Busquem el vertex que té l'Id passat per param.
       while (!exit and vi != vi_end) {
           ++next;
           if (g[*vi].id == id) {
               v = *vi;
               exit = true;
           }
           vi = next;
       }
       return v;
   }

   double distEucl(pair<int, int> punt1, pair<int, int> punt2){

       float varX = (punt1.first)-(punt2.first);
       float varY = (punt1.second)-(punt2.second);

       float dist = sqrt( varX*varX + varY*varY );

       return dist;
   }

   void printCc(map<int,int>& cc){
       cout << "Connected components:\n";
       for (std::map<int,int>::iterator it=cc.begin(); it!=cc.end(); ++it){
           cout << "Vertex " << it->first << " is in cc " << it->second << endl;
       }
       cout << endl;
   }

   void dfsCc(const UndirectedGraph& g, const int& v,
              const int& father, const int& nCc,
              map<int,bool>& visited, map<int,int>& cc){
       visited[v] = true;
       cc[v] = nCc;
       adjacency_iterator ai, ai_end;
       boost::tie(ai, ai_end) = boost::adjacent_vertices(getDescriptor(v,g),g);
       for (adjacency_iterator it = ai; it != ai_end; ++it){
           int id = g[*it].id;
           if (!visited[id]) {
               dfsCc(g,id,v,nCc,visited,cc);
           }
       }
   }

   bool getCc(const UndirectedGraph& g){
       int n = boost::num_vertices(g);

       map<int,bool> visited; // Inicialitzem el vector de vertexos visitats
       map<int,int> cc; // Inicialitzem el vector de components conexes a 0

       vertex_iterator vi, vi_end;
       boost::tie(vi, vi_end) = boost::vertices(g);
       for (vertex_iterator it = vi; it != vi_end; ++it){
           visited[g[*it].id] = false;
           cc[g[*it].id] = 0;
       }
       int nCc = 1; // Primera component conexa
       int id;

       for (vertex_iterator it = vi; it != vi_end; ++it){
           id = g[*it].id;
           if (!visited[id]){
               dfsCc(g,id,id,nCc,visited,cc);
               ++nCc;
           }
       }

       printCc(cc);
       return nCc <= 2;
   }

/* ---------------------------------------------------------- */


/* ----------------------------------------------------------
   ------------------ GRPAH GENERATORS ----------------------
   ---------------------------------------------------------- */

UndirectedGraph graellaGenerator(const int& n){
    int maxRange = pow(n,2);
    UndirectedGraph g;
    vertex_descriptor v;
    // Definim el graf de tamany n²
    for (int i = 0; i < maxRange; ++i){
        v = boost::add_vertex(g);
        g[v].id = i;
    }
    // Creem les adjacències amb els 4 veïns
    vertex_iterator vi, vi_end, aux;
    boost::tie(vi, vi_end) = boost::vertices(g);
    for(vertex_iterator it = vi; it != vi_end; ++it) {
        if (g[*it].id % n != n - 1) {
            aux = it; ++aux;
            boost::add_edge(*it, *aux, g);
        }
        if (g[*it].id + n < maxRange) {
            aux = it;
            for (int j = 0; j < n; ++j) ++aux;
            boost::add_edge(*it, *aux, g);
        }
    }

    return g;
}

UndirectedGraph binomialGenerator(const int& n, const double& p){
    UndirectedGraph g;
    vertex_descriptor v;
    // Definim el graf de tamany n
    for (int i = 0; i < n; ++i){
        v = boost::add_vertex(g);
        g[v].id = i;
    }
    // Creem les adjacències amb la resta de nodes amb prob p
    srand((unsigned)time( NULL ));
    vertex_iterator vi, vi_end, aux;
    boost::tie(vi, vi_end) = boost::vertices(g);
    for(vertex_iterator it = vi; it != vi_end; ++it) {
        aux = it;
        ++aux;
        while (aux != vi_end){
            double prob = (double) rand()/RAND_MAX;
            if(prob <= p)boost::add_edge(*it,*aux,g);
            ++aux;
        }
        cout << endl;
    }

    return g;
}

UndirectedGraph geometricGenerator(const int& n, const double& r){
    UndirectedGraph g;
    vertex_descriptor v;
    // Definim el graf de tamany n
    for (int i = 0; i < n; ++i){
        v = boost::add_vertex(g);
        g[v].id = i;
    }

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
            if(dist <= r)boost::add_edge(getDescriptor(i,g),getDescriptor(j,g),g);
        }
    }

    //Debug de la graella
    cout << "\nGRAELLA FINAL" <<endl;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            if(graella[i][j] != -1){
                cout<<" " << graella[i][j];
            }else cout << " X";
        }
        cout << endl;
    }
    cout << endl;

    return g;
}

/* --------------------------------------------------------- */

/* ----------------------------------------------------------
   ------------------ PERCOLATION FUNCS ---------------------
   ---------------------------------------------------------- */

void vertex_percolation(UndirectedGraph& g_per, const double q){
  vertex_iterator vi, vi_end, next;
  adjacency_iterator ai, ai_end;
  boost::tie(vi, vi_end) = boost::vertices(g_per);
  srand((unsigned)time( NULL )); // Random seed
  for(next = vi; next != vi_end; vi = next) {
      double p = (double)(rand()) / ((double)RAND_MAX);
      ++next;
      if (p <= q) {
          boost::clear_vertex(*vi, g_per); // S'eliminen les arestes
          boost::remove_vertex(*vi, g_per); // S'elimina el vertex
      }
  }
}

void edge_percolation(UndirectedGraph& g_per, const double q){
   vertex_iterator vi, vi_end;
   boost::tie(vi, vi_end) = boost::vertices(g_per);
   srand((unsigned)time( NULL )); // Random seed
   for(vertex_iterator it = vi; it != vi_end; ++it) {
       adjacency_iterator ai, ai_end, next;
       boost::tie(ai, ai_end) = boost::adjacent_vertices(*it,g_per);
       for (next = ai; next != ai_end; ai = next){
           ++next;
           double p = (double)(rand()) / ((double)RAND_MAX);
           if (p <= q){
               boost::remove_edge (*it, *ai, g_per);
           }
       }
   }
}

/* --------------------------------------------------------- */

int main() {
    int n;
    double p,r;
    string tria;
    string sortir = "n";
    // Definició del graf
    UndirectedGraph g;

    cout <<
        "Quin graf vols crear? 1.Graella, 2.Binomial o 3.Geometric [1/2/3]: ";
    cin >> tria;
    cout << "\nHas escrit: " << tria << endl;
    while( tria != "1" and tria != "2" and tria != "3" );

    cout << endl;
    if(tria == "2"){
        cout << "Entra el valor de n i p (natural i real): ";
        cin >> n >> p;
        g = binomialGenerator(n,p);
    } else if (tria == "3"){
        cout << "Entra el valor de n i r (natural i real): ";
        cin >> n >> r;
        g = geometricGenerator(n,r);
    } else if (tria == "1"){
        cout <<"Entra el valor de n (natural): ";
        cin >> n;
        g = graellaGenerator(n);
    }
    cout << endl;

    bool isConnex;

    // Prints del graf resultant
    printVertices(g);
    printEdges(g);
    isConnex = getCc(g);
    if (isConnex) cout << "El graf és connex!\n" << endl;
    else cout << "El graf no és connex.\n" << endl;

    float q = 0.05; // Factor de percolació
    cout << "Introdueix el factor de percolació (real): ";
    cin >> q;
    cout << endl;

    UndirectedGraph gNodePer = g;
    UndirectedGraph gEdgePer = g;

    vertex_percolation(gNodePer, q);
    // Prints del graf resultant amb percolació de nodes
    cout << "##########################################" << endl;
    cout << "Graf resultant de la percolació per nodes:" << endl;
    cout << "##########################################" << endl;
    printVertices(gNodePer);
    printEdges(gNodePer);
    isConnex = getCc(gNodePer);
    if (isConnex) cout << "El graf és connex!" << endl;
    else cout << "El graf no és connex." << endl;
    cout << "##########################################\n" << endl;

    edge_percolation(gEdgePer, q);
    // Prints del graf resultant amb percolació d'arestes
    cout << "############################################" << endl;
    cout << "Graf resultant de la percolació per arestes:" << endl;
    cout << "############################################" << endl;
    printVertices(gEdgePer);
    printEdges(gEdgePer);
    isConnex = getCc(gEdgePer);
    if (isConnex) cout << "El graf és connex!" << endl;
    else cout << "El graf no és connex." << endl;
    cout << "############################################\n" << endl;

    return 0;
}
