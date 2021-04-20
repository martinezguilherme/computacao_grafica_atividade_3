#ifndef CENARIO_HPP_
#define CENARIO_HPP_

#include "abcg.hpp"

#include <list>
#include <random>

class OpenGLWindow;

class Cenario {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_vboCores{};

  glm::vec4 m_color{0.3};
  float m_rotation{};
  float m_scale{1.0f};
  glm::vec2 m_translation{};

};

#endif