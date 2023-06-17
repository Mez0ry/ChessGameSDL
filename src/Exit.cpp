#include "Exit.hpp"

Exit::Exit(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window,bool& is_running) : m_IsRunning(is_running){
    m_Window = window;
    m_Renderer = renderer;

    m_Text.LoadFont("resources/fonts/Aileron/Aileron-SemiBold.otf",100);
    
    m_Text.LoadText(m_Renderer,"Thanks for playing",m_BlackColor);

    OnResize();
}

Exit::~Exit(){

}

void Exit::OnResize(){
    auto[win_w,win_h] = m_Window->GetWindowSize();

    m_Text->SetSize(ObjectSize(win_w * 0.7f, win_h / 2));
    auto size = m_Text->GetSize();
    m_Text->SetPosition({(win_w / 2) - (size.GetWidth() / 2),((win_h / 2) - (size.GetHeight() / 2))});
}

void Exit::OnCreate(){
    m_Renderer->SetRenderDrawColor({255,255,255,255});
}

void Exit::HandleInput(const Base::Ref<EventHandler> event_handler){

}

void Exit::Update(float dt){
    if(m_TextShowTime.GetTicks() < (10 * 1000)){
        if(!m_StartTimerOnce){
            m_TextShowTime.Start();
            m_StartTimerOnce = true;
        }
        double t = std::clamp(0.0,1.0,m_TextShowTime.GetTicks() * 0.0001);
        m_BlackColor.r = 0 + t * (255 - 0);
        m_BlackColor.g = 0 + t * (255 - 0);
        m_BlackColor.b = 0 + t * (255 - 0);
        m_Text.LoadText(m_Renderer,"Thanks for playing",m_BlackColor);
    }else{
        m_IsRunning = false;
    }
}

void Exit::Render(){
    m_Renderer->Render(m_Text);
    
}