#pragma once
/*
#include "ComponentManager.hpp"
#include "QuadTree.hpp"
#include <glm/glm.hpp>

struct Body
{ 
    glm::dvec2 pos {0.0};
    glm::dvec2 vel {0.0};
    glm::dvec2 acc {0.0};
    double mass = 0.0;
};

struct QTBarnesHut : public QuadTree<
    u32, 
    glm::dvec2, 
    double, 
    glm::dvec2>
{
    static constexpr double THETA = 0.5;
    static constexpr double SOFTENING = 0.01;
    static constexpr double G = 1.0;

    struct NodeBarnes
    {
        glm::dvec2 com {0.0};
        double total_mass  = 0.0;
    };

    std::vector<NodeBarnes> nodes_barnes_;
    std::vector<Body>& bodies;

    QTBarnesHut(std::vector<Body>& bodies) : bodies(bodies)
    {
        nodes_barnes_.resize(1);
    }

    void computeForce(u32 index)
    { 
        computeForceRecursive(0, index);
    }

    void insertBody(u32 index, glm::dvec2 pos)
    {
        insertBodyInto(0, {index, pos});
    }

    void buildBarnesData()
    {
        nodes_barnes_.assign(nodes_.size(), {});
    }
    
    void clearBarnes()
    {
        clear();
        nodes_barnes_.assign(nodes_.size(), {});
    }
protected:
    void insertBodyInto(u32 ni, const Item& item)
    {
        const Body& b = bodies[item.id];
        while (true)
        {
            NodeBarnes& nodeBarnes = nodes_barnes_[ni];
            Node& node = nodes_[ni];

            const bool isLeaf = node.isLeaf();
            const bool maxed  = node.depth >= static_cast<u8>(maxDepth);
            const bool canFit = node.itemCount < static_cast<u16>(nodeCapacity);
 
            if (isLeaf)
            {
                if (canFit)
                {
                    poolPush(ni, item);
                    return;
                }
 
                if (maxed)
                {
                    if (!node.usesHeap())
                        promoteToHeap(ni);

                    heap_[node.heapIndex()].push_back(item);
                    ++node.itemCount;
                    return;
                }

                if(node.itemCount == 1)
                {
                    nodeBarnes.com        = b.pos;
                    nodeBarnes.total_mass = b.mass; 
                }
                else 
                {
                    const double newMass = nodeBarnes.total_mass + b.mass;
                    nodeBarnes.com = (nodeBarnes.com * nodeBarnes.total_mass + b.pos * b.mass) / newMass;
                    nodeBarnes.total_mass = newMass;
                }
                splitNode(ni);
                nodes_barnes_.resize(nodes_barnes_.size() + 4);
            }
            
            const glm::dvec2 c = nodes_[ni].bounds.center();
            const int  q = (item.primitive.x >= c.x ? 1 : 0) | (item.primitive.y >= c.y ? 2 : 0);
            ni = nodes_[ni].firstChild + static_cast<u32>(q);
        }
    }

    void computeForceRecursive(u32 ni, u32 index)
    {
        const NodeBarnes& nodeBarnes = nodes_barnes_[ni];
        const Node& node = nodes_[ni];
        Body& b = bodies[index];

        if (nodeBarnes.total_mass == 0.0) return;

        glm::dvec2 delta = nodeBarnes.com - b.pos;
        double dist2 = delta.x*delta.x + delta.y*delta.y + SOFTENING*SOFTENING;

        if (node.firstChild == NULL_NODE) return;
        else
        {
            double s = node.bounds.maxX - node.bounds.minX;
            if ((s * s) / dist2 >= THETA * THETA) {
                for (int i = 0; i < 4; ++i) computeForceRecursive(node.firstChild + i, index);
                return;
            }
        }

        double dist = std::sqrt(dist2);
        double F = nodeBarnes.total_mass / (dist2 * dist);
        b.acc += F * delta;
    }
    
};

*/