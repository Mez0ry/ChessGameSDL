#include "Board.hpp"
#include "Entity.hpp"

Vec2 Board::m_BoardTopLeft = {0, 0};

Board::Board(const Base::Ref<Renderer> renderer)
{
    m_Renderer = renderer;
    m_WhiteSquareTexture.LoadTexture(m_Renderer, "resources/Boards/board_3_basic.png");
    m_WhiteSquareTexture.SetSize(m_OneSquareSize);
    m_WhiteSquareTexture.SetSize<SourceRect>(m_OneSquareSize);
    m_WhiteSquareTexture.SetPosition({0, 0});

    m_BlackSquareTexture.ShareSDLTexture(m_WhiteSquareTexture);
    m_BlackSquareTexture.SetSize(m_OneSquareSize);
    m_BlackSquareTexture.SetSize<SourceRect>(m_OneSquareSize);
    m_BlackSquareTexture.SetPosition<SourceRect>({128, 0});
}

Board::~Board() {}

void Board::OnResize(const Base::Ref<Window> window)
{
    auto [win_w, win_h] = window->GetWindowSize();

    int square_width = (win_w * 0.9 / m_BoardSize.x);
    int square_height = (win_h * 0.9 / m_BoardSize.y);

    if (square_width > square_height)
    {
        square_width = square_height;
    }
    else
    {
        square_height = square_width;
    }

    m_OneSquareSize = ObjectSize(square_width, square_height);

    m_BoardTopLeft.x = (win_w - (m_OneSquareSize.GetWidth() * m_BoardSize.x)) / 2;
    m_BoardTopLeft.y = (win_h - (m_OneSquareSize.GetHeight() * m_BoardSize.y)) / 2;

    m_WhiteSquareTexture.SetSize(m_OneSquareSize);
    m_BlackSquareTexture.SetSize(m_OneSquareSize);
}

void Board::Update(float dt) {}

void Board::Render()
{
    for (size_t y = 0; y < m_BoardSize.y; y++)
    {
        for (size_t x = 0; x < m_BoardSize.x; x++)
        {
            Vec2 new_pos;
            new_pos.x = m_BoardTopLeft.x + (x * m_OneSquareSize.GetWidth());
            new_pos.y = m_BoardTopLeft.y + (y * m_OneSquareSize.GetHeight());

            int texture_type = ((x & 1) == 0) ? 0 : 1;

            if ((y & 1) != 0)
            {
                texture_type = !texture_type;
            }

            switch (texture_type)
            {
            case 0:
            {
                m_WhiteSquareTexture.SetPosition(new_pos);
                m_Renderer->Render(m_WhiteSquareTexture);
                break;
            }
            case 1:
            {
                m_BlackSquareTexture.SetPosition(new_pos);
                m_Renderer->Render(m_BlackSquareTexture);
                break;
            }
            }
        }
    }
}

void Board::RenderEntity(const Base::Ref<Renderer> renderer, const Base::Ref<Entity> entity)
{
    const Vec2 &pos = entity->GetPosition();
    ObjectSize size = entity->GetSize();
    const Vec2 texture_pos = {m_BoardTopLeft.x + pos.x * size.GetWidth(),
                              m_BoardTopLeft.y + pos.y * size.GetHeight()};
    entity->GetTexture().SetPosition(texture_pos);
    entity->GetTexture().SetSize(size);

    renderer->Render(entity->GetTexture());
}

void Board::LoadPositionFromFen(const char *fen, std::vector<Player> &players)
{

    std::unordered_map<Base::Ref<Piece>, char> processed_pieces_map;

    for (auto &player : players)
    {
        int row_index = 0, col_index = 0;
        int space_counter = 0;
        auto &pieces = player.GetPieces();

        for (int i = 0; i < strlen(fen); i++)
        {
            if (fen[i] == ' ')
            {
                space_counter++;
                continue;
            }

            std::function<bool(Base::Ref<Piece>)> requirement = [&](Base::Ref<Piece> piece) -> bool
            {
                auto piece_type = piece->GetPieceType();
                auto team = piece->GetTeam();

                char piece_symbol = (team == Piece::Team::WHITE) ? ((char)piece_type & '_') : ((char)piece_type | ' ');

                return (fen[i] == piece_symbol && processed_pieces_map.count(piece) == 0);
            };

            auto found_pieces = player.FindPiecesIf(requirement);

            if (std::isdigit(fen[i]))
            {
                col_index += (fen[i] - 48);
            }
            else if (fen[i] == '/')
            {
                row_index++;
                col_index = 0;
            }
            else if (found_pieces.size() >= 1 && space_counter < 1)
            {
                Vec2 position(col_index++, row_index);
                found_pieces.front()->SetPosition(position);
                processed_pieces_map.insert(std::make_pair(found_pieces.front(), fen[i]));
            }

            // if (fen[i] == 'w') {
            //   m_CurrentTurn = Team::WHITE;
            // } else if (fen[i] == 'b') {
            //   m_CurrentTurn = Team::BLACK;
            // }
        }
    }
}

