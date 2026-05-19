#pragma once

#include "collections/spatial/QuadTree.hpp"
#include "game/PCManager.hpp"
#include <glm/glm.hpp>

#include <tuple>

struct PCPosition;
struct PCAcceleration;

struct BarnesHutQT : public QuadTree<u32, glm::dvec2, double, glm::dvec2>
{
    using QT = QuadTree<u32, glm::dvec2, double, glm::dvec2>;

    static constexpr double THETA = 2.05;
    static constexpr double SOFTENING = 0.01;
    static constexpr double G = 6.67428e-11;

    struct NodeBarnes
    {
        glm::dvec2 com {0.0};
        double total_mass  = 0.0;
    };

    BarnesHutQT()
    {
        nodes_barnes_.resize(1); // Setting root
    }

    void insertBodyInto(
        u32 ni,
        u32 object_id,
        const glm::dvec2& pos,
        double mass);

    void computeForce(
        u32 rootNi, 
        u32 self_id, 
        const glm::dvec2& pos, 
              glm::dvec2& acc, 
        double mass);
        
    std::vector<NodeBarnes> nodes_barnes_;
private:
    
}; 