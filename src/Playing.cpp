#include "Playing.hpp"
#include "SceneManager.hpp"
#include "Menu.hpp"

Playing::Playing(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window,SceneManager* scene_manager) : m_Board(renderer), m_SceneManager(scene_manager){
  m_Renderer = renderer;
  m_Window = window;
  m_Board.OnResize(window);
  m_Players.resize(2);

  auto one_square_size = m_Board.GetOneSquareSize();
  
  PieceBuilder piece_builder;
  
  Vec2 negative_pos = {-2,-2};

  for (int col_index = 0; col_index < 2; col_index++) {
    Piece::Team team;
    team = ((col_index & 1) == 0) ? Piece::Team::WHITE : Piece::Team::BLACK;
    std::string prefix = (team == Piece::Team::WHITE) ? "w_" : "b_";

    std::string texture_path = "resources/Pieces/" + prefix + "pawn.png";
    Texture pawn_texture(m_Renderer,texture_path);
    
    texture_path = "resources/Pieces/" + prefix + "bishop.png";
    Texture bishop_texture(m_Renderer,texture_path);
    
    texture_path = "resources/Pieces/" + prefix + "knight.png";
    Texture knight_texture(m_Renderer,texture_path);
    
    texture_path = "resources/Pieces/" + prefix + "rook.png";
    Texture rook_texture(m_Renderer,texture_path);
    
    for (int k = 0; k < 8; k++) {
      m_Players[col_index].AddPiece(piece_builder.ShareSDLTexture(pawn_texture)
                                          .SetPieceType(Piece::PieceType::PAWN)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[col_index].AddPiece(piece_builder.ShareSDLTexture(bishop_texture)
                                          .SetPieceType(Piece::PieceType::BISHOP)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[col_index].AddPiece(piece_builder.ShareSDLTexture(knight_texture)
                                          .SetPieceType(Piece::PieceType::KNIGHT)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[col_index].AddPiece(piece_builder.ShareSDLTexture(rook_texture)
                                          .SetPieceType(Piece::PieceType::ROOK)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos({0,0})
                                          .SetSize(one_square_size)
                                          .build());
    }
    m_Players[col_index].AddPiece(piece_builder.LoadTexture(m_Renderer,std::string("resources/Pieces/" + prefix + "queen.png"))
                                        .SetPieceType(Piece::PieceType::QUEEN)
                                        .SetTeam(team)
                                        .SetSourceTexture({0,0},{128,128})
                                        .SetPos(negative_pos)
                                        .SetSize(one_square_size)
                                        .build());

   
    m_Players[col_index].AddPiece(piece_builder.LoadTexture(m_Renderer,std::string("resources/Pieces/" + prefix + "king.png"))
                                        .SetPieceType(Piece::PieceType::KING)
                                        .SetTeam(team)
                                        .SetSourceTexture({0,0},{128,128})
                                        .SetPos(negative_pos)
                                        .SetSize(one_square_size)
                                        .build());
  }//!for loop

  m_Board.LoadPositionFromFen(CHESS_BOARD_DEFAULT_FEN,m_Players);
  
  m_Board.CalculateMoves(m_Players);
}

Playing::~Playing(){

}


void Playing::OnResize(){
  m_Board.OnResize(m_Window);
  for(auto& player : m_Players){
    for(auto& piece : player.GetPieces()){
      piece->SetSize(m_Board.GetOneSquareSize());
    }
  }
}

void Playing::OnCreate(){

}

void Playing::OnDestroy(){

}

void Playing::HandleInput(const Base::Ref<EventHandler> event_handler){
  auto& keyboard_input = event_handler->GetKeyboardInput();
  auto& mouse_input = event_handler->GetMouseInput();
  m_MouseInput = mouse_input;
  auto cursor_pos = mouse_input.GetMousePosition();

  if(event_handler->OnMouseButtonDown()){
     if(mouse_input.IsPressed(SDL_BUTTON_LEFT)){
      for(auto& player : m_Players){
        for(auto& piece : player.GetPieces()){
          if(piece->GetTexture().PointIsOnTexture(cursor_pos) && !piece->IsInactive()){
            piece->SetDragging(true);
          }
        }
      }
    }
  }

  if(event_handler->OnMouseButtonUp()){
   if(mouse_input.IsReleased(SDL_BUTTON_LEFT)){
      for(auto& player : m_Players){
        for(auto& piece : player.GetPieces()){
          if(piece->IsDragging() && !piece->IsInactive()){
            piece->SetDragging(false);

            Vec2 new_pos;
            new_pos.x = (cursor_pos.x - m_Board.GetTopLeft().x) / m_Board.GetOneSquareSize().GetWidth();
            new_pos.y = (cursor_pos.y - m_Board.GetTopLeft().y) / m_Board.GetOneSquareSize().GetHeight();

            if(m_Board.IsOnBoard(new_pos) && !m_Board.MoveLeadToCheck(m_Players,piece,new_pos) && piece->GetTeam() == m_Board.GetCurrentTurn() && m_Board.GetBoardState() == Board::BoardState::EMPTY){
              Move move;
              move.pieceToMove = piece;
              move.moveFrom = piece->GetPosition();
              move.moveTo = new_pos;

              if(piece->IsAttackedSquare(move.moveTo)){
                move.pieceToKill = m_Board.GetPieceAt(m_Players,move.moveTo);
                move.killedPos = move.pieceToKill->GetPosition();
              }
              
              if(m_Board.MakeMove(move)){
                m_Board.CalculateMoves(m_Players);
                
                m_Board.SwitchTurn();

                if(m_Board.IsCheckmated(m_Players,m_Players[0])){
                  m_Board.SetBoardState(Board::BoardState::LOSE);
                }else if(m_Board.IsCheckmated(m_Players,m_Players[1])){
                  m_Board.SetBoardState(Board::BoardState::WIN);
                }else if(m_Board.IsStalemate(m_Players)){
                  m_Board.SetBoardState(Board::BoardState::STALEMATE);
                }
              }
            }
          }
        }
      }
    }
  }
  m_Board.HandleInput(event_handler);
}


void Playing::Update(float dt){
  m_Board.Update(dt);
}

void Playing::Render(){
  m_Board.Render();

  for(auto& player : m_Players){
    for(auto& piece : player.GetPieces()){
      if(piece->IsInactive()) continue;

      if(piece->IsDragging()){
        DragEntity(m_MouseInput.GetMousePosition(),m_Renderer).Execute(piece);
      }else{
        m_Board.RenderEntity(m_Renderer,piece);
      }
    }
  }
  
}