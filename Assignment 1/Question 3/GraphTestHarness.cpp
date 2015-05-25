#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>


using namespace std;


//===================================================================
// BCode
//===================================================================

class BCode {
public:                         // Building Code ADT
    explicit BCode( string );       // constructor
    string code () const;           // accessor - returns shorthand Building Code
private:
    string code_;
};

// constructor - creates a new Building Code
BCode::BCode ( string code ) : code_(code) { }

// accessor - returns the code value of object
string BCode::code() const {
    return code_;
}

// comparison operators
bool operator== (const BCode &a, const BCode &b) {
    return a.code() == b.code();
}

bool operator!= (const BCode &a, const BCode &b) {
    return !(a==b);
}

// streaming operators
istream& operator>> ( istream &sin, BCode &b ) {
    string code;
    sin >> code;
    b = BCode(code);

    return sin;
}

ostream& operator<< ( ostream &sout, const BCode &b ) {
    sout << b.code();

    return sout;
}


//===================================================================
// Building
//===================================================================

class Building {
public:                                 // Building ADT
    Building ( const BCode&, string );      // constructor
    BCode bCode () const;                   // accessor - returns the building code object of building
    string name () const;                   // accessor - returns the name of the building
private:
    BCode const bCode_;
    string const name_;
};

//constructor - building code
Building::Building(const BCode &bCode, string name) : bCode_(bCode), name_(name) { }

// accessor - returns the building code object of building
BCode Building::bCode() const {
    return bCode_;
}

// accessor - returns the name value of object
string Building::name() const {
    return name_;
}

// streaming operator
ostream& operator<<( ostream &sout, const Building &b ) {
    sout << b.bCode() << "\t" << b.name() << endl;

    return sout;
}

//===================================================================
// BuildingNode
//===================================================================

struct BuildingNode {
    Building* building;
    BuildingNode *next;
};


//===================================================================
// Collection
//===================================================================

class Collection {
public:                                     // Collection
    Collection();                               // constructor
    void insert( string , string );             // inserts building with name and code into the collection
    void remove( string );                      // removes the building with the same code from the collection
    Building* findBuilding( string ) const;     // finds and returns the building with the given building code
private:
    BuildingNode* buildings_;
};

// constructor - creates a new collection object with the an empty buildings set
Collection::Collection() : buildings_(NULL) { }

// inserts building with name and code into the collection
void Collection::insert(string code, string name) {
    BuildingNode* newBuilding = new BuildingNode;
    BCode bCode = BCode(code);
    newBuilding->building = new Building(bCode, name);
    newBuilding->next = buildings_;
    buildings_ = newBuilding;
}

// removes the building with the same code from the collection
void Collection::remove(string code) {
    BuildingNode* curNode = buildings_;
    if (curNode == NULL) {
        return;
    } else if (curNode->building->bCode().code() == code) {
        buildings_ = curNode->next;
        delete curNode->building;
        delete curNode;
    } else {
        BuildingNode* curNode_next = curNode->next;
        while (curNode_next != NULL) {
            if (curNode_next->building->bCode().code() == code) {
                curNode->next = curNode_next->next;
                delete curNode_next->building;
                delete curNode_next;
                return;
            }
            curNode = curNode->next;
            curNode_next = curNode_next->next;
        }
    }
}

// finds and returns the building with the given building code
Building* Collection::findBuilding(string code) const {
    BuildingNode* curNode = buildings_;
    while (curNode != NULL) {
        if (curNode->building->bCode().code() == code) {
            return curNode->building;
        }
        curNode = curNode->next;
    }
    return NULL;
}


//===================================================================
// BuildingConnector
//===================================================================

struct BuildingConnector {
    BuildingNode *from, *to;
    string connectorType;
    BuildingConnector *next;
};


//===================================================================
// Graph (of Buildings and Connectors)-
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
private:
    BuildingNode* findBuildingNode ( string ) const;        // finds buildingNode in with the same code
    void removeAdjacentEdge ( string );                     // removes adjacentEdges for the specified building with building code
    BuildingConnector* findBuildingConnector ( string, string ) const; // finds building connectors linking building with code1 to building with code2
    void printReverse( struct BuildingNode*, struct BuildingNode* ) const; // prints the list of buildingNodes in reverse
    int countConnector () const;                            // counts the total number of connectors or edges
    BuildingNode* buildings_;
    BuildingConnector* connectors_;
};


Graph::Graph() : buildings_(NULL), connectors_(NULL) { }

Graph::~Graph() {
    deleteGraph();
}

