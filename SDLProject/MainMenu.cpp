/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MainMenu.h"
#include "Utility.h"

unsigned int MENU_DATA[] = { 0 };
GLuint m_font_texture_id;

MainMenu::~MainMenu(){
    delete m_game_state.player;
    delete m_game_state.map;
    delete m_game_state.background;
}


void MainMenu::initialise(){
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(1, 1, MENU_DATA, map_texture_id, 1.0f, 20, 30);
    
    m_font_texture_id = Utility::load_texture("assets/font1.png");
    
    m_game_state.background = new Entity();
    m_game_state.background->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.background->set_scale(glm::vec3(6.0f,6.0f, 0.0f));
    m_game_state.background->m_texture_id = Utility::load_texture("assets/mainMenuBG.png");
    m_game_state.background->update(0.0f, m_game_state.background, NULL, 0, m_game_state.map);
    
    m_game_state.player = new Entity();
    
    
}

void MainMenu::update(float delta_time){
    
}

void MainMenu::render(ShaderProgram *program){
    m_game_state.map->render(program);
    m_game_state.background->render(program);
    
   // Utility::draw_text(program, m_font_texture_id, std::string("Press Enter to Start"), 0.75f, -0.45f, glm::vec3(-2.75f, 0.0f, 0.0f));
    //Utility::draw_text(<#ShaderProgram *program#>, <#GLuint font_texture_id#>, <#std::string text#>, <#float screen_size#>, <#float spacing#>, <#glm::vec3 position#>)
}

