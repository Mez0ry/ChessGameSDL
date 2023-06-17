#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__
#include "Piece.hpp"
#include <vector>
#include <functional>

class Player {
public:
  Player();
  ~Player();

  void AddPiece(Base::Ref<Piece> piece);
  
  std::vector<Base::Ref<Piece>>& GetPieces(){return m_Pieces;}

  void CopyPieces(std::vector<Base::Ref<Piece>>& pieces_out) const;

  std::vector<Base::Ref<Piece>> FindPiecesIf(const std::function<bool(Base::Ref<Piece>)>& function);
private:
  std::vector<Base::Ref<Piece>> m_Pieces;
};

#endif //!__PLAYER_HPP__