#ifndef __BOARD_HPP__
#define __BOARD_HPP__
#include "Base.hpp"
#include "Player.hpp"
#include "Texture.hpp"
#include <array>
#include <vector>
#include "Vector.hpp"
#include "MoveInfo.hpp"

#define BOARD_HEIGHT 8
#define BOARD_WIDTH 8

class Board {
public:
  Board(const Base::Ref<Renderer> renderer);
  ~Board();

  void OnResize(const Base::Ref<Window> window);
  void Update(float dt);
  void Render();
  static void RenderEntity(const Base::Ref<Renderer> renderer,const Base::Ref<Entity> entity);

  void LoadPositionFromFen(const char *fen, std::vector<Player> &players);
  
  /**
   * @brief makes move if move is legal
  */
  void MakeMove(const MoveInfo& move_info);
  
  void UnmakeMove();

  void CalculatePseudoLegalMoves(std::vector<Player> &players, Base::Ref<Piece>& current_piece);
  void CalculatePseudoBishopMoves(std::vector<Player> &players, Base::Ref<Piece>& current_piece);
  void CalculatePseudoRookMoves(std::vector<Player>& players, Base::Ref<Piece>& current_piece);

  void CalculateLegalMoves(std::vector<Player> &players, Base::Ref<Piece>& current_piece);
  void CalculateLegalBishopMoves(std::vector<Player>& players, Base::Ref<Piece>& current_piece);
  
  void CalculateMoves(std::vector<Player>& players);

  bool SquareIsOccupied(std::vector<Player> &players,const Vec2& square) const{
    std::function<bool(Base::Ref<Piece>)> requirement = [&](Base::Ref<Piece> piece) ->bool{
      return (piece->GetPosition() == square);
    };

    for(auto& player : players){
      auto pieces = player.FindPiecesIf(requirement);
      if(!pieces.empty()){
        return true;
      }
    }

    return false;
  }
  
  Base::Ref<Piece> GetPieceAt(std::vector<Player>& players,const Vec2& square){
    std::function<bool(Base::Ref<Piece>)> requirement = [&](Base::Ref<Piece> piece) ->bool{
      return (piece->GetPosition() == square);
    };

    for(auto& player : players){
      auto pieces = player.FindPiecesIf(requirement);
      if(!pieces.empty()){
        return pieces.front();
      }
    }

    return nullptr;
  }
  
public:
  bool IsOnBoard(const Vec2 &pos) {
    return (pos.y < m_BoardSize.y && pos.x < m_BoardSize.x && pos.y > -1 && pos.x > -1);
  }

  uint32_t GetArrayIndexAt(const Vec2 &pos) const {
    return (pos.y * BOARD_WIDTH + pos.x);
  }

  ObjectSize GetOneSquareSize() const { return m_OneSquareSize; }

  const Vec2 &GetTopLeft() const { return m_BoardTopLeft; }

private:
  std::array<uint8_t, BOARD_HEIGHT * BOARD_WIDTH> m_Board;
  Texture m_BlackSquareTexture;
  Texture m_WhiteSquareTexture;
  const Vec2 m_BoardSize = {BOARD_WIDTH, BOARD_HEIGHT};

private:
  Base::Ref<Renderer> m_Renderer;
  ObjectSize m_OneSquareSize = ObjectSize(128, 128);
  static Vec2 m_BoardTopLeft;
  std::vector<MoveInfo> m_MovesVec;
};
#endif //! __BOARD_HPP__