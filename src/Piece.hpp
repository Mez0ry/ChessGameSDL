#ifndef __PIECE_HPP__
#define __PIECE_HPP__
#include "Base.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Vector.hpp"

class PieceBuilder;

class Piece : virtual public Entity {
public:
  enum class PieceType {
    PAWN = 'p',
    ROOK = 'r',
    KNIGHT = 'n',
    BISHOP = 'b',
    QUEEN = 'q',
    KING = 'k',
    UNKNOWN
  };

  enum class Team { WHITE, BLACK, UNKNOWN };

private:
  friend PieceBuilder;

public:
  Piece();
  Piece(const Piece& other){
    m_Type = other.m_Type;
    m_Team = other.m_Team;
    m_Position = other.m_Position;
    m_Texture = other.m_Texture;
  }

  Piece(Piece&& other){
    m_Type = std::move(other.m_Type);
    m_Team = other.m_Team;
    other.m_Team = Team::UNKNOWN;
    m_Position = std::move(other.m_Position);
    m_Texture = std::move(other.m_Texture);
  }

  ~Piece();

  Piece& operator=(Piece&&) = default;
  Piece& operator=(const Piece&) = default;

  Texture &GetTexture() override { return m_Texture; }

  void SetPosition(const Vec2 &pos) override { m_Position = pos; }
  Vec2 &GetPosition() override { return m_Position; }
  const Vec2 &GetPosition() const override { return m_Position; }

  void SetSize(const ObjectSize &size) override { m_Texture.SetSize(size); }
  const ObjectSize GetSize() const override { return m_Texture.GetSize(); }
  
  Team GetTeam() const {return m_Team;}
  PieceType GetPieceType() const {return m_Type;}
public:
  std::vector<Vec2>& GetPseudoLegalMoves(){
    return m_PseudoLegalMoves;
  }

  std::vector<Vec2>& GetLegalMoves(){
    return m_LegalMoves;
  }

  std::vector<Vec2>& GetAttackMoves(){
    return m_AttackMoves;
  }

  std::vector<Vec2>& GetDefendingMoves(){
    return m_DefendingMoves;
  }
  
private:
  PieceType m_Type = PieceType::UNKNOWN;
  Team m_Team = Team::UNKNOWN;
  Vec2 m_Position; // position on map
  Texture m_Texture;
private:
  std::vector<Vec2> m_PseudoLegalMoves;
  std::vector<Vec2> m_LegalMoves;
  std::vector<Vec2> m_AttackMoves; // piece attacking piece from opposite team
  std::vector<Vec2> m_DefendingMoves; // piece attacking piece from its own team
};
#endif //! __PIECE_HPP__