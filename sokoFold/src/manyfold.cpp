#include <manyfold.h>
namespace manyfold
{
    // overload ostream
    std::ostream &operator<<(std::ostream &os, const manyfold &manyfold)
    {
        for (auto &&i : manyfold.vertexs)
        {
            os << std::format("v {:>3}, {:>7.3f}, {:>7.3f}, {:>7.3f}\n", manyfold.vids.find(i.get())->second, (*i).coord[0], (*i).coord[1], (*i).coord[2]);
        }
        for (auto &&i : manyfold.faces)
        {
            halfEdge *p;
            os << std::format("f {:>3}, ", manyfold.fids.find(i.get())->second);
            for (p = i.get()->he; p->next != i.get()->he; p = p->next)
            {
                os << std::format("{:>3}, ", manyfold.vids.find(p->vert)->second);
            }
            os << std::format("{:>3}\n", manyfold.vids.find(p->vert)->second);
        }
        return os;
    }
    // overload ofstream
    std::ofstream &operator<<(std::ofstream &outFile, manyfold &manyfold)
    {
        ssize_t cnt;
        cnt = 1;
        for (auto &&i : manyfold.vertexs)
        {
            manyfold.vids[i.get()] = cnt;
            manyfold.vptr[cnt] = i.get();
            cnt++;
        }
        cnt = 1;
        for (auto &&i : manyfold.faces)
        {
            manyfold.fids[i.get()] = cnt;
            manyfold.fptr[cnt] = i.get();
            cnt++;
        }
        for (size_t i = 1; i <= manyfold.vertexs.size(); i++)
        {
            double(&cord)[3] = manyfold.vptr.find(i)->second->coord;
            outFile << std::format("v {:.6f} {:.6f} {:.6f}\n", cord[0], cord[1], cord[2]);
        }
        for (size_t i = 1; i <= manyfold.faces.size(); i++)
        {
            halfEdge *p;
            face *f = manyfold.fptr.find(i)->second;
            outFile << std::format("f ");
            for (p = f->he; p->next != f->he; p = p->next)
            {
                outFile << std::format("{} ", manyfold.vids.find(p->vert)->second);
            }
            outFile << std::format("{}\n", manyfold.vids.find(p->vert)->second);
        }
        return outFile;
    }
    // overload ifstream
    std::ifstream &operator>>(std::ifstream &inFile, manyfold &manyfold)
    {
        std::string line;                                          // input line
        std::map<std::pair<ssize_t, ssize_t>, halfEdge *> edgeMap; // edgeMap:(vid,vid)-->edgePtr
        manyfold.clear();                                          // clear all the data
        while (std::getline(inFile, line))
        {
            if (line.empty())
                continue;
            if (line[0] == 'v' && line[1] == ' ') // vertex
            {
                std::istringstream iss(line); // read string stream
                char vType;
                double x, y, z;
                iss >> vType >> x >> y >> z;

                // create vert
                std::unique_ptr<vertex> vert = std::make_unique<vertex>();
                vert->coord[0] = x;
                vert->coord[1] = y;
                vert->coord[2] = z;
                vert->he = nullptr;
                manyfold.vertexs.push_back(std::move(vert));
                manyfold.vptr[manyfold.vertexs.size()] = manyfold.vertexs.back().get();
                manyfold.vids[manyfold.vertexs.back().get()] = manyfold.vertexs.size();
            }
            else if (line[0] == 'f')
            {
                // NOTE!!! this algorithm require the object is a "orientable manifold"

                std::istringstream iss(line); // read string stream
                char fType;
                int vIndex[4]; // assum it's quad
                iss >> fType >> vIndex[0] >> vIndex[1] >> vIndex[2] >> vIndex[3];

                // create face
                std::unique_ptr<face> facePtr = std::make_unique<face>();

                // create halfEdge
                halfEdge *prevEdge = nullptr, *firstEdge = nullptr;
                for (size_t i = 0; i < 4; i++)
                {
                    std::unique_ptr<halfEdge> edge = std::make_unique<halfEdge>();
                    // face & vert
                    edge.get()->vert = manyfold.vptr[vIndex[i]];
                    edge.get()->face = facePtr.get();
                    // next
                    if (prevEdge)
                        prevEdge->next = edge.get();
                    else
                        firstEdge = edge.get();
                    // pair
                    ssize_t nextIndex = vIndex[(i + 1) % 4]; // mod index
                    std::pair<ssize_t, ssize_t> pairKey = std::make_pair(nextIndex, vIndex[i]);
                    if (edgeMap.count(pairKey))
                    {
                        edge->pair = edgeMap[pairKey];
                        edgeMap[pairKey]->pair = edge.get();
                    }
                    else
                    {
                        edgeMap[std::make_pair(vIndex[i], nextIndex)] = edge.get();
                    }
                    // pushback and maintainance
                    manyfold.halfEdges.push_back(std::move(edge));
                    prevEdge = manyfold.halfEdges.back().get();
                }
                // final
                prevEdge->next = firstEdge;
                facePtr->he = firstEdge;
                // push face
                manyfold.faces.push_back(std::move(facePtr));
                manyfold.fptr[manyfold.faces.size()] = manyfold.faces.back().get();
                manyfold.fids[manyfold.faces.back().get()] = manyfold.faces.size();
            }
        }
        return inFile;
    }
    // clear all the data
    void manyfold::clear()
    {
        vertexs.clear();
        faces.clear();
        halfEdges.clear();

        vids.clear();
        vptr.clear();
        fids.clear();
        fptr.clear();
    }
} // namespace manyfold
