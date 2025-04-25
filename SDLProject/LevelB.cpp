/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"
#include "Effects.h"


#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 8
#define FIXED_TIMESTEP 0.0166666f
float door_cool = 0.0f;

extern glm::mat4 g_view_matrix, g_projection_matrix;


unsigned int LEVELB_DATA[] ={
    81, 81, 233, 81, 81, 81, 273, 81, 81, 81, 273, 81, 81, 81, 273, 81, 81, 81, 81, 81,
    0, 1, 253, 1, 1, 1, 293, 1, 1, 1, 293, 1, 1, 1, 293, 1, 1, 1, 1, 2,
    20, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 22,
    20, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 22,
    20, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 22,
    20, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 22,
    20, 163, 233, 163, 163, 163, 273, 163, 163, 163, 273, 163, 163, 163, 273, 163, 163, 163, 163, 22,
    40, 41, 253, 41, 41, 41, 293, 41, 41, 41, 293, 41, 41, 41, 293, 41, 41, 41, 41, 42
    
};


LevelB::~LevelB(){
    delete m_game_state.player;
    delete m_game_state.map;
}

void LevelB::initialise(){
   // g_effects = new Effects(g_projection_matrix, g_view_matrix);
    //g_effects->start(FADEIN, 1.0f);

    m_game_state.next_scene_id = -1;
    
    m_game_state.timer = 60.0f;
    m_game_state.game_lost = false;
    m_game_state.lose_timer = 0.0f;
    
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 20, 30);
    
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    GLuint texture_ids = Utility::load_texture("assets/isaSprite2.jpg");

    m_game_state.player = new Entity(
        texture_ids,               // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.7f,                      // width
        0.7f,                      // height
        PLAYER                    // entity type
    );
        
    m_game_state.player->set_position(glm::vec3(2.0f, -5.2f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(0.75f, 0.75f, 0.0f));
    
    GLuint knight_texture_id = Utility::load_texture("assets/knight.png");
    m_game_state.knight = new Entity[2];
    
    m_game_state.knight[0] = Entity(knight_texture_id, 2.0f, 1.0f, 1.0f, KNIGHT, RunningKNIGHT, IDLE);
    
    m_game_state.knight[0].set_position(glm::vec3(6.0f, -5.0f, 0.0f));
    m_game_state.knight[0].set_scale(glm::vec3(1.0f));
    m_game_state.knight[0].left_bound = 2.0f;
    m_game_state.knight[0].right_bound = 16.0f;
    m_game_state.knight[0].platform_direction = 1;
    m_game_state.knight[0].m_is_active = true;
    
    
    m_game_state.knight[1] = Entity(knight_texture_id, 2.0f, 1.0f, 1.0f, KNIGHT, RunningKNIGHT, IDLE);
    
    m_game_state.knight[1].set_position(glm::vec3(6.0f, -2.0f, 0.0f));
    m_game_state.knight[1].set_scale(glm::vec3(1.0f));
    m_game_state.knight[1].left_bound = 2.0f;
    m_game_state.knight[1].right_bound = 16.0f;
    m_game_state.knight[1].platform_direction = 1;
    m_game_state.knight[1].m_is_active = true;
    
    // shaders
    m_spotlight_shader.load("shaders/vertex_spotlight.glsl", "shaders/fragment_spotlight.glsl");
    m_spotlight_shader.set_projection_matrix(g_projection_matrix);
    m_spotlight_shader.set_view_matrix(g_view_matrix);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/clock-ticking-83775.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(60);
    
    m_game_state.slashSfx = Mix_LoadWAV("assets/lose sound 1_0.wav");
    Mix_VolumeChunk(m_game_state.slashSfx, 40);
        
}

