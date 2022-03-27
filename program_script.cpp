#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <fstream>
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

   bool graphIsCCC(const vector<bool> ccc){
       bool complex = true;
       for (int i = 1; complex && i < ccc.size(); ++i){
           if(!ccc[i]) complex = false;
       }
       if (complex) cout << "Totes les components són complexes" << endl;
       else cout << "Hi ha components no complexes" << endl;
       return complex;
   }

   void printCCC(const vector<bool> ccc){
       for (int i = 1; i < ccc.size(); ++i){
           if(ccc[i]) cout << "La component conexa " << i << " es complexa" << endl;
           else cout << "La component conexa " << i << " no es complexa" << endl;
       }
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
              map<int,bool>& visited, map<int,int>& cc, int& cicles){
       visited[v] = true;
       cc[v] = nCc;
       adjacency_iterator ai, ai_end;
       boost::tie(ai, ai_end) = boost::adjacent_vertices(getDescriptor(v,g),g);
       for (adjacency_iterator it = ai; it != ai_end; ++it){
           int id = g[*it].id;
           if (!visited[id]) {
               dfsCc(g,id,v,nCc,visited,cc,cicles);
           }
           else if(id != father) ++cicles;
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
       int cicles;
       // Vector per saber si una component connexa és complexa inizialitzem la
       // components conexa 0 ja que no la utilitzarem
       vector <bool> ccc(1);

       for (vertex_iterator it = vi; it != vi_end; ++it){
           id = g[*it].id;
           if (!visited[id]){
               cicles = 0;
               dfsCc(g,id,id,nCc,visited,cc,cicles);
               if(cicles > 1) ccc.push_back(false);
               else ccc.push_back(true);
               ++nCc;
           }
       }

       //printCc(cc);
       if (nCc <= 2) cout << "El graf es connex" << endl;
       else cout << "El graf no és connex" << endl;
       printCCC(ccc);
       //return graphIsCCC(ccc);
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

int main(int argc, char** argv) {
    // Definició del graf
    int n = std::stoi(argv[2]);
    double p, r, q;
    int tria = std::stoi(argv[1]);
    p = std::stod(argv[3]);
    r = std::stod(argv[4]);
    q = std::stod(argv[5]);

    //cin >> tria >> n >> p >> r >> q;
    UndirectedGraph g;
    string prefix;
    switch (tria) {
        case 1:
            g = graellaGenerator(n);
            prefix = "graella_";
        break;
        case 2:
            g = binomialGenerator(n, p);
            prefix = "binomial_";
        break;
        case 3:
            g = geometricGenerator(n, r);
            prefix = "geometric_";
        break;
        default:
            perror ("Tria invalida");
    }
    bool isConnex;
    int vertexRatio, edgeRatio;
    UndirectedGraph gNodePer = g;
    UndirectedGraph gEdgePer = g;


    vertex_percolation(gNodePer, q);
    isConnex = getCc(gNodePer);
    if (isConnex) {
        string filename = prefix+"vertex_output_connex_"
                          +std::to_string(n)+"_"
                          +std::to_string(q)+".txt";
        ofstream myfile (filename, ios_base::app);
        if (myfile.is_open()){
            myfile << "Is connex\n";
            myfile.close();
        }
        else cout << "Unable to open file";
    }

    edge_percolation(gEdgePer, q);
    isConnex = getCc(gEdgePer);
    if (isConnex) {
        string filename = prefix+"edge_output_connex_"
                          +std::to_string(n)+"_"
                          +std::to_string(q)+".txt";
        ofstream myfile (filename, ios_base::app);
        if (myfile.is_open()){
            myfile << "Is connex\n";
            myfile.close();
        }
        else cout << "Unable to open file";
    }
    return 0;
}
