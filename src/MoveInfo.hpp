#ifndef __MOVE_INFO_HPP__
#define __MOVE_INFO_HPP__
#include "Vector.hpp"
#include "Piece.hpp"

#define SHORT_CASTLE_MASK 1 << 0
#define LONG_CASTLE_MASK  1 << 1
#define EN_PASSANT_MASK   1 << 2
#define PROMOTION_MASK    1 << 3

struct MoveInfo
{
    MoveInfo() : pieceToMove(nullptr),pieceToKill(nullptr), specialMove(0), promotionTo(Piece::PieceType::UNKNOWN) {}
    Base::Ref<Piece> pieceToMove;
    Vec2 moveFrom;
    Vec2 moveTo;
    uint8_t specialMove; // 0b - shortCastle, 1b - LongCastle, 2b - enpassant, 3b protomotion
    Piece::PieceType promotionTo;
    
    Vec2 killedPos;
    Base::Ref<Piece> pieceToKill;
};

#endif //!__MOVE_INFO_HPP__