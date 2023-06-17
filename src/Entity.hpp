#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

class Texture;
struct Vec2;
struct ObjectSize;

class Entity {
public:
  virtual ~Entity() {}
  
  virtual Texture& GetTexture() = 0;

  virtual void SetPosition(const Vec2 &pos) = 0;
  virtual Vec2& GetPosition() = 0;
  virtual const Vec2& GetPosition() const = 0;

  virtual void SetSize(const ObjectSize &size) = 0;
  virtual const ObjectSize GetSize() const = 0;

  void SetDragging(bool status){m_IsDragging = status;}
  bool IsDragging() const {return m_IsDragging;}
protected:
  bool m_IsDragging;
};
#endif //! __ENTITY_HPP__