void Board::MakePseudoMove(const MoveInfo &move_info)
{
    switch(static_cast<MoveInfo::MoveType>(move_info.type)){
        case MoveInfo::MoveType::MOVE:{
            auto& move = std::get<Move>(move_info.info);
            auto &current_piece = move.pieceToMove;
            current_piece->SetPosition(move.moveTo);
            break;
        }
        case MoveInfo::MoveType::SPECIAL_MOVE:{
            auto& special_move = std::get<Piece::SpecialMove>(move_info.info);
            
            switch(static_cast<Piece::SpecialMoveType>(special_move.type)){
                case Piece::SpecialMoveType::ENPASSANT:{
                    auto& enpassant = std::get<Piece::SpecialMove::EnPassant>(special_move.variant);
                    auto& current_piece = enpassant.move.pieceToMove;
                    current_piece->SetPosition(enpassant.move.moveTo);
                    break;
                }

                case Piece::SpecialMoveType::CASTLE:{
                    auto& castle = std::get<Piece::SpecialMove::Castle>(special_move.variant);
                    
                    auto& rook_move_to = castle.rookMove.moveTo;
                    castle.rookMove.pieceToMove->SetPosition(rook_move_to);
                    
                    auto& king_move_to = castle.kingMove.moveTo;
                    castle.kingMove.pieceToMove->SetPosition(king_move_to);
                    break;
                }
            }
            break;
        }
    }

    m_MovesVec.push_back(move_info);
}

bool Board::MakeMove(const Move &move)
{
    MoveInfo move_info;
    move_info.type = MoveInfo::MoveType::MOVE;
    std::get<Move>(move_info.info) = move;

    if (move.pieceToMove->IsLegalMove(move.moveTo))
    {
        if (move.pieceToKill)
        {
            RemovePiece(move.pieceToKill);
        }

        std::function<bool(const Piece::SpecialMove&)> spec_move_pred = [&](const Piece::SpecialMove& special_move) -> bool{ 
            switch(static_cast<Piece::SpecialMoveType>(special_move.type)){
                case Piece::SpecialMoveType::ENPASSANT:{
                    auto& enpassant_info = std::get<Piece::SpecialMove::EnPassant>(special_move.variant);
                    return (enpassant_info.move.moveTo == move.moveTo);
                    break;
                }
                case Piece::SpecialMoveType::CASTLE:{
                    auto& castle_info = std::get<Piece::SpecialMove::Castle>(special_move.variant);
                    return (castle_info.castleMove == move.moveTo);
                    break;
                }
            }
            return false;
        };

        if(Piece::SpecialMove* special_move_info = move.pieceToMove->GetSpecialMoveIf(spec_move_pred); special_move_info != nullptr){
            move_info.type = MoveInfo::MoveType::SPECIAL_MOVE;
            move_info.info.emplace<1>(Piece::SpecialMove());
            
            auto& context = std::get<Piece::SpecialMove>(move_info.info);
            switch(special_move_info->type){
                case Piece::SpecialMoveType::ENPASSANT:{
                    auto& enpassant_info = std::get<Piece::SpecialMove::EnPassant>(special_move_info->variant);
                    context.type = Piece::SpecialMoveType::ENPASSANT;
                    
                    if(enpassant_info.move.pieceToKill){
                        auto& enpassant = std::get<Piece::SpecialMove::EnPassant>(context.variant);
                        enpassant.move = enpassant_info.move;
                        enpassant.move.moveFrom = move.moveFrom;
                        enpassant.move.pieceToMove = move.pieceToMove;
                        RemovePiece(enpassant_info.move.pieceToKill);
                    }
                    break;
                }
                case Piece::SpecialMoveType::CASTLE:{
                    auto& castle_info = std::get<Piece::SpecialMove::Castle>(special_move_info->variant);
                    context.type = Piece::SpecialMoveType::CASTLE;
                    context.variant.emplace<Piece::SpecialMove::Castle>(Piece::SpecialMove::Castle());

                    auto& castle = std::get<Piece::SpecialMove::Castle>(context.variant);

                    castle.castleMove = castle_info.castleMove;
                    castle.kingMove   = castle_info.kingMove;
                    castle.rookMove   = castle_info.rookMove;
                    break;
                }
            }
            
        }
        MakePseudoMove(move_info);
        return true;
    }
    return false;
}

