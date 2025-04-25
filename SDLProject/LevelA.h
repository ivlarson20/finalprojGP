/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

class LevelA : public Scene {
public:
    // info for game A puzzle
    Entity* puzzleTiles;
    int puzzle_tile_count;
    bool puzzle_solved;
    
    
    ~LevelA();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
};
