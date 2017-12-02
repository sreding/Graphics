#include "ObjectGroup.h"
#include "objloader.hpp"

ObjectGroup::ObjectGroup(const std::string &_path)
{

}
void ObjectGroup::add(ObjModel obj){
    objects.push_back(obj);
}