void Board::UnmakeMove()
{
    if (m_MovesVec.empty())
        return;
    auto &move_info = m_MovesVec.back();
    
    auto unmake_normal_move = [&](const Move& move){
        auto &piece = move.pieceToMove;

        piece->SetPosition(move.moveFrom);
        if (move.pieceToKill)
        {
            RevivePiece(move.pieceToKill, move.killedPos);
        }
    };

    switch(static_cast<MoveInfo::MoveType>(move_info.type)){
        case MoveInfo::MoveType::MOVE:{
            auto& move = std::get<Move>(move_info.info);
            unmake_normal_move(move);
            break;
        }
        case MoveInfo::MoveType::SPECIAL_MOVE:{
            auto& special_move = std::get<Piece::SpecialMove>(move_info.info);
            
            switch(static_cast<Piece::SpecialMoveType>(special_move.type)){
                case Piece::SpecialMoveType::ENPASSANT:{
                    unmake_normal_move(std::get<Piece::SpecialMove::EnPassant>(special_move.variant).move);
                    break;
                }
            }
            break;
        }
    }
    
    m_MovesVec.pop_back();
}

#pragma GCC diagnostic push // Also works for clang compiler
#pragma GCC diagnostic ignored "-Wswitch"
void Board::CalculatePseudoLegalMoves(std::vector<Player> &players, std::shared_ptr<Piece> &current_piece)
{
    if (current_piece->IsInactive())
        return;

    auto curr_pos = current_piece->GetPosition();
    auto curr_pType = current_piece->GetPieceType();
    auto curr_team = current_piece->GetTeam();

    auto &curr_pseudo_legal = current_piece->GetPseudoLegalMoves();
    curr_pseudo_legal.clear();

    switch (curr_pType)
    {
    case Piece::PieceType::PAWN:
    {
        if (curr_pos.y == (m_BoardSize.y - 2) || curr_pos.y == 1)
        {
            curr_pseudo_legal.push_back({curr_pos.x, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 1 : curr_pos.y + -1});
            curr_pseudo_legal.push_back({curr_pos.x, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 2 : curr_pos.y + -2});
        }
        else
        {
            curr_pseudo_legal.push_back({curr_pos.x, (curr_team == Piece::Team::BLACK) ? curr_pos.y + std::abs(1) : curr_pos.y + -std::abs(1)});
        }

        curr_pseudo_legal.push_back({curr_pos.x - 1, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 1 : curr_pos.y + -1});

        curr_pseudo_legal.push_back({curr_pos.x + 1, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 1 : curr_pos.y + -1});

        Vec2 left_enpassant = {curr_pos.x - 1, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 1 : curr_pos.y + -1},
             right_enpassant = {curr_pos.x + 1, (curr_team == Piece::Team::BLACK) ? curr_pos.y + 1 : curr_pos.y + -1};
        curr_pseudo_legal.push_back(left_enpassant);
        curr_pseudo_legal.push_back(right_enpassant);
        break;
    }
    case Piece::PieceType::BISHOP:
    {
        CalculatePseudoBishopMoves(players, current_piece);
        break;
    }

    case Piece::PieceType::KNIGHT:
    {
        std::array<int, 8> x_offsets = {-2, -1, 1, 2, 2, 1, -1, -2};
        std::array<int, 8> y_offsets = {1, 2, 2, 1, -1, -2, -2, -1};

        for (int i = 0; i < x_offsets.size() && i < y_offsets.size(); i++)
        {
            Vec2 position = {curr_pos.x + x_offsets[i], curr_pos.y + y_offsets[i]};
            if (Board::IsOnBoard(position))
            {
                curr_pseudo_legal.push_back(position);
            }
        }
        break;
    }

    case Piece::PieceType::ROOK:
    {
        CalculatePseudoRookMoves(players, current_piece);
        break;
    }

    case Piece::PieceType::QUEEN:
    {
        CalculatePseudoBishopMoves(players, current_piece);
        CalculatePseudoRookMoves(players, current_piece);
        break;
    }

    case Piece::PieceType::KING:
    {
        std::vector<int> x_offsets = {1, -1, 0, 0, 1, -1, -1, 1};
        std::vector<int> y_offsets = {0, 0, 1, -1, 1, 1, -1, -1};

        for (int i = 0; i < x_offsets.size() && i < y_offsets.size(); i++)
        {
            Vec2 position = {curr_pos.x + x_offsets[i], curr_pos.y + y_offsets[i]};
            if (IsOnBoard(position))
            {
                curr_pseudo_legal.push_back(position);
            }
        }
        break;
    }
    }
}
#pragma GCC diagnostic pop