Graph::Graph(const Graph &graph) {
    BuildingNode* curNode = graph.buildings_;
    buildings_ = NULL;
    while(curNode != NULL) {
        addNode(curNode->building);
        curNode = curNode->next;
    }

    BuildingConnector* curConnector = graph.connectors_;
    connectors_ = NULL;
    while(curConnector != NULL) {
        addEdge(curConnector->from->building->bCode().code(), curConnector->to->building->bCode().code(), curConnector->connectorType);
        curConnector = curConnector->next;
    }
}

void Graph::addNode(Building *building) {
    BuildingNode* newBuilding = new BuildingNode;
    newBuilding->building = building;

    if (buildings_ == NULL || buildings_->building->bCode().code() >= building->bCode().code()) {
        newBuilding->next = buildings_;
        buildings_ = newBuilding;
    } else {
        BuildingNode* currNode = buildings_;
        while (currNode->next != NULL) {
            if(currNode->next->building->bCode().code() > building->bCode().code()) {
                newBuilding->next = currNode->next;
                break;
            }
            currNode = currNode->next;
        }
        currNode->next = newBuilding;
    }
}

void Graph::removeNode(string code) {
    BuildingNode* curNode = buildings_;
    if (curNode == NULL) {
        return;
    } else if (curNode->building->bCode().code() == code) {
        buildings_ = curNode->next;
        removeAdjacentEdge(code);
        delete curNode;
    } else {
        BuildingNode* curNode_next = curNode->next;
        while (curNode_next != NULL) {
            if (curNode_next->building->bCode().code() == code) {
                curNode->next = curNode_next->next;
                removeAdjacentEdge(code);
                delete curNode_next;
                break;
            }
            curNode = curNode->next;
            curNode_next = curNode_next->next;
        }
    }
}

Building* Graph::findBuilding(string code) const {
    BuildingNode* curNode = buildings_;
    while (curNode != NULL) {
        if (curNode->building->bCode().code() == code) {
            return curNode->building;
        }
        curNode = curNode->next;
    }
    return NULL;
}

BuildingNode* Graph::findBuildingNode (string code) const {
    BuildingNode* curNode = buildings_;
    while (curNode != NULL) {
        if (curNode->building->bCode().code() == code) {
            return curNode;
        }
        curNode = curNode->next;
    }
    return NULL;
}

void Graph::addEdge(string code1, string code2, string connectorType) {
    BuildingConnector* newConnector = new BuildingConnector;
    newConnector->from = findBuildingNode(code1);
    newConnector->to = findBuildingNode(code2);
    newConnector->connectorType = connectorType;
    newConnector->next = connectors_;
    connectors_ = newConnector;
}

void Graph::removeEdge(string code1, string code2) {
    BuildingConnector* curConnector = connectors_;
    if (curConnector == NULL) {
        return;
    } else if (curConnector->from->building->bCode().code() == code1 && curConnector->to->building->bCode().code() == code2) {
        connectors_ = curConnector->next;
        delete curConnector;
    } else {
        BuildingConnector* curConnector_next = curConnector->next;
        while (curConnector_next != NULL) {
            if ((curConnector_next->from->building->bCode().code() == code1 && curConnector_next->to->building->bCode().code() == code2) ||
                (curConnector_next->to->building->bCode().code() == code1 && curConnector_next->from->building->bCode().code() == code2)) {
                curConnector->next = curConnector_next->next;
                delete curConnector_next;
                return;
            }
            curConnector = curConnector->next;
            curConnector_next = curConnector_next->next;
        }
    }
}

void Graph::removeAdjacentEdge(string code) {
    BuildingConnector* previous = NULL;
    BuildingConnector* curConnector = connectors_;
    if(curConnector == NULL) {
        return;
    }
    while (curConnector != NULL && (curConnector->from->building->bCode().code() == code || curConnector->to->building->bCode().code() == code)) {
        previous = curConnector;
        curConnector = curConnector->next;
        connectors_ = curConnector;
        delete previous;
    }

    while (curConnector != NULL) {
        if(curConnector->from->building->bCode().code() == code || curConnector->to->building->bCode().code() == code) {
            previous->next = curConnector->next;
            delete curConnector;
            curConnector = previous->next;
        } else {
            previous = curConnector;
            curConnector = curConnector->next;
        }
    }
}

BuildingConnector* Graph::findBuildingConnector(string code1, string code2) const {
    BuildingConnector* curConnector = connectors_;
    while (curConnector != NULL) {
        if((curConnector->from->building->bCode().code() == code1 && curConnector->to->building->bCode().code() == code2)
           || (curConnector->from->building->bCode().code() == code2 && curConnector->to->building->bCode().code() == code1)) {
            return curConnector;
        }
        curConnector = curConnector->next;
    }
    return NULL;
}

