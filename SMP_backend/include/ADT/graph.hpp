#pragma once

#include "hash_map.hpp"
#include "set.hpp"
#include <vector>

using NodeID = unsigned long long;

class Graph
{
private:
    HashMap<NodeID, Set<NodeID>> outAdj;
    HashMap<NodeID, Set<NodeID>> inAdj;

public:
    Graph() {}
    Graph(size_t reserveNodes)
    {
        outAdj.reserve(reserveNodes);
        inAdj.reserve(reserveNodes);
    }

    bool addEdge(NodeID from, NodeID to)
    {
        if (!hasNode(from) || !hasNode(to))
        {
            return false;
        }

        bool changed1 = outAdj[from].insert(to);
        bool changed2 = inAdj[to].insert(from);

        return changed1 || changed2;
    }

    bool removeEdge(NodeID from, NodeID to)
    {
        if (!hasNode(from) || !hasNode(to))
        {
            return false;
        }

        bool changed1 = outAdj[from].erase(to);
        bool changed2 = inAdj[to].erase(from);

        return changed1 || changed2;
    }

    bool hasEdge(NodeID from, NodeID to) const
    {
        if (!outAdj.contains(from))
            return false;

        return outAdj.get(from)->contains(to);
    }

    bool hasNode(NodeID id)
    {
        return outAdj.contains(id) || inAdj.contains(id);
    }

    bool addNode(NodeID id)
    {
        bool created = false;

        if (!outAdj.contains(id))
        {
            outAdj[id];
            created = true;
        }
        if (!inAdj.contains(id))
        {
            inAdj[id];
            created = true;
        }
        return created;
    }

    const Set<NodeID> *outNeighbors(NodeID from) const
    {
        if (!outAdj.contains(from))
            return nullptr;

        return outAdj.get(from);
    }

    const Set<NodeID> *inNeighbors(NodeID to) const
    {
        if (!inAdj.contains(to))
            return nullptr;

        return inAdj.get(to);
    }

    size_t outDegree(NodeID from) const
    {
        const Set<NodeID> *s = outAdj.get(from);
        if (!s)
            return 0;
        return s->size();
    }

    size_t inDegree(NodeID to) const
    {
        const Set<NodeID> *s = inAdj.get(to);
        if (!s)
            return 0;
        return s->size();
    }

    void clear()
    {
        outAdj.clear();
        inAdj.clear();
    }

    void reserve(size_t sz)
    {
        outAdj.reserve(sz);
        inAdj.reserve(sz);
    }
};