void Board::CalculatePseudoBishopMoves(std::vector<Player> &players, std::shared_ptr<Piece> &current_piece)
{
    std::vector<Vec2> &pseudo_legal_moves = current_piece->GetPseudoLegalMoves();
    auto &curr_pos = current_piece->GetPosition();

    // Calculate number of squares
    // in the top-left diagonal
    int tl = std::min(curr_pos.x, curr_pos.y);

    // Calculate number of squares
    // in the top-right diagonal
    int tr = std::min(curr_pos.x, m_BoardSize.x - 1 - curr_pos.y);

    // Calculate number of squares
    // in the bottom-left diagonal
    int bl = std::min(m_BoardSize.x - 1 - curr_pos.x, curr_pos.y);

    // Calculate number of squares
    // in the bottom-right diagonal
    int br = std::min(m_BoardSize.x - 1 - curr_pos.x, m_BoardSize.x - 1 - curr_pos.y);

    // all possible moves
    for (int i = 1; i <= tl; i++)
    {
        pseudo_legal_moves.push_back({curr_pos.x - i, curr_pos.y - i});
    }
    for (int i = 1; i <= tr; i++)
    {
        pseudo_legal_moves.push_back({curr_pos.x - i, curr_pos.y + i});
    }
    for (int i = 1; i <= bl; i++)
    {
        pseudo_legal_moves.push_back({curr_pos.x + i, curr_pos.y - i});
    }
    for (int i = 1; i <= br; i++)
    {
        pseudo_legal_moves.push_back({curr_pos.x + i, curr_pos.y + i});
    }
}

void Board::CalculatePseudoRookMoves(std::vector<Player> &players, Base::Ref<Piece> &current_piece)
{
    Vec2 tmp = current_piece->GetPosition();
    std::vector<Vec2> &pseudo_legal_moves = current_piece->GetPseudoLegalMoves();

    constexpr std::uint8_t mask[4] = {1 << 0,  // left is not on board
                                      1 << 1,  // right is not on board
                                      1 << 2,  // up is not on board
                                      1 << 3}; // down is not on board

    uint8_t flags = 0;

    for (Vec2 left = {tmp.x - 1, tmp.y}, right = {tmp.x + 1, tmp.y}, up = {tmp.x, tmp.y + 1}, down = {tmp.x, tmp.y - 1}; (flags ^ (mask[0] | mask[1] | mask[2] | mask[3])) != 0; left.x -= 1, right.x += 1, up.y += 1, down.y -= 1)
    {

        if (!IsOnBoard(left))
        {
            flags |= mask[0];
        }
        else
        {
            pseudo_legal_moves.push_back(left);
        }
        if (!IsOnBoard(right))
        {
            flags |= mask[1];
        }
        else
        {
            pseudo_legal_moves.push_back(right);
        }
        if (!IsOnBoard(up))
        {
            flags |= mask[2];
        }
        else
        {
            pseudo_legal_moves.push_back(up);
        }
        if (!IsOnBoard(down))
        {
            flags |= mask[3];
        }
        else
        {
            pseudo_legal_moves.push_back(down);
        }
    }
}

#pragma GCC diagnostic push // Also works for clang compiler
#pragma GCC diagnostic ignored "-Wswitch"