void LevelB::update(float delta_time)
{

    if (door_cool > 0.0f){
        door_cool -= delta_time;
    }
    
    if (!m_game_state.game_lost && !m_game_state.player->m_wins) {
        m_game_state.timer -= delta_time;

        if (m_game_state.timer <= 0.0f) {
            m_game_state.game_lost = true;
            m_game_state.timer = 0.0f;
            m_game_state.lose_timer = 0.0f;

        }
    }
    if (m_game_state.game_lost) {
        m_game_state.player->set_movement(glm::vec3(0.0f));
        m_game_state.player->set_speed(0.0f);

        m_game_state.lose_timer += delta_time; // start counting seconds

        if (m_game_state.lose_timer >= 5.0f) {
            m_game_state.next_scene_id = 0; // Go back to main menu (scene 0)
        }
        
        return; // Stop updating game normally
    }
    m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0,
                                m_game_state.map);
   // m_game_state.knight->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    for (int i = 0; i<2; i++){
        m_game_state.knight[i].ai_activate(m_game_state.player, delta_time);
        m_game_state.knight[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        
        if (m_game_state.player->check_collision(&m_game_state.knight[i])){
            Mix_PlayChannel(-1, m_game_state.slashSfx, 0);
            m_game_state.next_scene_id = 2;
            return;
        }
    }
    
    
    // DOOR MOVEMENT LOGIC
    
    // inital positions
    glm::vec3 player_pos = m_game_state.player->get_position();
    glm::vec3 respawn_pos = glm::vec3(2.0f, -5.2f, 0.0f);   // position of original door you go through
    float door_w = 1.0f;
    float door_h = 1.0f;
    glm::vec3 door_0_pos = glm::vec3(2.0f, -1.0f, 0.0f);
    glm::vec3 door_1_pos = glm::vec3(6.0f, -1.0f, 0.0f);
    glm::vec3 door_2_pos = glm::vec3(10.0f, -1.0f, 0.0f);
    glm::vec3 door_3_pos = glm::vec3(14.0f, -1.0f, 0.0f);
    glm::vec3 door_4_pos = glm::vec3(6.0f, -6.2f, 0.0f);
    glm::vec3 door_5_pos = glm::vec3(10.0f, -6.2f, 0.0f);
    glm::vec3 door_6_pos = glm::vec3(14.0f, -6.2f, 0.0f);

    
    // door 0 (the other brown door) SHADER LOGIC WITH THIS , maybe do a bool so i can go in and out
    bool touching_door_0 =
            fabs(player_pos.x - door_0_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_0_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_0 && door_cool <= 0.0f){
        printf("Door entered, moving to door OG\n");
        player_pos = respawn_pos;
        door_cool = 0.5f;
        m_use_spotlight = !m_use_spotlight;
    }

    // door 1
    bool touching_door_1 =
            fabs(player_pos.x - door_1_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_1_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_1 && door_cool <= 0.0f){
        printf("Door entered, moving to door 5\n");
        player_pos = door_5_pos;
        door_cool = 0.5f;
    }
    // door 2
    bool touching_door_2 =
            fabs(player_pos.x - door_2_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_2_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_2 && door_cool <= 0.0f){
        printf("Door entered, moving to door 4\n");
        player_pos = door_4_pos;
        door_cool = 0.5f;
    }
    // door 3
    bool touching_door_3 =
            fabs(player_pos.x - door_3_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_3_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_3 && door_cool <= 0.0f){
        printf("Door entered, moving to door 6\n");
        player_pos = door_6_pos;
        door_cool = 0.5f;

    }
    // door 4
    bool touching_door_4 =
            fabs(player_pos.x - door_4_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_4_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_4 && door_cool <= 0.0f){
        printf("Door entered, moving to door 2\n");
        player_pos = door_2_pos;
        door_cool = 0.5f;

    }
    // door 5 ONTO NEXT LEVEL
    bool touching_door_5 =
            fabs(player_pos.x - door_5_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_5_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_5 && door_cool <= 0.0f){
        printf("Door entered, moving onto next level\n");
        m_game_state.next_scene_id = 3;
        door_cool = 0.5f;

    }
    // door 6
    bool touching_door_6 =
            fabs(player_pos.x - door_6_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
            fabs(player_pos.y - door_6_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

    if (touching_door_6 && door_cool <= 0.0f){
        printf("Door entered, moving to door 3\n");
        player_pos = door_3_pos;
        door_cool = 0.5f;

    }
    m_game_state.player->set_position(player_pos);

    

    
    
}

void LevelB::render(ShaderProgram *program)
{
    ShaderProgram *active_shader = (m_use_spotlight) ? &m_spotlight_shader : program;
    
    glUseProgram(active_shader->get_program_id());
    
    active_shader->set_view_matrix(g_view_matrix);
    active_shader->set_projection_matrix(g_projection_matrix);
    
    if (m_use_spotlight) {
        glm::vec3 player_pos = m_game_state.player->get_position();
        glm::vec4 player_cam = g_view_matrix * glm::vec4(player_pos, 1.0f);
        glm::vec3 light_pos(player_cam.x, player_cam.y, 0.0f); // fix to vec3
        m_spotlight_shader.set_light_position(light_pos);
    }
    
    m_game_state.map->render(active_shader);

    m_game_state.player->render(active_shader);
    m_game_state.knight[0].render(active_shader);
    m_game_state.knight[1].render(active_shader);
    
    std::string time_text = "Time: " + std::to_string(static_cast<int>(m_game_state.timer));
    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                       time_text, 0.4f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));

    if (m_game_state.game_lost) {
        Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                           "You Lose!", 0.7f, 0.01f, glm::vec3(4.0f, -5.0f, 0.0f));
    }


  //  g_effects->render();

}


