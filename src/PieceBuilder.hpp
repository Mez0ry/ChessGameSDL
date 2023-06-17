#ifndef __PIECE_BUILDER_HPP__
#define __PIECE_BUILDER_HPP__
#include "Piece.hpp"
#include <string>

class PieceBuilder {
private:

public:
  PieceBuilder() = default;
  ~PieceBuilder() = default;
  
  PieceBuilder& LoadTexture(const Base::Ref<Renderer> renderer,const std::string& path){
    m_piece.GetTexture().LoadTexture(renderer,path);
    return (*this);
  }

  PieceBuilder& ShareSDLTexture(const Texture& texture){
    m_piece.GetTexture().ShareSDLTexture(texture);
    return (*this);
  }

  PieceBuilder& SetPos(const Vec2& pos){
    m_piece.SetPosition(pos);
    return (*this);
  }

  PieceBuilder& SetSize(const ObjectSize& size){
    m_piece.SetSize(size);
    return (*this);
  }
  
  PieceBuilder& SetPieceType(Piece::PieceType type){
    m_piece.m_Type = type;
    return (*this);
  }

  PieceBuilder& SetTeam(Piece::Team team){
    m_piece.m_Team = team;
    return (*this);
  }
  
  PieceBuilder& SetSourceTexture(const Vec2& source_texture_pos,const ObjectSize &size){
    m_piece.GetTexture().SetSize<SourceRect>(size);
    m_piece.GetTexture().SetPosition<SourceRect>(source_texture_pos);
    return (*this);
  }

  [[nodiscard]] Base::Ref<Piece> build(){
    auto res = Base::CreateRef<Piece>(std::move(m_piece));
    m_piece = Piece();
    
    return res;
  }
  
private:
  Piece m_piece;
};
#endif //!__PIECE_BUILDER_HPP__