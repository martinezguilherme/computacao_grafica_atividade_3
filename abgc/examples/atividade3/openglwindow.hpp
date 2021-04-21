#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
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

  Model m_model;
  Model m_cenario3d;
  int m_trianglesToDraw{};
  int m_trianglesToDraw_cenario{};

  TrackBall m_trackBallModel;
  TrackBall m_trackBallLight;

  float m_zoom{};

  float m_aviao_lados, m_aviao_vertical{};

  glm::vec3 m_eyePosition{};
  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_modelMatrixOriginal{1.0f};
  glm::mat4 m_modelMatrix_cenario{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  // Shaders
  const std::vector<const char*> m_shaderNames{
      "cubereflect", "cuberefract", "normalmapping", "texture", "blinnphong",
      "phong",       "gouraud",     "normal",        "depth"};
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
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};

  // Light and material properties
  glm::vec4 m_lightDir_cenario{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia_cenario{1.0f};
  glm::vec4 m_Id_cenario{1.0f};
  glm::vec4 m_Is_cenario{1.0f};
  glm::vec4 m_Ka_cenario;
  glm::vec4 m_Kd_cenario;
  glm::vec4 m_Ks_cenario;
  float m_shininess_cenario{};

  // Avião
  glm::vec3 m_aviao_vetor_velocidade{0.0f, 0.0f, 0.0f};
  float m_aviao_angulo{0.0f};
  float m_aviaoAnguloR{0.0f};
  glm::mat4 m_aviao_velocidade{1.0f};
  float m_aviao_anguloRotacaoPadrao{2.5f};

  int m_aviaoCameraCinematica = 0;

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
  void renderCenario();
  void terminateSkybox();
  void loadModel(std::string_view path);
  void carregarCenario(std::string_view path);
  void update();
};

#endif