int Graph::countConnector() const {
    int i = 0;
    BuildingConnector* curConnector = new BuildingConnector;
    while(curConnector != NULL) {
        i++;
        curConnector = curConnector->next;
    }
    return i;
}

//===================================================================
// Paths
//===================================================================

struct Paths {
    BuildingNode *path;
    Paths *next;
};

void Graph::printReverse(struct BuildingNode* current, struct BuildingNode* previous) const {
    if(previous == NULL)
        return;

    printReverse(current->next, previous->next);
    BuildingConnector* found = findBuildingConnector(current->building->bCode().code(), previous->building->bCode().code());
    if(found != NULL) {
        cout << " (" << findBuildingConnector(current->building->bCode().code(), previous->building->bCode().code())->connectorType << ") " << current->building->bCode();
    }
}

void Graph::printPaths(string code1, string code2, const bool allpaths) const {
    BuildingNode *path = new BuildingNode;
    path->building = findBuilding(code1);
    path->next = NULL;

    Paths* paths = new Paths;
    paths->path = path;
    paths->next = NULL;

    while(paths != NULL) {
        BuildingNode* tmp_path = paths->path;
        paths = paths->next;

        Building* lastNode = tmp_path->building;

        if(lastNode->bCode().code() == code2) {
            cout << "\t";
            BuildingNode* curTempNode = tmp_path;
            if(curTempNode != NULL) {
                BuildingNode* lastNode = tmp_path;
                while(lastNode->next != NULL) {
                    lastNode = lastNode->next;
                }
                cout << lastNode->building->bCode();
                if(curTempNode->next != NULL) {
                    printReverse(curTempNode, curTempNode->next);
                }
            }
            cout << endl;
            if (!allpaths) {
                cout << endl;
                return;
            }
        }
        BuildingConnector* curConnector = connectors_;
        BuildingNode* linkNodes = NULL;
        while (curConnector != NULL) {
            if (curConnector->from->building->bCode() == lastNode->bCode()) {
                BuildingNode* newBuilding = new BuildingNode;
                newBuilding->building = curConnector->to->building;
                newBuilding->next = linkNodes;
                linkNodes = newBuilding;
            } else if (curConnector->to->building->bCode() == lastNode->bCode()) {
                BuildingNode* newBuilding = new BuildingNode;
                newBuilding->building = curConnector->from->building;
                newBuilding->next = linkNodes;
                linkNodes = newBuilding;
            }
            curConnector = curConnector->next;
        }
        BuildingNode* curLinkNode = linkNodes;
        while (curLinkNode != NULL) {
            BuildingNode* curTempPathNode = tmp_path;
            bool inPath = false;
            while (curTempPathNode != NULL) {
                if (curLinkNode->building->bCode() == curTempPathNode->building->bCode()) {
                    inPath = true;
                    break;
                }
                curTempPathNode = curTempPathNode->next;
            }
            if (!inPath) {
                BuildingNode* newPath = new BuildingNode;
                newPath->building = curLinkNode->building;
                newPath->next = tmp_path;

                Paths* newPaths = new Paths;
                newPaths->path = newPath;
                newPaths->next = paths;
                paths = newPaths;
            }
            curLinkNode = curLinkNode->next;
        }
    }
    cout << endl;
}

void Graph::deleteGraph() {
    while(buildings_ != NULL) {
        BuildingNode* curNode = buildings_;
        buildings_ = buildings_->next;
        delete curNode;
    }
    while(connectors_ != NULL) {
        BuildingConnector* curConnector = connectors_;
        connectors_ = connectors_->next;
        delete curConnector;
    }
}

// streaming operator
ostream& operator<< ( ostream &sout, const Graph &g ) {
    BuildingNode* curNode = g.buildings_;
    while(curNode != NULL) {
        sout << *(curNode->building);
        BuildingConnector* curConnector = g.connectors_;
        bool firstConnector = false;
        while(curConnector != NULL) {
            if (curConnector->from->building->bCode() == curNode->building->bCode()) {
                if (!firstConnector) {
                    sout << "\tConnects to: " << curConnector->to->building->bCode() << " (" << curConnector->connectorType << ")";
                } else {
                    sout << ", " << curConnector->to->building->bCode() << " (" << curConnector->connectorType << ")";
                }
                firstConnector = true;
            } else if (curConnector->to->building->bCode() == curNode->building->bCode()) {
                if (!firstConnector) {
                    sout << "\tConnects to: " << curConnector->from->building->bCode() << " (" << curConnector->connectorType << ")";
                } else {
                    sout << ", " << curConnector->from->building->bCode() << " (" << curConnector->connectorType << ")";
                }
                firstConnector = true;
            }
            curConnector = curConnector->next;
        }
        if(firstConnector) {
            sout << endl;
        }
        sout << endl;
        curNode = curNode->next;
    }

    return sout;
}

