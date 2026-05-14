#include "game/BarnesHut.hpp"
#include "game/PCManager.hpp"

#include <iostream>

static PlanetComponentManager& pcm = PlanetComponentManager::instance();

void BarnesHutQT::step(double dt)
{
    // 1) half-step velocities using previous acceleration
    for (auto [vel_c, acc_c] : PCM::View<PCVelocity, PCAcceleration>(&pcm))
        vel_c.velocity += acc_c.acceleration * (0.5 * dt);

    // 2) full-step positions
    for (auto [pos_c, vel_c] : PCM::View<PCPosition, PCVelocity>(&pcm))
        pos_c.position += vel_c.velocity * dt;

    // 3) rebuild quadtree for updated positions
    this->clear();
    nodes_barnes_.resize(1);

    for (auto& object : pcm.get_objects())
        insertBodyInto(0, object.get_id());

    // 4) recompute accelerations from the new tree
    for (auto [acc_c] : PCM::View<PCAcceleration>(&pcm))
        acc_c.acceleration = glm::dvec2(0.0, 0.0);

    for (auto& object : pcm.get_objects())
    {
        const u32 id = object.get_id();

        auto& pos = pcm.get_component<PCPosition>(PCM::Object(id));
        auto& acc = pcm.get_component<PCAcceleration>(PCM::Object(id));

        auto body = std::tie(pos, acc);
        computeForceRecursive(0, id, body);
    }

    // 5) second half-step velocities using new acceleration
    for (auto [vel_c, acc_c] : PCM::View<PCVelocity, PCAcceleration>(&pcm))
        vel_c.velocity += acc_c.acceleration * (0.5 * dt);
}

void BarnesHutQT::insertBodyInto(u32 ni, u32 object_id)
{
    const PCM::Object obj(object_id);
    const auto& pos  = pcm.get_component<PCPosition>  (obj).position;
    double      mass = pcm.get_component<PCProperties>(obj).mass;
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

void BarnesHutQT::computeForceRecursive(
    u32 ni,
    u32 self_id,
    std::tuple<PCPosition&, PCAcceleration&>& body)
{
    const NodeBarnes& nodeBarnes = nodes_barnes_[ni];
    const Node&       node       = nodes_[ni];

    if (nodeBarnes.total_mass == 0.0) return;

    const glm::dvec2& self_pos = std::get<PCPosition&>(body).position;
    const glm::dvec2  delta    = nodeBarnes.com - self_pos;
    const double dist2 = delta.x*delta.x + delta.y*delta.y + SOFTENING*SOFTENING;

    if (node.firstChild == NULL_NODE)
    {
        for(auto& it : getItems(node)) {
            if (it.id == self_id) continue;

            const glm::dvec2 d = it.primitive - self_pos;
            const double d2    = d.x*d.x + d.y*d.y + SOFTENING*SOFTENING;
            const double m     = pcm.get_component<PCProperties>(PCM::Object(it.id)).mass;
            const double dist  = std::sqrt(d2);
            std::get<PCAcceleration&>(body).acceleration += (m / (d2 * dist)) * d;
        };
        return;
    }

    const double s = node.bounds.maxX - node.bounds.minX;
    if ((s * s) / dist2 < THETA * THETA)
    {
        const double dist = std::sqrt(dist2);
        const double F    = nodeBarnes.total_mass / (dist2 * dist);
        std::get<PCAcceleration&>(body).acceleration += F * delta;
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            computeForceRecursive(node.firstChild + i, self_id, body);
    }
}