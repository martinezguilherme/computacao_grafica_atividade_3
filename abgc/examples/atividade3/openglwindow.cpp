#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>


void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  // Keyboard events
  if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_w){
      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(1, 0, 0));
    }
    if (event.key.keysym.sym == SDLK_s){
      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(-1, 0, 0));
    }
    if (event.key.keysym.sym == SDLK_q){
      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(0, 1, 0));
    }
    if (event.key.keysym.sym == SDLK_e){
      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(0, -1, 0));
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a){
      if (m_aviao_angulo >= 360){
        m_aviao_angulo = 0;
      }
      m_aviao_angulo += m_aviao_anguloRotacaoPadrao;

      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(0, 0, 1));
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d){
      if (m_aviao_angulo < 0){
        m_aviao_angulo = 360;
      }
      m_aviao_angulo -= m_aviao_anguloRotacaoPadrao;
      m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_aviao_anguloRotacaoPadrao), glm::vec3(0, 0, -1));
    }
    if (event.key.keysym.sym == SDLK_UP){
      m_aviaoVelocidade += 0.08f;
    }
    if (event.key.keysym.sym == SDLK_DOWN){
      m_aviaoVelocidade -= 0.08f;
    }
      
  }
  // Muda tipo de camera
  if (event.key.keysym.sym == SDLK_c && event.type == SDL_KEYDOWN){
      m_aviaoCameraCinematica += 1;
      m_aviaoCameraCinematica = m_aviaoCameraCinematica % 3;
    }


  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);


  for (int i = 0; i < 12; i++)
  {
    int numeroAleatorio = (std::rand() % 10) - 5;
    printf("Número aleatório: %d", numeroAleatorio);
    m_vetorPosicoesAleatorias[i] = numeroAleatorio;
  }

  // Create programs
  for (const auto& name : m_shaderNames) {
    auto path{getAssetsPath() + "shaders/" + name};
    auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  // Load default model
  carregarCenario(getAssetsPath() + "Street environment_V01.obj");
  loadModel(getAssetsPath() + "11805_airplane_v2_L2.obj");
  carregarCidade(getAssetsPath() + "Camellia City.obj");

  // Load cubemap
  m_model.loadCubeTexture(getAssetsPath() + "maps/cube/");

  // Initial trackball spin
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.0001f);

  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(90.0f), glm::vec3(-1, 0, 0));


  m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(0.3f));


  //m_modelMatrix_cenario = glm::translate(m_modelMatrix_cenario, glm::vec3(-1, 0, 0));
  m_modelMatrix =  glm::translate(m_modelMatrix, glm::vec3(0, 0, 1));

  initializeSkybox();
}

