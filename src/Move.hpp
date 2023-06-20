#ifndef __MOVE_INFO_HPP__
#define __MOVE_INFO_HPP__
#include "Vector.hpp"
#include "Base.hpp"

class Piece;

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