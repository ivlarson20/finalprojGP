/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
attribute vec4 position;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec2 texCoordVar;
varying vec2 varPosition;

void main()
{
    vec4 viewPosition = viewMatrix * modelMatrix * position;
    varPosition = vec2(viewPosition.x, viewPosition.y);
    texCoordVar = texCoord;
    gl_Position = projectionMatrix * viewPosition;
}
