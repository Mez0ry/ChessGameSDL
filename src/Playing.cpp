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

  for (int row = 0; row < 2; row++) {
    Piece::Team team;
    team = ((row & 1) == 0) ? Piece::Team::WHITE : Piece::Team::BLACK;
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
      m_Players[row].AddPiece(piece_builder.ShareSDLTexture(pawn_texture)
                                          .SetPieceType(Piece::PieceType::PAWN)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[row].AddPiece(piece_builder.ShareSDLTexture(bishop_texture)
                                          .SetPieceType(Piece::PieceType::BISHOP)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[row].AddPiece(piece_builder.ShareSDLTexture(knight_texture)
                                          .SetPieceType(Piece::PieceType::KNIGHT)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos(negative_pos)
                                          .SetSize(one_square_size)
                                          .build());
    }

    for (int k = 0; k < 2; k++) {
      m_Players[row].AddPiece(piece_builder.ShareSDLTexture(rook_texture)
                                          .SetPieceType(Piece::PieceType::ROOK)
                                          .SetTeam(team)
                                          .SetSourceTexture({0,0},{128,128})
                                          .SetPos({0,0})
                                          .SetSize(one_square_size)
                                          .build());
    }
    m_Players[row].AddPiece(piece_builder.LoadTexture(m_Renderer,std::string("resources/Pieces/" + prefix + "queen.png"))
                                        .SetPieceType(Piece::PieceType::QUEEN)
                                        .SetTeam(team)
                                        .SetSourceTexture({0,0},{128,128})
                                        .SetPos(negative_pos)
                                        .SetSize(one_square_size)
                                        .build());

   
    m_Players[row].AddPiece(piece_builder.LoadTexture(m_Renderer,std::string("resources/Pieces/" + prefix + "king.png"))
                                        .SetPieceType(Piece::PieceType::KING)
                                        .SetTeam(team)
                                        .SetSourceTexture({0,0},{128,128})
                                        .SetPos(negative_pos)
                                        .SetSize(one_square_size)
                                        .build());
  }//!for loop

  m_Board.LoadPositionFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",m_Players);
}

Playing::~Playing(){

}


void Playing::OnResize(){

}

void Playing::OnCreate(){

}

void Playing::OnDestroy(){

}

void Playing::HandleInput(const Base::Ref<EventHandler> event_handler){
  auto& keyboard_input = event_handler->GetKeyboardInput();
  auto& mouse_input = event_handler->GetMouseInput();
  
}


void Playing::Update(float dt){
  m_Board.Update(dt);
}

void Playing::Render(){
  m_Board.Render();

  for(auto& player : m_Players){
    for(auto& piece : player.GetPieces()){
      m_Board.RenderEntity(m_Renderer,piece);
    }
  }
  
}
