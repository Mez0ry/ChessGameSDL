#include "Menu.hpp"
#include "SceneManager.hpp"
#include "Playing.hpp"
#include "Exit.hpp"

Menu::Menu(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window,SceneManager* scene_manager) : m_SceneManager(scene_manager){
    m_Renderer = renderer;
    m_Window = window;

    m_TitlePanelTexture.LoadTexture(m_Renderer,"resources/UI/TitlePanel01.png");
    m_TitlePanelTexture.SetSize(ObjectSize(418,30));

    m_TitleText.LoadFont("resources/fonts/Wombyland/OpenType-TT/WombyLand.ttf",30);
    m_TitleText.LoadText(m_Renderer,"ChessGameSDL",{172,7,62,230});

    m_TitleText->SetSize(ObjectSize(200,30));

    auto button_texture = m_ButtonsTexture[0].LoadTexture(m_Renderer,"resources/UI/Button15.png");

    for(auto& button : m_ButtonsTexture){
        button.ShareSDLTexture(button_texture);
    }
    
    auto options_font = Text::StaticLoadFont("resources/fonts/Aileron/Aileron-SemiBold.otf",100);
    
    for(auto& option : m_MenuOptions){
        auto& text = option.text;
        auto& label = option.label;

        text[0].ShareFont(options_font);
        text[1].ShareFont(options_font);
        text[0].LoadText(m_Renderer,label,m_WhiteColor);
        text[1].LoadText(m_Renderer,label,m_RedColor);
    }
    
    OnResize();
}

Menu::~Menu(){

}

void Menu::OnResize() {
    int win_w{},win_h {};

    std::tie(win_w,win_h) = m_Window->GetWindowSize();

    ObjectSize menu_option_size_dst(100,50);

    int y_offset = 0;
    
    for(auto& option : m_MenuOptions){
        auto& text = option.text;

        Vec2 pos;
        pos.x = (win_w / 2) - (menu_option_size_dst.GetWidth() / 2);
        pos.y = ((win_h / 2) - (menu_option_size_dst.GetHeight() * 1.2)) + y_offset;
        for(auto i = 0;i<2;i++){
         text[i]->SetRect(pos,menu_option_size_dst);
        }
        
        y_offset += (menu_option_size_dst.GetHeight() * 2);
    }

    for(size_t i = 0;i < m_ButtonsTexture.size() && i < m_MenuOptions.size();i++){
       auto& button = m_ButtonsTexture[i];
       auto& menu_option = m_MenuOptions[i];
       auto option_size = menu_option.text[0]->GetSize();
       auto option_pos = menu_option.text[0]->GetPosition();

       ObjectSize button_size(option_size.GetWidth() * 1.2f,option_size.GetHeight() * 1.2f);
       button.SetSize(button_size);

       ObjectSize diff_size = (button.GetSize() - option_size);
       Vec2 pos;
       
       pos.x = (option_pos.x - (diff_size.GetWidth() / 2));
       pos.y = (option_pos.y - (diff_size.GetHeight() / 2));
       button.SetPosition(pos);
    }

    auto title_size = m_TitleText->GetSize();

    m_TitleText->SetPosition({win_w / 2 - (title_size.GetWidth() / 2), static_cast<int>(win_h * 0.1f)});

    ObjectSize title_panel_size(title_size.GetWidth() * 1.3f,title_size.GetHeight() * 1.5f);
    m_TitlePanelTexture.SetSize(title_panel_size);

    ObjectSize diff = m_TitlePanelTexture.GetSize() - title_size;
    Vec2 pos;
    
    pos.x = (m_TitleText->GetPosition().x - (diff.GetWidth() / 2));
    pos.y = (m_TitleText->GetPosition().y - (diff.GetHeight() / 2));

    m_TitlePanelTexture.SetPosition(pos);
}

void Menu::OnCreate(){
    m_Renderer->SetRenderDrawColor({50,40,90,220});
}

void Menu::HandleInput(const Base::Ref<EventHandler> event_handler){
  auto& keyboard_input = event_handler->GetKeyboardInput();
  auto& mouse_input = event_handler->GetMouseInput();

  if(event_handler->OnMouseMotion()){
    for(auto& menu_option : m_MenuOptions){
        if(menu_option.text[0]->PointIsOnTexture(mouse_input.GetMousePosition())){
            menu_option.text_type_index = 1;
        }else{
            menu_option.text_type_index = 0;
        }
    }
  }
  int option_counter = 0;
  for(auto& menu_option : m_MenuOptions){
    auto& text = menu_option.text;
    auto& index = menu_option.text_type_index;
    if(mouse_input.IsPressed(SDL_BUTTON_LEFT) && text[index]->PointIsOnTexture(mouse_input.GetMousePosition())){
        switch(option_counter){
        case 0:{
            m_SceneManager->TransitionTo<Playing>();
            break;
        }
        case 1:{
            m_SceneManager->TransitionTo<Exit>();
            break;
        }
        }//!switch
    }
    option_counter++;
  }
}

void Menu::Update(float dt){

}

void Menu::Render(){
    for(size_t i = 0;i < m_ButtonsTexture.size() && i < m_MenuOptions.size();i++){
        auto& menu_option = m_MenuOptions[i];
        auto& text = m_MenuOptions[i].text;
        auto& button = m_ButtonsTexture[i];
        m_Renderer->Render(button);
        m_Renderer->Render(text[menu_option.text_type_index]);
    }
    m_Renderer->Render(m_TitlePanelTexture);
    m_Renderer->Render(m_TitleText);
}