Graph& Graph::operator=(const Graph &target) {
    if(this == &target)
        return *this;

    BuildingNode* curNode = target.buildings_;
    while(curNode != NULL) {
        addNode(curNode->building);
        curNode = curNode->next;
    }

    BuildingConnector* curConnector = target.connectors_;
    while(curConnector != NULL) {
        addEdge(curConnector->from->building->bCode().code(), curConnector->to->building->bCode().code(), curConnector->connectorType);
        curConnector = curConnector->next;
    }

    return *this;
}

bool Graph::operator==(const Graph &graph) const {
    BuildingNode* graphOneCurNode = buildings_;
    BuildingNode* graphTwoCurNode = graph.buildings_;

    while (graphOneCurNode != NULL && graphTwoCurNode != NULL) {
        if (graphOneCurNode->building->bCode() != graphTwoCurNode->building->bCode()) {
            return false;
        }
        graphOneCurNode = graphOneCurNode->next;
        graphTwoCurNode = graphTwoCurNode->next;
    }
    if (graphOneCurNode != NULL || graphTwoCurNode != NULL) {
        return false;
    }

    if(countConnector() != graph.countConnector()) {
        return false;
    }

    BuildingConnector* graphOneCurConnector = connectors_;
    while (graphOneCurConnector != NULL) {
        BuildingConnector* graphTwoCurConnector = graph.connectors_;
        bool found = false;
        while(graphTwoCurConnector != NULL) {
            if (((graphTwoCurConnector->from->building->bCode() == graphOneCurConnector->from->building->bCode() && graphTwoCurConnector->to->building->bCode() == graphOneCurConnector->to->building->bCode())
                 || (graphTwoCurConnector->to->building->bCode() == graphOneCurConnector->from->building->bCode() && graphTwoCurConnector->from->building->bCode() == graphOneCurConnector->to->building->bCode()))
                && graphOneCurConnector->connectorType == graphTwoCurConnector->connectorType) {
                found = true;
                break;
            }
            graphTwoCurConnector = graphTwoCurConnector->next;
        }
        if(!found) {
            return false;
        }
        graphOneCurConnector = graphOneCurConnector->next;
    }
    return true;
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
    cout << map1;

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

                // print the current map to the console
            case print: {
                cout << *map;
                break;
            }

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
                string junk;
                getline ( cin, junk );
                break;
            }


                // delete the entire graph (no memory leak).  There is no change to the collection of Buildings.
            case delGraph: {
                map->deleteGraph();
                break;
            }

                // remove an existing edge from the current map
            case remEdge: {
                string code1, code2;
                cin >> code1 >> code2;
                map->removeEdge( code1, code2 );
                string junk;
                getline ( cin, junk );
                break;
            }

                // remove an existing node from the current map.  There is no change to the collection of Buildings.
            case remNode: {
                string code;
                cin >> code;
                map->removeNode( code );
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
                string junk;
                getline ( cin, junk );
                break;
            }

                // check whether map1 is equal to map2
            case eq: {
                if ( map1 == map2 ) {
                    cout << "Maps 1 and 2 are equal." << endl;
                }
                else {
                    cout << "Maps 1 and 2 are NOT equal." << endl;
                }
                break;
            }

                // graph copy constructor
            case copyGraph: {
                Graph map3( *map );
                cout << map3;
                string junk;
                getline( cin, junk );
                break;
            }

                // graph assignment operator
            case assignGraph: {
                map1 = map2;
                cout << map1;
                break;
            }

                // find path(s) in graph from one building to another building
            case path: {
                string code1, code2, all;
                cin >> code1 >> code2 >> all;
                cout << "Paths from " << code1 << " to " << code2 << " are: " << endl;
                bool printall = ( all.length() > 0 && all.at(0) == 't' ) ? true : false;
                map->printPaths( code1, code2, printall );
                string junk;
                getline( cin, junk );
                break;
            }

            default: {
                cerr << "Invalid command." << endl;
            }
        }

        cout << "Command: ";
        cin >> command;
        op = convertOp( command );

    } // while cin OK

}