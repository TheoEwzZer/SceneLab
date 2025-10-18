#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "GeometryGenerator.hpp"

#include <string>

class OBJLoader {
public:
    static GData loadOBJ(
        const std::string &filename, const std::string &filepath);
};

#endif /* OBJLOADER_H */