void Board::CalculateLegalMoves(std::vector<Player> &players, std::shared_ptr<Piece> &current_piece)
{
    if (current_piece->IsInactive())
        return;

    auto curr_pos = current_piece->GetPosition();
    auto curr_pType = current_piece->GetPieceType();
    auto curr_team = current_piece->GetTeam();

    auto &curr_pseudo_legal_moves = current_piece->GetPseudoLegalMoves();
    auto &curr_legal_moves = current_piece->GetLegalMoves();
    auto &curr_defenders = current_piece->GetDefendingMoves();
    auto &curr_attackers = current_piece->GetAttackMoves();
    auto &curr_special_moves = current_piece->GetSpecialMoves();

    curr_legal_moves.clear();
    curr_defenders.clear();
    curr_attackers.clear();
    curr_special_moves.clear();

    switch (curr_pType)
    {
    case Piece::PieceType::PAWN:
    {
        auto isMovingBack = [&](const Base::Ref<Piece> &piece, const Vec2 &direction)
        {
            auto team = piece->GetTeam();
            if (team == Piece::Team::BLACK && direction.y == -1)
            {
                return true;
            }
            else if (team == Piece::Team::WHITE && direction.y == 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        };

        auto isEnpassant = [&](const Base::Ref<Piece> &piece, const Vec2 &direction,std::vector<Piece::SpecialMove>& special_moves,const Vec2& pseudo_move)
        {
            if (m_MovesVec.empty())
                return false;

            auto piece_team = piece->GetTeam();
            Vec2 piece_pos = piece->GetPosition();
            const int curr_rank = (piece_team == Piece::Team::BLACK) ? 4 : 3;

            MoveInfo &last_played_move = m_MovesVec.back();

            auto is_required_direction = [&](Piece::Team piece_team, const Vec2 &dir)
            {
                switch (piece_team)
                {
                case Piece::Team::WHITE:
                {
                    return ((dir.x == -1 && dir.y == -1) || (dir.x == 1 && dir.y == -1));
                    break;
                }
                case Piece::Team::BLACK:
                {
                    return ((dir.x == -1 && dir.y == 1) || (dir.x == 1 && dir.y == 1));
                    break;
                }
                }
                return false;
            };

            auto is_required_rank = [this](int rank, const MoveInfo &last_move_info, Base::Ref<Piece> piece, Base::Ref<Piece> found_piece)
            {
                if(static_cast<MoveInfo::MoveType>(last_move_info.type) == MoveInfo::MoveType::MOVE){
                    auto& last_move = std::get<Move>(last_move_info.info);
                    
                    auto piece_team = piece->GetTeam();
                    switch (piece_team)
                    {
                    case Piece::Team::WHITE:
                    {
                        return (rank == piece->GetPosition().y && last_move.pieceToMove->GetTeam() == Piece::Team::BLACK && last_move.moveFrom.y == 1 && last_move.moveTo == found_piece->GetPosition());
                        break;
                    }
                    case Piece::Team::BLACK:
                    {
                        return (rank == piece->GetPosition().y && last_move.pieceToMove->GetTeam() == Piece::Team::WHITE && last_move.moveFrom.y == m_BoardSize.y - 2 && last_move.moveTo == found_piece->GetPosition());
                        break;
                    }
                    }
                }
                return false;
            };

            if (is_required_direction(piece->GetTeam(), direction))
            {
                auto ent = GetPieceAt(players, {piece_pos.x - 1, piece_pos.y});
                if (ent != nullptr && ent->GetTeam() != piece->GetTeam() && ent != piece)
                {
                    if (is_required_rank(curr_rank, last_played_move, piece, ent))
                    {
                        Piece::SpecialMove spec_move;
                        spec_move.type = Piece::SpecialMoveType::ENPASSANT;
                        auto& enpassant = std::get<Piece::SpecialMove::EnPassant>(spec_move.variant);
                        
                        enpassant.move.pieceToKill = ent;
                        enpassant.move.killedPos = ent->GetPosition();
                        enpassant.move.moveTo = pseudo_move;
                        special_moves.push_back(spec_move);

                        return true;
                    }
                }
                ent = GetPieceAt(players, {piece_pos.x + 1, piece_pos.y});
                if (ent != nullptr && ent->GetTeam() != piece->GetTeam() && ent != piece)
                {
                    if (is_required_rank(curr_rank, last_played_move, piece, ent))
                    {
                        Piece::SpecialMove spec_move;
                        spec_move.type = Piece::SpecialMoveType::ENPASSANT;
                        auto& enpassant = std::get<Piece::SpecialMove::EnPassant>(spec_move.variant);
                        
                        enpassant.move.pieceToKill = ent;
                        enpassant.move.killedPos = ent->GetPosition();
                        enpassant.move.moveTo = pseudo_move;
                        special_moves.push_back(spec_move);
                        return true;
                    }
                }
            }
            return false;
        };

        for (auto &pseudo_legal : curr_pseudo_legal_moves)
        {
            Vec2 direction = (pseudo_legal - curr_pos).Normalize();

            if (isEnpassant(current_piece, direction,curr_special_moves,pseudo_legal))
            {
                curr_legal_moves.push_back(pseudo_legal);
                continue;
            }

            if (direction.x == -1 && direction.y == 1 || direction.x == -1 && direction.y == -1)
            { // dir top-left diagonal
                auto piece = GetPieceAt(players, pseudo_legal);
                if (piece != nullptr && piece != current_piece && !isMovingBack(current_piece, direction))
                {
                    if (piece->GetTeam() != current_piece->GetTeam())
                    {
                        curr_legal_moves.push_back(pseudo_legal);
                        curr_attackers.push_back(pseudo_legal);
                    }
                    else
                    {
                        curr_legal_moves.push_back(piece->GetPosition());
                        curr_defenders.push_back(piece->GetPosition());
                    }
                }
            }
            else if (direction.x == 1 && direction.y == 1 || direction.x == 1 && direction.y == -1)
            { // dir top-right diagonal
                auto piece = GetPieceAt(players, pseudo_legal);
                if (piece != nullptr && piece != current_piece && !isMovingBack(current_piece, direction))
                {
                    if (piece->GetTeam() != current_piece->GetTeam())
                    {
                        curr_legal_moves.push_back(pseudo_legal);
                        curr_attackers.push_back(pseudo_legal);
                    }
                    else
                    {
                        curr_legal_moves.push_back(piece->GetPosition());
                        curr_defenders.push_back(piece->GetPosition());
                    }
                }
            }
            else
            {
                if (!isMovingBack(current_piece, direction))
                {
                    if (!SquareIsOccupied(players, pseudo_legal))
                    {
                        curr_legal_moves.push_back(pseudo_legal);
                    }
                }
            }
        }
        break;
    }
    case Piece::PieceType::BISHOP:
    {
        CalculateLegalBishopMoves(players, current_piece);
        break;
    }

    case Piece::PieceType::KNIGHT:
    {
        std::array<int, 8> x_offsets = {-2, -1, 1, 2, 2, 1, -1, -2};
        std::array<int, 8> y_offsets = {1, 2, 2, 1, -1, -2, -2, -1};

        for (int i = 0; i < x_offsets.size() && i < y_offsets.size(); i++)
        {
            Vec2 position = {curr_pos.x + x_offsets[i], curr_pos.y + y_offsets[i]};
            auto entity = GetPieceAt(players, position);
            if (Board::IsOnBoard(position))
            {
                if (entity != nullptr)
                {
                    if ((curr_team != entity->GetTeam()))
                    {
                        curr_attackers.push_back(position);
                        curr_legal_moves.push_back(position);
                    }
                    else
                    {
                        curr_defenders.push_back(position);
                    }
                }
                else
                {
                    curr_legal_moves.push_back(position);
                }
            }
        }
        break;
    }

    case Piece::PieceType::ROOK:
    {
        CalculateLegalRookMoves(players,current_piece);
        break;
    }

    case Piece::PieceType::QUEEN:
    {
        CalculateLegalBishopMoves(players, current_piece);
        CalculateLegalRookMoves(players,current_piece);
        break;
    }

    case Piece::PieceType::KING:
    {
        for(auto& move : curr_pseudo_legal_moves){
            auto entity = GetPieceAt(players, move);
            if (Board::IsOnBoard(move))
            {
                if (entity != nullptr)
                {
                    if ((curr_team != entity->GetTeam() && !PieceHasDefenders(players,entity)))
                    {
                        curr_attackers.push_back(move);
                        curr_legal_moves.push_back(move);
                    }
                    else
                    {
                        curr_defenders.push_back(move);
                    }
                }
                else
                {
                    curr_legal_moves.push_back(move);
                }
            }
        }
        
        CalculateCastle(players,current_piece);
        break;
    }
    }
}
#pragma GCC diagnostic pop

void Board::CalculateLegalBishopMoves(std::vector<Player> &players, Base::Ref<Piece> &current_piece)
{
    auto curr_pos = current_piece->GetPosition();
    auto curr_pType = current_piece->GetPieceType();
    auto curr_team = current_piece->GetTeam();

    auto &curr_legal_moves = current_piece->GetLegalMoves();
    auto &curr_defenders = current_piece->GetDefendingMoves();
    auto &curr_attackers = current_piece->GetAttackMoves();

    // Calculate number of squares
    // in the top-left diagonal
    int tl = std::min(curr_pos.x, curr_pos.y);

    // Calculate number of squares
    // in the top-right diagonal
    int tr = std::min(curr_pos.x, m_BoardSize.x - 1 - curr_pos.y);

    // Calculate number of squares
    // in the bottom-left diagonal
    int bl = std::min(m_BoardSize.x - 1 - curr_pos.x, curr_pos.y);

    // Calculate number of squares
    // in the bottom-right diagonal
    int br = std::min(m_BoardSize.x - 1 - curr_pos.x, m_BoardSize.x - 1 - curr_pos.y);

    auto fill_moves = [&curr_attackers, &curr_defenders, &curr_legal_moves, &curr_team, this](Base::Ref<Piece> piece, const Vec2 &move) -> void
    {
        if (IsOnBoard(move))
        {
            if (piece != nullptr)
            {
                if ((curr_team != piece->GetTeam()))
                {
                    curr_attackers.push_back(move);
                    curr_legal_moves.push_back(move);
                }
                else
                {
                    curr_defenders.push_back(move);
                }
            }
            else
            {
                curr_legal_moves.push_back(move);
            }
        }
    };

    // all possible moves
    for (int i = 1; i <= tl; i++)
    {
        Vec2 position = {curr_pos.x - i, curr_pos.y - i};
        auto piece = GetPieceAt(players, position);
        fill_moves(piece, position);
        if (piece != nullptr)
        {
            break;
        }
    }

    for (int i = 1; i <= tr; i++)
    {
        Vec2 position = {curr_pos.x - i, curr_pos.y + i};
        auto piece = GetPieceAt(players, position);
        fill_moves(piece, position);
        if (piece != nullptr)
        {
            break;
        }
    }

    for (int i = 1; i <= bl; i++)
    {
        Vec2 position = {curr_pos.x + i, curr_pos.y - i};
        auto piece = GetPieceAt(players, position);
        fill_moves(piece, position);
        if (piece != nullptr)
        {
            break;
        }
    }

    for (int i = 1; i <= br; i++)
    {
        Vec2 position = {curr_pos.x + i, curr_pos.y + i};
        auto piece = GetPieceAt(players, position);
        fill_moves(piece, position);
        if (piece != nullptr)
        {
            break;
        }
    }
}

void Board::CalculateLegalRookMoves(std::vector<Player> &players, Base::Ref<Piece> &current_piece)
{
    auto curr_pos = current_piece->GetPosition();
    auto curr_pType = current_piece->GetPieceType();
    auto curr_team = current_piece->GetTeam();

    auto &curr_legal_moves = current_piece->GetLegalMoves();
    auto &curr_defenders = current_piece->GetDefendingMoves();
    auto &curr_attackers = current_piece->GetAttackMoves();
    auto& curr_pseudo_legal_moves = current_piece->GetPseudoLegalMoves();
    
    auto add_if_legal_move = [&](const Vec2 &pseudo_legal, uint8_t &flags, uint8_t mask)
    {
        auto piece = GetPieceAt(players,pseudo_legal);
        if (IsOnBoard(pseudo_legal))
        {

            if (piece != nullptr && piece != current_piece)
            {
                if (piece->GetTeam() != current_piece->GetTeam())
                {
                    curr_legal_moves.push_back(pseudo_legal);
                    curr_attackers.push_back(pseudo_legal);
                }
                else
                {
                    if (piece->GetTeam() == current_piece->GetTeam())
                    {
                        curr_defenders.push_back(piece->GetPosition());
                    }
                }
                flags |= mask;
            }
            else
            {
                curr_legal_moves.push_back(pseudo_legal);
            }
        }
        else
        {
            flags |= mask;
        }
    };

    constexpr uint8_t mask[4] = {1 << 0,  // computing legal moves for dir left finished.
                                 1 << 1,  // dir right finished
                                 1 << 2,  // dir up finished
                                 1 << 3}; // dir down finished
    uint8_t flags = 0;

    for (int i = 0; i < curr_pseudo_legal_moves.size() && (flags ^ (mask[0] | mask[1] | mask[2] | mask[3])) != 0; i++)
    {
        Vec2 direction = (curr_pseudo_legal_moves[i] - curr_pos).Normalize();
        if (direction.x == -1 && direction.y == 0 && !(flags & mask[0]))
        {
            add_if_legal_move(curr_pseudo_legal_moves[i], flags, mask[0]);
        }
        else if (direction.x == 1 && direction.y == 0 && !(flags & mask[1]))
        {
            add_if_legal_move(curr_pseudo_legal_moves[i], flags, mask[1]);
        }
        else if (direction.y == -1 && direction.x == 0 && !(flags & mask[2]))
        {
            add_if_legal_move(curr_pseudo_legal_moves[i], flags, mask[2]);
        }
        else if (direction.y == 1 && direction.x == 0 && !(flags & mask[3]))
        {
            add_if_legal_move(curr_pseudo_legal_moves[i], flags, mask[3]);
        }
    }
}

void Board::CalculateCastle(std::vector<Player>& players, Base::Ref<Piece>& king_piece){
    if(KingInCheck(players,king_piece->GetTeam()) || PieceWasMoved(king_piece)){
        return;
    }

    auto curr_pos = king_piece->GetPosition();
    auto curr_pType = king_piece->GetPieceType();
    auto curr_team = king_piece->GetTeam();

    auto &curr_legal_moves = king_piece->GetLegalMoves();
    auto &curr_defenders = king_piece->GetDefendingMoves();
    auto &curr_attackers = king_piece->GetAttackMoves();
    auto& curr_pseudo_legal_moves = king_piece->GetPseudoLegalMoves();
    auto& curr_special_moves = king_piece->GetSpecialMoves();

    Vec2 rook_pos_left = {0,(curr_team == Piece::Team::WHITE) ? (m_BoardSize.y - 1) : 0};
    Vec2 rook_pos_right = {(m_BoardSize.y - 1) ,(curr_team == Piece::Team::WHITE) ? (m_BoardSize.y - 1) : 0};
    auto left_rook = GetPieceAt(players,rook_pos_left);
    auto right_rook = GetPieceAt(players,rook_pos_right);

    bool long_side_castle_available = true;
    bool short_side_castle_available = true;

    if(left_rook == nullptr){
        long_side_castle_available = false;
    }

    if(right_rook == nullptr){
        short_side_castle_available = false;
    }

    if(!long_side_castle_available && !short_side_castle_available){
        return;
    }

    if(PieceWasMoved(left_rook)){
        long_side_castle_available = false;
    }

    if(PieceWasMoved(right_rook)){
        short_side_castle_available = false;
    }

    if(!long_side_castle_available && !short_side_castle_available){
        return;
    }

    if(long_side_castle_available){

        Vec2 towards_king_dir = (curr_pos - left_rook->GetPosition()).Normalize();

        Vec2 castle_move1{left_rook->GetPosition() + towards_king_dir},
             castle_move2{left_rook->GetPosition()};

        curr_legal_moves.push_back(castle_move1);
        curr_legal_moves.push_back(castle_move2);

        Piece::SpecialMove spec_move;
        spec_move.variant.emplace<Piece::SpecialMove::Castle>(Piece::SpecialMove::Castle());
        spec_move.type = Piece::SpecialMoveType::CASTLE;
        auto& castle = std::get<Piece::SpecialMove::Castle>(spec_move.variant);

        castle.castleMove = castle_move1;
        castle.kingMove.moveFrom = king_piece->GetPosition();
        castle.kingMove.moveTo = {king_piece->GetPosition().x - 2,king_piece->GetPosition().y};
        castle.kingMove.pieceToMove = king_piece;

        castle.rookMove.moveFrom = left_rook->GetPosition();
        castle.rookMove.moveTo = {left_rook->GetPosition().x + 3,left_rook->GetPosition().y};
        castle.rookMove.pieceToMove = left_rook;
        
        curr_special_moves.push_back(spec_move);

        auto second_spec_move = spec_move;
        auto& second_castle = std::get<Piece::SpecialMove::Castle>(second_spec_move.variant);
        second_castle.castleMove = castle_move2;

        curr_special_moves.push_back(second_spec_move);
    }

    if(short_side_castle_available){

        Vec2 towards_king_dir = (curr_pos - right_rook->GetPosition()).Normalize();

        Vec2 castle_move1{right_rook->GetPosition() + towards_king_dir},
             castle_move2{right_rook->GetPosition()};

        curr_legal_moves.push_back(castle_move1);
        curr_legal_moves.push_back(castle_move2);

        Piece::SpecialMove spec_move;
        spec_move.variant.emplace<Piece::SpecialMove::Castle>(Piece::SpecialMove::Castle());
        spec_move.type = Piece::SpecialMoveType::CASTLE;
        auto& castle = std::get<Piece::SpecialMove::Castle>(spec_move.variant);

        castle.castleMove = castle_move1;
        castle.kingMove.moveFrom = king_piece->GetPosition();
        castle.kingMove.moveTo = {king_piece->GetPosition().x + 2,king_piece->GetPosition().y};
        castle.kingMove.pieceToMove = king_piece;
        
        castle.rookMove.moveFrom = right_rook->GetPosition();
        castle.rookMove.moveTo = {right_rook->GetPosition().x - 2,right_rook->GetPosition().y};
        castle.rookMove.pieceToMove = right_rook;
        
        curr_special_moves.push_back(spec_move);
        
        auto second_spec_move = spec_move;
        auto& second_castle = std::get<Piece::SpecialMove::Castle>(second_spec_move.variant);
        second_castle.castleMove = castle_move2;

        curr_special_moves.push_back(second_spec_move);
    }
}

void Board::CalculateMoves(std::vector<Player> &players)
{
    for (auto &player : players)
    {
        for (auto &piece : player.GetPieces())
        {
            CalculatePseudoLegalMoves(players, piece);
            CalculateLegalMoves(players, piece);
        }
    }
}

void Board::RemovePiece(Base::Ref<Piece> piece)
{
    piece->SetInactive(true);
    piece->SetPosition({-100, -100});
}

void Board::RevivePiece(Base::Ref<Piece> piece, const Vec2 &killed_pos)
{
    piece->SetInactive(false);
    piece->SetPosition(killed_pos);
}

bool Board::KingInCheck(std::vector<Player> &players, Piece::Team team) const
{
    Base::Ref<Piece> king = nullptr;

    for (auto &player : players)
    {
        if (player.GetPieces().front()->GetTeam() == team)
        {
            king = player.FindPieceIf([&](Base::Ref<Piece> piece)
                                      { return (piece->GetPieceType() == Piece::PieceType::KING); });
            break;
        }
        else
        {
            continue;
        }
    }

    if (king == nullptr)
        return false;

    /*opponent*/
    for (auto &player : players)
    {
        if (player.GetPieces().front()->GetTeam() == team)
            continue;

        for (auto &piece : player.GetPieces())
        {
            if (piece->IsInactive())
                continue;

            if (piece->IsAttackedSquare(king->GetPosition()))
            {
                return true;
            }
        }
    }

    return false;
}

bool Board::MoveLeadToCheck(std::vector<Player> &players, Base::Ref<Piece> piece, const Vec2 &move_to)
{
    Move move;
    move.moveFrom = piece->GetPosition();
    move.moveTo = move_to;
    move.pieceToMove = piece;

    if (piece->IsAttackedSquare(move_to))
    {
        move.pieceToKill = GetPieceAt(players, move_to);
        move.killedPos = move.pieceToKill->GetPosition();
    }

    bool in_check = false;

    if (MakeMove(move))
    {
        CalculateMoves(players);

        in_check = KingInCheck(players, piece->GetTeam());

        UnmakeMove();
        CalculateMoves(players);
    }

    return in_check;
}