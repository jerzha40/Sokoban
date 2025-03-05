#include <list>
#include <memory>
#include <unordered_map>
#include <format>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

namespace manyfold
{
    struct face;
    struct halfEdge;
    struct vertex;
    struct halfEdge
    {                   //*face1==he->face;              face1
        vertex *vert;   //*v1==he->vert;             v1-------->v2------->v3
        halfEdge *pair; //*v2==he->pair->vert;       v1<--------v2
        halfEdge *next; //                               face2      next
        face *face;     //*face2==he->pair->face;
    };
    struct face
    {
        halfEdge *he;
    };
    struct vertex
    {
        double coord[3];
        halfEdge *he;
    };

    class manyfold
    {
    private:
        std::list<std::unique_ptr<halfEdge>> halfEdges; // half edges list[ptr]
        std::list<std::unique_ptr<face>> faces;         // faces list[ptr]
        std::list<std::unique_ptr<vertex>> vertexs;     // vertexs list[ptr]

        std::unordered_map<ssize_t, vertex *> vptr; // vertex ptr
        std::unordered_map<vertex *, ssize_t> vids; // vertex id
        std::unordered_map<ssize_t, face *> fptr;   // face ptr:    id-->ptr
        std::unordered_map<face *, ssize_t> fids;   // face id:     ptr-->id

    public:
        manyfold(/* args */) {}
        ~manyfold() {}
        friend std::ostream &operator<<(std::ostream &os, const manyfold &manyfold);
        friend std::ofstream &operator<<(std::ofstream &outFile, manyfold &manyfold);
        friend std::ifstream &operator>>(std::ifstream &inFile, manyfold &manyfold);
        void clear();
    };
} // namespace manyfold
