#include <iostream>
#include <fstream>


using namespace std;


//===================================================================
// BCode
//===================================================================

class BCode {
public:
    BCode( string );                // constructor
    string code () const;           // accessor - string value of building code
private:
    string code_;
};


// constructor -- constructs a new building code
BCode::BCode ( string code ) : code_(code) { }

// accessor - returns code value of object
string BCode::code() const {
    return code_;
}


//===================================================================
// Building
//===================================================================

class Building {
public:
    Building ( const BCode&, string );      // constructor
    BCode bCode () const;                   // accessor - building code of the building
    string code () const;                   // accessor - string value of building code of the building
    string name () const;                   // accessor - name of the building
private:
    BCode const bCode_;
    string const name_;
};


// constructor -- constructs a new building with a building code and a name
Building::Building(const BCode &bCode, string name) : bCode_(bCode), name_(name) { }

// accessor - returns building code object of object
BCode Building::bCode() const {
    return bCode_;
}

// accessor - returns string value of building code object of object
string Building::code() const {
    return bCode_.code();
}

// accessor - returns building name value of object
string Building::name() const {
    return name_;
}


// comparison operators
bool operator< (const Building &a, const Building &b) {
    return a.code() < b.code();
}

bool operator>= (const Building &a, const Building &b) {
    return !(a < b);
}


// streaming operator
ostream& operator<<( ostream &sout, const Building &b ) {
    sout << b.code() << "\t" << b.name() << endl;

    return sout;
}


//===================================================================
// BuildingNode
//===================================================================

class BuildingNode {
public:
    BuildingNode( Building*, BuildingNode *next = NULL );   // constructor
    Building* building () const;                            // accessor - building value of the building node
    BuildingNode* next () const;                            // accessor - returns next building node
    void nextIs( BuildingNode* );                           // mutator - update the next building node
private:
    Building* building_;
    BuildingNode* next_;
};


// constructor -- constructs a new building node with an optional building and next building node
BuildingNode::BuildingNode(Building *building, BuildingNode *next) : building_(building), next_(next) { }

// accessor - returns building value of object
Building* BuildingNode::building() const {
    return building_;
}

// accessor - returns next building node value of object
BuildingNode* BuildingNode::next() const {
    return next_;
}

// mutator - updates the next building node value of object
void BuildingNode::nextIs(BuildingNode *next) {
    next_ = next;
}


//===================================================================
// Collection
//===================================================================

class Collection {
public:
    Collection();                               // constructor
    ~Collection();                              // destructor
    void insert( string , string );             // mutator - add building to collection
    void remove( string );                      // mutator - remove building from collection
    Building* findBuilding( string ) const;     // accessor - find building in collection
private:
    BuildingNode* buildings_;
};


// constructor -- constructs a new empty collection of buildings
Collection::Collection() : buildings_(NULL) { }

// destructor -- destructs all buildings in the collection
Collection::~Collection() {
    while(buildings_) {
        BuildingNode *tempNode = buildings_;
        buildings_ = buildings_->next();
        delete tempNode->building();
        delete tempNode;
    }
}

// mutator - adds a building node with a building code and name to the buildings value of object
void Collection::insert(string code, string name) {
    BCode bCode = BCode(code);
    Building *building = new Building(bCode, name);
    buildings_ = new BuildingNode(building, buildings_);
}

// mutator - removes a building node with the building code from the buildings value of object
void Collection::remove(string code) {
    BuildingNode *curNode = buildings_;
    // If the collection of buildings is empty then do nothing
    if(curNode == NULL) {
        return;
    }
    // If the root building node has the building code then delete the root node
    else if (curNode->building()->code() == code) {
        buildings_ = curNode->next();
        delete curNode->building();
        delete curNode;
    }
    // Otherwise check other building nodes and delete the building node with the building code
    else {
        while(curNode->next()) {
            if(curNode->next()->building()->code() == code) {
                BuildingNode *tempNode = curNode->next();
                curNode->nextIs(tempNode->next());
                delete tempNode->building();
                delete tempNode;
                return;
            }
            curNode = curNode->next();
        }
    }
}

// accessor - finds building with code in the collection
Building* Collection::findBuilding(string code) const {
    BuildingNode *curNode = buildings_;
    while(curNode) {
        if(curNode->building()->code() == code) {
            return curNode->building();
        }
        curNode = curNode->next();
    }
    return NULL;
}


//===================================================================
// BuildingEdge
//===================================================================

