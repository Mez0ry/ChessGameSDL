#include "Board.hpp"
#include "Entity.hpp"

Vec2 Board::m_BoardTopLeft = {0, 0};

Board::Board(const Base::Ref<Renderer> renderer) {
  m_Renderer = renderer;
  m_WhiteSquareTexture.LoadTexture(m_Renderer,
                                   "resources/Boards/board_3_basic.png");
  m_WhiteSquareTexture.SetSize(m_OneSquareSize);
  m_WhiteSquareTexture.SetSize<SourceRect>(m_OneSquareSize);
  m_WhiteSquareTexture.SetPosition({0, 0});

  m_BlackSquareTexture.ShareSDLTexture(m_WhiteSquareTexture);
  m_BlackSquareTexture.SetSize(m_OneSquareSize);
  m_BlackSquareTexture.SetSize<SourceRect>(m_OneSquareSize);
  m_BlackSquareTexture.SetPosition<SourceRect>({128, 0});
}

Board::~Board() {}

void Board::OnResize(const Base::Ref<Window> window) {
  auto [win_w, win_h] = window->GetWindowSize();

  int square_width = (win_w * 0.9 / m_BoardSize.x);
  int square_height = (win_h * 0.9 / m_BoardSize.y);

  if (square_width > square_height) {
    square_width = square_height;
  } else {
    square_height = square_width;
  }

  m_OneSquareSize = ObjectSize(square_width, square_height);

  m_BoardTopLeft.x = (win_w - (m_OneSquareSize.GetWidth() * m_BoardSize.x)) / 2;
  m_BoardTopLeft.y =
      (win_h - (m_OneSquareSize.GetHeight() * m_BoardSize.y)) / 2;

  m_WhiteSquareTexture.SetSize(m_OneSquareSize);
  m_BlackSquareTexture.SetSize(m_OneSquareSize);
}

void Board::Update(float dt) {}

void Board::Render() {
  for (size_t y = 0; y < m_BoardSize.y; y++) {
    for (size_t x = 0; x < m_BoardSize.x; x++) {
      Vec2 new_pos;
      new_pos.x = m_BoardTopLeft.x + (x * m_OneSquareSize.GetWidth());
      new_pos.y = m_BoardTopLeft.y + (y * m_OneSquareSize.GetHeight());

      int texture_type = ((x & 1) == 0) ? 0 : 1;

      if ((y & 1) != 0) {
        texture_type = !texture_type;
      }

      switch (texture_type) {
      case 0: {
        m_WhiteSquareTexture.SetPosition(new_pos);
        m_Renderer->Render(m_WhiteSquareTexture);
        break;
      }
      case 1: {
        m_BlackSquareTexture.SetPosition(new_pos);
        m_Renderer->Render(m_BlackSquareTexture);
        break;
      }
      }
    }
  }
}

void Board::RenderEntity(const Base::Ref<Renderer> renderer, const Base::Ref<Entity> entity) {
  const Vec2 &pos = entity->GetPosition();
  ObjectSize size = entity->GetSize();
  const Vec2 texture_pos = {m_BoardTopLeft.x + pos.x * size.GetWidth(),
                            m_BoardTopLeft.y + pos.y * size.GetHeight()};
  entity->GetTexture().SetPosition(texture_pos);
  entity->GetTexture().SetSize(size);

  renderer->Render(entity->GetTexture());
}

void Board::LoadPositionFromFen(const char *fen, std::vector<Player> &players) {

  std::unordered_map<Base::Ref<Piece>,char> processed_pieces_map;
  
  for (auto &player : players) {
    int row_index = 0, col_index = 0;
    int space_counter = 0;
    auto& pieces = player.GetPieces();

    for (int i = 0; i < strlen(fen); i++) {
      if (fen[i] == ' ') {
        space_counter++;
        continue;
      }

      std::function<bool(Base::Ref<Piece>)> requirement = [&](Base::Ref<Piece> piece) ->bool{
        auto piece_type = piece->GetPieceType();
        auto team = piece->GetTeam();

        char piece_symbol = (team == Piece::Team::WHITE) ? ((char)piece_type & '_') : ((char)piece_type | ' ');

        return (fen[i] == piece_symbol && processed_pieces_map.count(piece) == 0);
      };

      auto found_pieces = player.FindPiecesIf(requirement);
      
      if (std::isdigit(fen[i])) {
        col_index += (fen[i] - 48);
      } else if (fen[i] == '/') {
        row_index++;
        col_index = 0;
      } else if (found_pieces.size() >= 1 && space_counter < 1) {
        Vec2 position(col_index++, row_index);
        found_pieces.front()->SetPosition(position);
        processed_pieces_map.insert(std::make_pair(found_pieces.front(),fen[i]));
      }

      // if (fen[i] == 'w') {
      //   m_CurrentTurn = Team::WHITE;
      // } else if (fen[i] == 'b') {
      //   m_CurrentTurn = Team::BLACK;
      // }
    }
  }
}