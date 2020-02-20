#include "mesh.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "marchingcubes.h"

Mesh::Mesh(const std::string& filepath)
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

            if (pos == "f")
            {
                for (int i = 0; i < 3; i++)
                {
                    ss >> pos;
                    indices.push_back(std::stoul(pos) - 1);
                }
            }
        }
    }

    stream.close();
}

Mesh::Mesh()
{
    // do nothing
}