class BuildingEdge {
public:
    BuildingEdge( BuildingNode*, BuildingNode*, string, BuildingEdge *next = NULL );// constructor
    BuildingNode* node1 () const;                                                   // accessor - first building node of the building edge
    BuildingNode* node2 () const;                                                   // accessor - second building node of the building edge
    string connector () const;                                                      // accessor - connector type of the building edge
    BuildingEdge* next () const;                                                    // accessor - next building edge of the building edge
    void nextIs( BuildingEdge* );                                                   // mutator - updates the next building edge
    bool connects( string, string = "" ) const;                                          // checks if the building edge connects two buildings
private:
    BuildingNode *node1_, *node2_;
    string connector_;
    BuildingEdge* next_;
};


// constructor -- constructs a new building edge with two building nodes, connector type, and an optional next building edge
BuildingEdge::BuildingEdge(BuildingNode *node1, BuildingNode *node2, string connector, BuildingEdge *next) : node1_(node1), node2_(node2), connector_(connector), next_(next) { }

// accessor - returns first building node value of object
BuildingNode* BuildingEdge::node1() const {
    return node1_;
}

// accessor - returns second building node value of object
BuildingNode* BuildingEdge::node2() const {
    return node2_;
}

// accessor - returns connector type value o object
string BuildingEdge::connector() const {
    return connector_;
}

// accessor - returns next building edge value of object
BuildingEdge* BuildingEdge::next() const {
    return next_;
}

// mutator - updates the next building edge value of object
void BuildingEdge::nextIs(BuildingEdge *next) {
    next_ = next;
}

// returns true if building edge connects building nodes with the building code
bool BuildingEdge::connects(string code1, string code2) const {
    if(code2.empty()) {
        return node1_->building()->code() == code1 || node2_->building()->code() == code1;
    } else {
        return (node1_->building()->code() == code1 && node2_->building()->code() == code2) ||
                (node2_->building()->code() == code1 && node1_->building()->code() == code2);
    }
}


//===================================================================
// Graph (of Buildings and Connectors)
//===================================================================

class Graph {
public:
    Graph();                                                // constructor
    ~Graph();                                               // destructor
    Graph ( const Graph& );                                 // copy constructor
    void addNode ( Building* );                             // mutator - add node to graph
    void removeNode ( string );                             // mutator - remove node from graph
    Building* findBuilding ( string ) const;                // accessor - find building stored in node in graph
    void addEdge ( string, string, string );                // mutator - add edge to graph
    void removeEdge ( string, string );                     // mutator - remove edge from graph
    void printPaths ( string, string, const bool = false ) const; // accessor - print path from one node to another
    void deleteGraph();                                     // delete graph
    friend ostream& operator<< ( ostream&, const Graph& );  // insertion operator (insert graph into output stream)
    Graph& operator= ( const Graph& );                      // assignment operator for graph objects
    bool operator== ( const Graph& ) const;                 // equality operator for graph objects

    // TODO: remove only for debugging purposes
    void printNodes() const;
    void printEdges() const;
private:
    BuildingNode* findBuildingNode ( string ) const;        // accessor - finds building node in graph
    void removeAdjacentEdges( string );                     // mutator - removes adjacent building edges of a building node in the graph

    BuildingNode* nodes_;
    BuildingEdge* edges_;
};


// constructor -- constructs a new empty graph
Graph::Graph() : nodes_(NULL), edges_(NULL) { }

// destructor -- destructs building nodes and edges in the graph
Graph::~Graph() {
    deleteGraph();
}

// mutator - adds building node to the building nodes value of object
void Graph::addNode(Building *building) {
    BuildingNode *newNode = new BuildingNode(building);

    if(nodes_ == NULL || *(nodes_->building()) >= *building) {
        newNode->nextIs(nodes_);
        nodes_ = newNode;
    } else {
        BuildingNode *curNode = nodes_;
        while(curNode->next() && *(curNode->next()->building()) < *building) {
            curNode = curNode->next();
        }
        newNode->nextIs(curNode->next());
        curNode->nextIs(newNode);
    }
}

// mutator - removes building node from the building nodes value of object
void Graph::removeNode(string code) {
    BuildingNode *curNode = nodes_;
    // If there are no building nodes then do nothing
    if(curNode == NULL) {
        return;
    }
    // If the root building node has the building code then delete the root node
    else if (curNode->building()->code() == code) {
        nodes_ = curNode->next();
        removeAdjacentEdges(curNode->building()->code());
        delete curNode;
    }
    // Otherwise check other building nodes and delete the building node with the building code
    else {
        while(curNode->next()) {
            if(curNode->next()->building()->code() == code) {
                BuildingNode *tempNode = curNode->next();
                curNode->nextIs(tempNode->next());
                removeAdjacentEdges(tempNode->building()->code());
                delete tempNode;
                break;
            }
            curNode = curNode->next();
        }
    }
}

