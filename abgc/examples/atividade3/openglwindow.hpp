#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>
#include <cstdlib>
#include <iostream>

#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "model.hpp"
#include "trackball.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  int m_viewportWidth{};
  int m_viewportHeight{};

  GameData m_gameData;

  Model m_model;
  Model m_cenario3d;
  Model m_cidade3d;
  int m_trianglesToDraw{};
  int m_trianglesToDraw_cenario{};
  int m_trianglesToDraw_cidade{};

  TrackBall m_trackBallModel;
  TrackBall m_trackBallLight;

  abcg::ElapsedTimer m_restartWaitTimer;

  float m_zoom{};

  float m_aviao_lados, m_aviao_vertical{};
  glm::vec3 m_eyePosition{};
  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_modelMatrixOriginal{1.0f};
  glm::mat4 m_modelMatrix_cenario{1.0f};
  glm::mat4 m_modelMatrix_cidade{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  // Shaders
  const std::vector<const char*> m_shaderNames{
      "texture"};
  std::vector<GLuint> m_programs;
  int m_currentProgramIndex{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{1.0f};
  glm::vec4 m_Kd{1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{};

  // Light and material properties
  glm::vec4 m_lightDir_cenario{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia_cenario{1.0f};
  glm::vec4 m_Id_cenario{1.0f};
  glm::vec4 m_Is_cenario{1.0f};
  glm::vec4 m_Ka_cenario{1.0f};
  glm::vec4 m_Kd_cenario{1.0f};
  glm::vec4 m_Ks_cenario{1.0f};
  float m_shininess_cenario{};

  // Luz e materiais cidade
  glm::vec4 m_lightDir_cidade{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia_cidade{1.0f};
  glm::vec4 m_Id_cidade{1.0f};
  glm::vec4 m_Is_cidade{1.0f};
  glm::vec4 m_Ka_cidade;
  glm::vec4 m_Kd_cidade;
  glm::vec4 m_Ks_cidade;
  float m_shininess_cidade{};

  // Cenário
  int m_vetorPosicoesAleatorias[12];
  

  // Avião
  float m_aviaoVelocidade{0.0f};
  float m_aviao_angulo{0.0f};
  float m_aviaoAnguloR{0.0f};
  glm::vec3 m_posicao_aviao;
  glm::mat4 m_aviao_velocidade{1.0f};
  float m_aviao_anguloRotacaoPadrao{2.5f};

  int m_aviaoCameraCinematica = 0;

  // Asteroides
  //int const qtd_asteroides = 20;
  float const m_velocidadeAsteroides = 0.5f;
  glm::vec3 m_asteroides[5];

  // Skybox
  const std::string m_skyShaderName{"skybox"};
  GLuint m_skyVAO{};
  GLuint m_skyVBO{};
  GLuint m_skyProgram{};

  // clang-format off
  const std::array<glm::vec3, 36>  m_skyPositions{
    // Front
    glm::vec3{-1, -1, +1}, glm::vec3{+1, -1, +1}, glm::vec3{+1, +1, +1},
    glm::vec3{-1, -1, +1}, glm::vec3{+1, +1, +1}, glm::vec3{-1, +1, +1},
    // Back
    glm::vec3{+1, -1, -1}, glm::vec3{-1, -1, -1}, glm::vec3{-1, +1, -1},
    glm::vec3{+1, -1, -1}, glm::vec3{-1, +1, -1}, glm::vec3{+1, +1, -1},
    // Right
    glm::vec3{+1, -1, -1}, glm::vec3{+1, +1, -1}, glm::vec3{+1, +1, +1},
    glm::vec3{+1, -1, -1}, glm::vec3{+1, +1, +1}, glm::vec3{+1, -1, +1},
    // Left
    glm::vec3{-1, -1, +1}, glm::vec3{-1, +1, +1}, glm::vec3{-1, +1, -1},
    glm::vec3{-1, -1, +1}, glm::vec3{-1, +1, -1}, glm::vec3{-1, -1, -1},
    // Top
    glm::vec3{-1, +1, +1}, glm::vec3{+1, +1, +1}, glm::vec3{+1, +1, -1},
    glm::vec3{-1, +1, +1}, glm::vec3{+1, +1, -1}, glm::vec3{-1, +1, -1},
    // Bottom
    glm::vec3{-1, -1, -1}, glm::vec3{+1, -1, -1}, glm::vec3{+1, -1, +1},
    glm::vec3{-1, -1, -1}, glm::vec3{+1, -1, +1}, glm::vec3{-1, -1, +1}
  };
  // clang-format on

  void initializeSkybox();
  void renderSkybox();
  void renderCenario(glm:: vec3 m_deslocamento);
  void renderCidade(glm:: vec3 m_deslocamento);
  void terminateSkybox();
  void loadModel(std::string_view path);
  void carregarCenario(std::string_view path);
  void carregarCidade(std::string_view path);
  void checkCollisions();
  void update();
  void restart();
};

#endif