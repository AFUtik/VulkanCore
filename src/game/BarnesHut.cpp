#include "game/BarnesHut.hpp"

#include <iostream>

void BarnesHutQT::insertBodyInto(
    u32 ni, 
    u32 object_id,
    const glm::dvec2& pos,
    double mass
)
{
    const PCM::Object obj(object_id);
    const QT::Item item = {object_id, pos};

    while (true)
    {
        NodeBarnes&      nodeBarnes = nodes_barnes_[ni];
        typename QT::Node& node     = this->nodes_[ni];

        if (nodeBarnes.total_mass == 0.0) {
            nodeBarnes.com        = pos;
            nodeBarnes.total_mass = mass;
        } else {
            const double newMass  = nodeBarnes.total_mass + mass;
            nodeBarnes.com        = (nodeBarnes.com * nodeBarnes.total_mass
                                    + pos * mass) / newMass;
            nodeBarnes.total_mass = newMass;
        }

        const bool isLeaf = node.isLeaf();
        const bool maxed  = node.depth >= static_cast<u8>(this->maxDepth);
        const bool canFit = node.itemCount < static_cast<u16>(this->nodeCapacity);

        if (isLeaf)
        {
            if (canFit) {
                poolPush(ni, item);
                return;
            }

            if (maxed) {
                if (!node.usesHeap()) promoteToHeap(ni);
                this->heap_[node.heapIndex()].push_back(item);
                ++node.itemCount;
                return;
            }

            splitNode(ni);
            nodes_barnes_.resize(nodes_barnes_.size() + 4);
        }

        const glm::dvec2 c = this->nodes_[ni].bounds.center();
        const int q = (pos.x >= c.x ? 1 : 0) | (pos.y >= c.y ? 2 : 0);
        ni = this->nodes_[ni].firstChild + static_cast<u32>(q);
    }
}

void BarnesHutQT::computeForce(
    u32 rootNi, 
    u32 self_id, 
    const glm::dvec2& pos, 
          glm::dvec2& acc, 
    double mass)
{
    static constexpr double SOFT2  = SOFTENING * SOFTENING;
    static constexpr double THETA2 = THETA * THETA;

    u32 stack[128];
    int top = 0;
    stack[top++] = rootNi;

    while (top > 0)
    {
        const u32 ni = stack[--top];

        const NodeBarnes& nb = nodes_barnes_[ni];
        if (nb.total_mass == 0.0) continue;

        const Node& node = nodes_[ni];

        const glm::dvec2 delta = nb.com - pos;
        const double dist2 = delta.x*delta.x + delta.y*delta.y + SOFT2;

        if (node.firstChild == NULL_NODE)
        {
            for (const auto& it : getItems(node))
            {
                if (it.id == self_id) [[unlikely]] continue;

                const glm::dvec2 d  = it.primitive - pos;
                const double     d2 = d.x*d.x + d.y*d.y + SOFT2;

                const double inv_dist = 1.0 / std::sqrt(d2);
                acc += (mass * inv_dist * inv_dist * inv_dist) * d;
            }
            continue;
        }

        const double s = node.bounds.maxX - node.bounds.minX;
        if (s * s < THETA2 * dist2) 
        {
            const double inv_dist = 1.0 / std::sqrt(dist2);
            acc += (nb.total_mass * inv_dist * inv_dist * inv_dist) * delta;
        }
        else
        {
            const u32 fc = node.firstChild;
            stack[top++] = fc;
            stack[top++] = fc + 1;
            stack[top++] = fc + 2;
            stack[top++] = fc + 3;
        }
    }
}