// accessor - returns building, with the building code, of a building node in the graph
Building* Graph::findBuilding(string code) const {
    BuildingNode *node = findBuildingNode(code);
    if(node) {
        return node->building();
    }
    return NULL;
}

// mutator - adds building edge to the building edges value of object
void Graph::addEdge(string code1, string code2, string connector) {
    edges_ = new BuildingEdge(findBuildingNode(code1), findBuildingNode(code2), connector, edges_);
}

// mutator - remove building edge from the building edges value of object
void Graph::removeEdge(string code1, string code2) {
    BuildingEdge *curEdge = edges_;
    // If there are no building edges then do nothing
    if(curEdge == NULL) {
        return;
    }
    // If the root building edge connects the buildings then delete the root node
    else if (curEdge->connects(code1, code2)) {
        edges_ = curEdge->next();
        delete curEdge;
    }
    // Otherwise check other building edges and delete the building edge that connects the buildings
    else {
        while(curEdge->next()) {
            if(curEdge->next()->connects(code1, code2)) {
                BuildingEdge *tempEdge = curEdge->next();
                curEdge->nextIs(tempEdge->next());
                delete tempEdge;
                break;
            }
            curEdge = curEdge->next();
        }
    }
}

// deletes building nodes and edges values of object
void Graph::deleteGraph() {
    while(edges_) {
        BuildingEdge *tempEdge = edges_;
        edges_ = edges_->next();
        delete tempEdge;
    }
    while(nodes_) {
        BuildingNode *tempNode = nodes_;
        nodes_ = nodes_->next();
        delete tempNode;
    }
}

// TODO: remove only for debugging purposes
void Graph::printNodes() const {
    BuildingNode *curNode = nodes_;
    while(curNode){
        cout << *(curNode->building());
        curNode = curNode->next();
    }
    cout << endl;
}

// TODO: remove only for debugging purposes
void Graph::printEdges() const {
    BuildingEdge *curEdge = edges_;
    while(curEdge){
        cout << *(curEdge->node1()->building()) << *(curEdge->node2()->building()) << curEdge->connector() << endl;
        curEdge = curEdge->next();
    }
    cout << endl;
}

// accessor - returns building node with the building code in the graph
BuildingNode* Graph::findBuildingNode(string code) const {
    BuildingNode *curNode = nodes_;
    while(curNode) {
        if(curNode->building()->code() == code) {
            return curNode;
        }
        curNode = curNode->next();
    }
    return NULL;
}

// mutator - removes building edges with the building code from the building edges value of object
void Graph::removeAdjacentEdges(string code) {
    BuildingEdge *prev;
    BuildingEdge *curEdge = edges_;

    // Delete all leading building edges that have the building
    while(curEdge && curEdge->connects(code)) {
        edges_ = curEdge->next();
        delete curEdge;
        curEdge = edges_;
    }

    // Delete all no leading building edges with the building
    while(curEdge) {
        if(curEdge->connects(code)) {
            prev->nextIs(curEdge->next());
            delete curEdge;
            curEdge = prev->next();
        } else {
            prev = curEdge;
            curEdge = curEdge->next();
        }
    }
}


//************************************************************************
//  Test Harness Helper functions
//************************************************************************

//  test-harness operators
enum Op { NONE, mapPtr, building, wreckage, findB, node, remNode, edge, remEdge, delGraph, copyGraph, assignGraph, eq, path, print };

Op convertOp( string opStr ) {
    switch( opStr[0] ) {
        case 'm': return mapPtr;
        case 'b': return building;
        case 'w': return wreckage;
        case 'f': return findB;
        case 'n': return node;
        case 'v': return remNode;
        case 'e': return edge;
        case 'r': return remEdge;
        case 'd': return delGraph;
        case 'c': return copyGraph;
        case 'a': return assignGraph;
        case 'q': return eq;
        case 'p': return path;
        case 'g': return print;
        default: {
            return NONE;
        }
    }
}

//******************************************************************
// Test Harness for Graph ADT
//******************************************************************

