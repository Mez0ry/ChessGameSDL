#ifndef __DRAG_ENTITY_HPP__
#define __DRAG_ENTITY_HPP__
#include "EntityCommand.hpp"
#include "Vector.hpp"
#include "Texture.hpp"

class DragEntity : public EntityCommand
{
public:
    DragEntity(const Vec2 &mouse_pos, Base::Ref<Renderer> renderer) : m_MousePos(mouse_pos) ,m_Renderer(renderer){}
    ~DragEntity() = default;

    void Execute(Base::Ref<Entity> entity) override{
        auto size = entity->GetTexture().GetSize();
        entity->GetTexture().SetPosition(Vec2(m_MousePos.x - (size.GetWidth() / 2), m_MousePos.y - (size.GetHeight() / 2)));
        m_Renderer->Render(entity->GetTexture());
    } 
private:
    Vec2 m_MousePos;
    Base::Ref<Renderer> m_Renderer;
};
#endif //!__DRAG_ENTITY_HPP__