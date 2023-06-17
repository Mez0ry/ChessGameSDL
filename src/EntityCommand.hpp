#ifndef __ENTITY_COMMAND_HPP__
#define __ENTITY_COMMAND_HPP__
#include "Entity.hpp"
#include "Base.hpp"

class EntityCommand
{
public:
    virtual ~EntityCommand() {}

    virtual void Execute(Base::Ref<Entity> entity) = 0;
    virtual void Undo() {}
};
#endif //! __ENTITY_COMMAND_HPP__