void OpenGLWindow::initializeSkybox() {
  // Create skybox program

  glEnable(GL_DEPTH_TEST);
  auto path{getAssetsPath() + "shaders/" + m_skyShaderName};
  m_skyProgram = createProgramFromFile(path + ".vert", path + ".frag");

  // Generate VBO
  glGenBuffers(1, &m_skyVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyPositions), m_skyPositions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_skyProgram, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_skyVAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_skyVAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::loadModel(std::string_view path) {
  m_model.loadDiffuseTexture(getAssetsPath() + "maps/airplane_body_diffuse_v1.jpg");
  m_model.loadNormalTexture(getAssetsPath() + "maps/airplane_body_diffuse_v1.jpg");
  m_model.loadFromFile(path);
  m_model.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw = m_model.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();

}

void OpenGLWindow::carregarCenario(std::string_view path) {
  m_cenario3d.loadDiffuseTexture(getAssetsPath() + "maps/textures/Building_V02_C.png");
  m_cenario3d.loadNormalTexture(getAssetsPath() + "maps/textures/Building_V02_C.png");
  m_cenario3d.loadFromFile(path);
  m_cenario3d.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw_cenario = m_cenario3d.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_cenario3d.getKa();
  m_Kd = m_cenario3d.getKd();
  m_Ks = m_cenario3d.getKs();
  m_shininess = m_cenario3d.getShininess();
}

void OpenGLWindow::carregarCidade(std::string_view path) {
  m_cidade3d.loadDiffuseTexture(getAssetsPath() + "maps/cidade_nortuna.jpg");
  m_cidade3d.loadNormalTexture(getAssetsPath() + "maps/cidade_nortuna.png");
  m_cidade3d.loadFromFile(path);
  m_cidade3d.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw_cidade = m_cidade3d.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_cidade3d.getKa();
  m_Kd = m_cidade3d.getKd();
  m_Ks = m_cidade3d.getKs();
  m_shininess = m_cidade3d.getShininess();
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint cubeTexLoc{glGetUniformLocation(program, "cubeTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint texMatrixLoc{glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(cubeTexLoc, 2);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 texMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  m_model.render(m_trianglesToDraw);

  for (int i = 0; i < 11; i = i + 2)
  {
    int entrada1 = m_vetorPosicoesAleatorias[i];
    int entrada2 = m_vetorPosicoesAleatorias[i+1];
    renderCenario(glm:: vec3(entrada1, -1.5f, entrada2));
  }

  renderCidade(glm:: vec3(0.0f, 0.0f, 0.0f));
  
  renderSkybox();

}

void OpenGLWindow::renderCenario(glm:: vec3 m_deslocamento) {
  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint cubeTexLoc{glGetUniformLocation(program, "cubeTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint texMatrixLoc{glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(cubeTexLoc, 2);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 texMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);
  glm::mat4 m_modelMatrix_cenario{1.0f};
  //printf("%f %f %f\n", m_deslocamento[0], m_deslocamento[1], m_deslocamento[2]);
  m_modelMatrix_cenario = glm::translate(m_modelMatrix_cenario, m_deslocamento);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix_cenario[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix_cenario)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  m_cenario3d.render(m_trianglesToDraw);
}


void OpenGLWindow::renderCidade(glm:: vec3 m_deslocamento) {
  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint cubeTexLoc{glGetUniformLocation(program, "cubeTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint texMatrixLoc{glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(cubeTexLoc, 2);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 texMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);
  glm::mat4 m_modelMatrix_cidade{1.0f};
  m_modelMatrix_cidade = glm::translate(m_modelMatrix_cidade, m_deslocamento);
  m_modelMatrix_cidade = glm::scale(m_modelMatrix_cidade, glm::vec3(15,15,15));

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix_cidade[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix_cidade)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  m_cidade3d.render(m_trianglesToDraw);
}

void OpenGLWindow::renderSkybox() {
  glUseProgram(m_skyProgram);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(m_skyProgram, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_skyProgram, "projMatrix")};
  GLint skyTexLoc{glGetUniformLocation(m_skyProgram, "skyTex")};

  // Set uniform variables
  auto viewMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(skyTexLoc, 0);

  glBindVertexArray(m_skyVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_model.getCubeTexture());

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glDepthFunc(GL_LEQUAL);
  glDrawArrays(GL_TRIANGLES, 0, m_skyPositions.size());
  glDepthFunc(GL_LESS);

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
    
  auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
  m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallModel.resizeViewport(width, height);
  m_trackBallLight.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
  terminateSkybox();
}

void OpenGLWindow::terminateSkybox() {
  glDeleteProgram(m_skyProgram);
  glDeleteBuffers(1, &m_skyVBO);
  glDeleteVertexArrays(1, &m_skyVAO);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_aviaoAnguloR = glm::radians(m_aviao_angulo - 45);
  float avancox = m_aviaoVelocidade*(cos(m_aviaoAnguloR) - sin(m_aviaoAnguloR));

  avancox = m_aviaoVelocidade*deltaTime;
  m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(
    0,
    avancox,
    0));

  glm::vec3 m_posicao_aviao = glm::vec3(m_modelMatrix[3][0], m_modelMatrix[3][1], m_modelMatrix[3][2]);

  // Camera fixa
  glm::vec3 m_posicaoCamera(0, 0, 2.0f  + m_zoom);

  // Camera cinemática
  if (m_aviaoCameraCinematica == 2){
    float m_posCameraX = deltaTime*(m_modelMatrix[3][2] - 0.7f)*(cos(m_aviaoAnguloR) - sin(m_aviaoAnguloR));
    float m_posCameraY = -deltaTime*(m_modelMatrix[3][0]) * (1*sin(m_aviaoAnguloR) + 1*cos(m_aviaoAnguloR));
    m_eyePosition = glm::vec3(m_posCameraY, m_modelMatrix[3][1], m_posCameraX);
    m_posicaoCamera = m_eyePosition;
  }

  //Camera segue o avião

  if (m_aviaoCameraCinematica == 0){
    float m_aviaoAnguloEspelhoR = -(m_aviaoAnguloR + glm::radians(135.0f));

    m_posicaoCamera = m_posicao_aviao + glm::vec3(cos(m_aviaoAnguloEspelhoR), 0,sin(m_aviaoAnguloEspelhoR));

  }
  m_viewMatrix = glm::lookAt(m_posicaoCamera, m_posicao_aviao,
                              glm::vec3(0.0f, 1.0f, 0.0f));


}