#include "Player.hpp"

Player::Player() {

}

Player::~Player() {

}

void Player::AddPiece(Base::Ref<Piece> piece) {
    m_Pieces.push_back(std::move(piece));
}

void Player::CopyPieces(std::vector<Base::Ref<Piece>> &pieces_out) const {
    for(auto& piece : m_Pieces){
        pieces_out.push_back(Base::CreateRef<Piece>(*piece));
    }
}

Base::Ref<Piece> Player::FindPieceIf(const std::function<bool(Base::Ref<Piece>)>& function){
    for(auto piece : m_Pieces){
        if(function(piece)){
            return piece;
        }
    }
    
    return nullptr;
}

std::vector<Base::Ref<Piece>> Player::FindPiecesIf(const std::function<bool(Base::Ref<Piece>)>& function){
    std::vector<Base::Ref<Piece>> pieces;
    for(auto piece : m_Pieces){
        if(function(piece)){
            pieces.push_back(piece);
        }
    }
    return pieces;
}