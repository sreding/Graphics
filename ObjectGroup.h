#ifndef OBJECTGROUP_H
#define OBJECTGROUP_H

#include "ObjModel.h"

class ObjectGroup
{
public:
    ObjectGroup(const std::string &_path);
    std::vector<ObjModel> objects;
    void add(ObjModel object);
};

#endif // OBJECTGROUP_H
