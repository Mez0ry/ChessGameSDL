#ifndef __PIECE_HPP__
#define __PIECE_HPP__
#include "Base.hpp"
#include "Entity.hpp"
#include "Texture.hpp"
#include "Vector.hpp"
#include "Move.hpp"
#include <variant>

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

  enum class SpecialMoveType : uint8_t{
    ENPASSANT,CASTLE,PROMOTION
  };

  struct SpecialMove{
    struct EnPassant{
      Move move;
    };

    struct Castle{
      Vec2 castleMove;
      Move rookMove;
      Move kingMove;
    };

    struct Promotion{
      Piece::PieceType promoteFrom;
      Piece::PieceType promoteTo;
      Move move;
    };

    SpecialMoveType type;
    std::variant<EnPassant,Castle,Promotion> variant;
  };

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
  void SetPieceType(PieceType type) {m_Type = type;}
  PieceType GetPieceType() const {return m_Type;}

  bool IsInactive() const {return m_IsInactive;}
  void SetInactive(bool status) {m_IsInactive = status;}
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

  SpecialMove* GetSpecialMoveIf(const std::function<bool(const SpecialMove&)> function){
    auto it = std::find_if(m_SpecialMoves.begin(),m_SpecialMoves.end(),function);
    if(it != m_SpecialMoves.end()){
      return &(*it);
    }
    return nullptr;
  }
  
  std::vector<SpecialMove>& GetSpecialMoves(){
    return m_SpecialMoves;
  }

  bool IsPseudoLegalMove(const Vec2& move) const{
    if(m_PseudoLegalMoves.empty()) return false;
    auto it = std::find_if(m_PseudoLegalMoves.begin(),m_PseudoLegalMoves.end(),[&](const Vec2& pos){return (pos == move);});
    return (it != m_PseudoLegalMoves.end()) ? true : false;
  }
  
  bool IsLegalMove(const Vec2& move) const{
    if(m_LegalMoves.empty()) return false;
    auto it = std::find_if(m_LegalMoves.begin(),m_LegalMoves.end(),[&](const Vec2& pos){return (pos == move);});
    return (it != m_LegalMoves.end()) ? true : false;
  }

  bool IsAttackedSquare(const Vec2& square) const{
    if(m_AttackMoves.empty()) return false;
    auto it = std::find_if(m_AttackMoves.begin(),m_AttackMoves.end(),[&](const Vec2& pos){return (pos == square);});
    return (it != m_AttackMoves.end()) ? true : false;
  }

  bool IsDefendedSquare(const Vec2& square) const{
    if(m_DefendingMoves.empty()) return false;
    auto it = std::find_if(m_DefendingMoves.begin(),m_DefendingMoves.end(),[&](const Vec2& pos){return (pos == square);});
    return (it != m_DefendingMoves.end()) ? true : false;
  }

private:
  PieceType m_Type = PieceType::UNKNOWN;
  Team m_Team = Team::UNKNOWN;
  Vec2 m_Position; // position on map
  Texture m_Texture;
  bool m_IsInactive = false;
private:
  std::vector<Vec2> m_PseudoLegalMoves;
  std::vector<Vec2> m_LegalMoves;
  std::vector<Vec2> m_AttackMoves; // current piece that is attacking piece from opposite team
  std::vector<Vec2> m_DefendingMoves; // current piece that is attacking piece from its own team
  std::vector<SpecialMove> m_SpecialMoves;
};
#endif //! __PIECE_HPP__