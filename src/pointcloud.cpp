#include "pointcloud.h"
#include <fstream>
#include <sstream>
#include "weightedrandom.h"
#include <iostream>
#include <glm/glm.hpp>

PointCloud::PointCloud(const std::string& filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::string shader = "";
    while (getline(stream, line))
    {
        if (line.size())
        {
            std::istringstream ss(line);
            std::string pos;
            ss >> pos;

            if (pos == "v")
            {
                for (int i = 0; i < 3; i++)
                {
                    ss >> pos;
                    vertices.push_back(std::stof(pos));
                }
            }
        }
    }

    stream.close();
}

PointCloud::PointCloud(const Mesh& mesh, const unsigned int n)
{
    std::vector<double> areas = {};
    vertices = {};
    
    for (unsigned int i = 0; i < mesh.indices.size(); i+=3)
    {
        glm::vec3 v1 = glm::vec3(
            mesh.vertices[3 * mesh.indices[i]],
            mesh.vertices[3 * mesh.indices[i] + 1],
            mesh.vertices[3 * mesh.indices[i] + 2]
        );

        glm::vec3 v2 = glm::vec3(
            mesh.vertices[3 * mesh.indices[i + 1]],
            mesh.vertices[3 * mesh.indices[i + 1] + 1],
            mesh.vertices[3 * mesh.indices[i + 1] + 2]
        );

        glm::vec3 v3 = glm::vec3(
            mesh.vertices[3 * mesh.indices[i + 2]],
            mesh.vertices[3 * mesh.indices[i + 2] + 1],
            mesh.vertices[3 * mesh.indices[i + 2] + 2]
        );

        double a = area(v1, v2, v3);
        areas.push_back(a);
    }
    
    WeightedRandom random(areas);
    for (int i = 0; i < n; i++)
    {
        int tri = random.sample() * 3;
        
        float u;
        float v;
        do {
            u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        } while (u + v > 1.0f);

        glm::vec3 v1 = glm::vec3(
            mesh.vertices[3 * mesh.indices[tri]],
            mesh.vertices[3 * mesh.indices[tri] + 1],
            mesh.vertices[3 * mesh.indices[tri] + 2]
        );

        glm::vec3 v2 = glm::vec3(
            mesh.vertices[3 * mesh.indices[tri + 1]],
            mesh.vertices[3 * mesh.indices[tri + 1] + 1],
            mesh.vertices[3 * mesh.indices[tri + 1] + 2]
        );

        glm::vec3 v3 = glm::vec3(
            mesh.vertices[3 * mesh.indices[tri + 2]],
            mesh.vertices[3 * mesh.indices[tri + 2] + 1],
            mesh.vertices[3 * mesh.indices[tri + 2] + 2]
        );

        glm::vec3 point = v1 + (u * (v2 - v1)) + (v * (v3 - v1));

        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);
    }
}

double PointCloud::area(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    return 0.5 * glm::length(glm::cross(v2 - v1, v3 - v1));
}