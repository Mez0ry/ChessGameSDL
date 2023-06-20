#ifndef __MOVE_INFO_HPP__
#define __MOVE_INFO_HPP__
#include "Vector.hpp"
#include "Base.hpp"

class Piece;

#define SHORT_CASTLE_MASK 1 << 0
#define LONG_CASTLE_MASK  1 << 1
#define EN_PASSANT_MASK   1 << 2
#define PROMOTION_MASK    1 << 3

struct Move
{
    Move() : pieceToMove(nullptr),pieceToKill(nullptr) {}
    Base::Ref<Piece> pieceToMove;
    Vec2 moveFrom;
    Vec2 moveTo;
    
    Vec2 killedPos;
    Base::Ref<Piece> pieceToKill;
};

#endif //!__MOVE_INFO_HPP__