int main( int argc, char *argv[] ) {
    Collection buildings;
    Graph map1, map2;

    // initialize buildings and map1 with input file, if present
    if ( argc > 1 ) {

        ifstream source(argv[1]);
        if ( source.fail() ) {
            cerr << "Error: Could not open file \"" << argv[1] << "\"." << endl;
            return 1;
        }

        // create a collection of buildings, and a map of buildings and interior links

        string type;
        source >> type;
        Op op = convertOp ( type );
        while ( !source.eof() ) {
            switch (op) {

                    // add a new building to the collection of Buildings, and add the building to map1
                case building : {
                    string code;
                    string name;
                    string name2;
                    source >> code >> name;
                    getline( source, name2 );
                    buildings.insert( code, name+name2 );
                    map1.addNode( buildings.findBuilding ( code ) );
                    break;
                }

                    // add a new link between two existing nodes in map1
                case edge: {
                    string code1, code2, type;
                    source >> code1 >> code2 >> type;
                    map1.addEdge( code1, code2, type );
                    string junk;
                    getline ( source, junk );
                    break;
                }

                default: { }
            }
            source >> type;
            op = convertOp( type );
        }
    }

//    cout << map1;

    Graph* map = &map1;  // input commands affect which ever graph that map points to (map1 or map2)

    cout << "Test harness for Graph ADT:" << endl << endl;

    // get input command
    cout << "Command: ";
    string command;
    cin >> command;

    Op op = convertOp( command );

    while ( !cin.eof() ) {
        switch (op) {

                // set variable map to point to new graph (map1 or map2)
            case mapPtr: {
                string mapNo;
                cin >> mapNo;
                map = ( mapNo[0] == '1' ) ? &map1 : &map2;
                break;
            }

//                // print the current map to the console
//            case print: {
//                cout << *map;
//                break;
//            }

                // add a new building to the collection of buildings
            case building : {
                string code;
                string name;
                string name2;
                cin >> code >> name;
                getline( cin, name2 );
                buildings.insert( code, name+name2 );
                break;
            }

                // add an existing building to the current map
            case node: {
                string code;
                cin >> code;
                map->addNode( buildings.findBuilding( code ) );

                // TODO: remove only for debugging purposes
                map->printNodes();

                string junk;
                getline( cin, junk );
                break;
            }

                // find a building in the current map
            case findB: {
                string code;
                cin >> code;
                Building *b = map->findBuilding ( code );
                if ( b ) {
                    cout << *b << endl;
                }
                else {
                    cout << "Couldn't find building " << code << endl;
                }
                string junk;
                getline( cin, junk );
                break;
            }

                // add a new link between existing graph nodes in the current map
            case edge: {
                string code1, code2, type;
                cin >> code1 >> code2 >> type;
                map->addEdge( code1, code2, type );

                // TODO: remove only for debugging purposes
                map->printEdges();

                string junk;
                getline ( cin, junk );
                break;
            }


                // delete the entire graph (no memory leak).  There is no change to the collection of Buildings.
            case delGraph: {
                map->deleteGraph();

                // TODO: remove only for debugging purposes
                map->printNodes();
                map->printEdges();

                break;
            }

                // remove an existing edge from the current map
            case remEdge: {
                string code1, code2;
                cin >> code1 >> code2;
                map->removeEdge( code1, code2 );

                // TODO: remove only for debugging purposes
                map->printEdges();

                string junk;
                getline ( cin, junk );
                break;
            }

                // remove an existing node from the current map.  There is no change to the collection of Buildings.
            case remNode: {
                string code;
                cin >> code;
                map->removeNode( code );

                // TODO: remove only for debugging purposes
                map->printNodes();

                string junk;
                getline( cin, junk );
                break;
            }

                // remove an existing building from the collection of buildings.  The building also needs to be removed from the two maps, as well as all links involving the building
            case wreckage: {
                string code;
                cin >> code;
                map1.removeNode( code );
                map2.removeNode( code );
                buildings.remove ( code );

                // TODO: remove only for debugging purposes
                map1.printNodes();
                map2.printNodes();

                string junk;
                getline ( cin, junk );
                break;
            }

//                // check whether map1 is equal to map2
//            case eq: {
//                if ( map1 == map2 ) {
//                    cout << "Maps 1 and 2 are equal." << endl;
//                }
//                else {
//                    cout << "Maps 1 and 2 are NOT equal." << endl;
//                }
//                break;
//            }
//
//                // graph copy constructor
//            case copyGraph: {
//                Graph map3( *map );
//                cout << map3;
//                string junk;
//                getline( cin, junk );
//                break;
//            }
//
//                // graph assignment operator
//            case assignGraph: {
//                map1 = map2;
//                cout << map1;
//                break;
//            }
//
//                // find path(s) in graph from one building to another building
//            case path: {
//                string code1, code2, all;
//                cin >> code1 >> code2 >> all;
//                cout << "Paths from " << code1 << " to " << code2 << " are: " << endl;
//                bool printall = ( all.length() > 0 && all.at(0) == 't' ) ? true : false;
//                map->printPaths( code1, code2, printall );
//                string junk;
//                getline( cin, junk );
//                break;
//            }

            default: {
                cerr << "Invalid command." << endl;
            }
        }

        cout << "Command: ";
        cin >> command;
        op = convertOp( command );

    } // while cin OK

}