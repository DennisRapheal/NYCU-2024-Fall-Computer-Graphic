#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "context.h"
#include "gl_helper.h"
#include "model.h"
#include "opengl_context.h"
#include "program.h"
#include "utils.h"

void initOpenGL();
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int, int action, int);

Context ctx;

Material mFlatwhite;
Material mShinyred;
Material mClearblue;

void loadMaterial() {
  mFlatwhite.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
  mFlatwhite.specular = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.shininess = 10;

  mShinyred.ambient = glm::vec3(0.1985f, 0.0000f, 0.0000f);
  mShinyred.diffuse = glm::vec3(0.5921f, 0.0167f, 0.0000f);
  mShinyred.specular = glm::vec3(0.5973f, 0.2083f, 0.2083f);
  mShinyred.shininess = 100.0f;

  mClearblue.ambient = glm::vec3(0.0394f, 0.0394f, 0.3300f);
  mClearblue.diffuse = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.specular = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.shininess = 10;
}

void loadPrograms() {
  ctx.programs.push_back(new ExampleProgram(&ctx));
  ctx.programs.push_back(new LightProgram(&ctx));
  //ctx.programs.push_back(new BasicProgram(&ctx));

  for (auto iter = ctx.programs.begin(); iter != ctx.programs.end(); iter++) {
    if (!(*iter)->load()) {
      std::cout << "Load program fail, force terminate" << std::endl;
      exit(1);
    }
  }
  glUseProgram(0);
}

Model* createDice() {
  /* TODO#1-1: Add the dice model
   *           1. Create a model by reading the model file "../assets/models/cube/cube.obj" with the object loader(Model::fromObjectFile()) you write.
   *           2. Add the texture "../assets/models/cube/dice.jpg" to the model.
   *           3. Do transform(rotation & scale) to the model.
   *           
   * Note:
   *           You should finish implement the object loader(Model::fromObjectFile()) first.
   *           You can refer to the Model class structure in model.h.
   * Hint:
   *           Model* m = Model::fromObjectFile();
   *           m->textures.push_back();
   *           m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
   *           ctx.models.push_back(m);
   */
  // 1. read obj file
  Model* m = Model::fromObjectFile("../assets/models/cube/cube.obj");
  if (!m) {
    std::cerr << "Failed to load dice model!" << std::endl;
    return nullptr;
  }
  m->numVertex = m->positions.size() / 3;

  // 2. push texture(m->textures defined in model.h)
  m->textures.push_back(createTexture("../assets/models/cube/dice.jpg"));

  // 3. set position and scaling
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  return m;
}

Model* createBottle() {
  /* TODO#1-2: Add the bottle model
   *           1. Create a model by reading the model file "../assets/models/bottle/bottle.obj" with the object loader(Model::fromObjectFile()) you write.
   *           2. Add the texture "../assets/models/bottle/bottle.jpg" to the model.
   *           3. Do transform(rotation & scale) to the model.
   *           4. Set the drawMode for this model
   * Note:
   *           You should finish implement the object loader(Model::fromObjectFile()) first.
   *           You can refer to the Model class structure in model.h.
   * Hint:
   *           Model* m = Model::fromObjectFile();
   *           m->textures.push_back();
   *           m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
   *           m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
   *           m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
   *           m->drawMode = 
   */
    // 1. read obj file
    Model* m = Model::fromObjectFile("../assets/models/bottle/bottle.obj");
    if (!m) {
      std::cerr << "Failed to load dice model!" << std::endl;
      return nullptr;
    }
    m->numVertex = m->positions.size() / 3;
    // 2. push texture(m->textures defined in model.h)
    m->textures.push_back(createTexture("../assets/models/bottle/bottle.jpg"));

    // 3. set position and scaling
    m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));            // 縮放
    m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 沿 X 軸旋轉 -90 度
    m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // 沿 Z 軸旋轉 90 度

    m->drawMode = GL_QUADS; // f in .obj has four vertices 
  return m;
}

Model* createPlane() {
  /* TODO#1-3: Add a plane model
   *           1. Create a model and manually set plane positions, normals, texcoords
   *           2. Add texure "../assets/models/Wood_maps/AT_Wood.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           GL_TEXTURE_WRAP is set to GL_REPEAT in createTexture, you may need to know
   *           what this means to set m->textures correctly
   */
  Model* m = new Model();

  std::vector<float> positions = {-4.096f, 0.0f, -2.56f, -4.096f, 0.0f, 2.56f,
                                  4.096f,  0.0f, 2.56f,  4.096f,  0.0f, -2.56f};
  std::vector<float> normals = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
  std::vector<float> texcoords = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};

  // 2. 填充到模型數據中
  m->positions = positions;
  m->normals = normals;
  m->texcoords = texcoords;

  // 3. 添加紋理
  m->textures.push_back(createTexture("../assets/models/Wood_maps/AT_Wood.jpg"));

  // 4. 設置頂點數和繪圖模式
  m->numVertex = 4;           // 四個頂點
  m->drawMode = GL_QUADS;     // 使用四邊形模式渲染

  // 5. 將平面平移到中心位置 (4.096, 0, 2.56) in setUpObject
  return m;
}

float bezier(float t, float p0, float p1, float p2, float p3) {
  return pow(1 - t, 3) * p0 + 3 * pow(1 - t, 2) * t * p1 + 3 * (1 - t) * t * t * p2 + t * t * t * p3;
}

Model* createBezierVaseModel() {
  const int segments = 36;         // Circular segments
  const int height_segments = 50;  // Height segments
  float height = 3.0f;             // Vase Height

  // Control points for the Bezier curve (you can try adjusting these to shape the vase)
  float p0 = 1;     // Radius at base
  float p1 = 1.5f;  // Control point 1
  float p2 = 1.2f;  // Control point 2
  float p3 = 0.5f;  // Radius at neck

  /* TODO#1-3: Add a vase outer surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  Model* vase = new Model();
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<int> indices; // 索引緩衝區

  // generate vertices
  for (int h = 0; h <= height_segments; ++h) {
    float t = static_cast<float>(h) / height_segments;
    float y = height * t;
    float radius = bezier(t, p0, p1, p2, p3);

    for (int s = 0; s <= segments; ++s) {
      float theta = static_cast<float>(s) / segments * 2.0f * M_PI;
      float x = radius * cos(theta);
      float z = radius * sin(theta);

      positions.push_back(x);
      positions.push_back(y);
      positions.push_back(z);

      glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);

      texcoords.push_back(static_cast<float>(s) / segments);
      texcoords.push_back(static_cast<float>(h) / height_segments);
    }
  }

  // construct each triangle in counter-clockwise order
  for (int h = 0; h < height_segments; ++h) {
    for (int s = 0; s < segments; ++s) {
      int curr = h * (segments + 1) + s;
      int next = curr + segments + 1;

      indices.push_back(curr);
      indices.push_back(next);
      indices.push_back(curr + 1);

      indices.push_back(next);
      indices.push_back(next + 1);
      indices.push_back(curr + 1);
    }
  }

  // push position, normal, textcoord into model
  for (int i : indices) {
    vase->positions.push_back(positions[i * 3 + 0]);
    vase->positions.push_back(positions[i * 3 + 1]);
    vase->positions.push_back(positions[i * 3 + 2]);

    vase->normals.push_back(normals[i * 3 + 0]);
    vase->normals.push_back(normals[i * 3 + 1]);
    vase->normals.push_back(normals[i * 3 + 2]);

    vase->texcoords.push_back(texcoords[i * 2 + 0]);
    vase->texcoords.push_back(texcoords[i * 2 + 1]);
  }

  GLuint texture = createTexture("../assets/models/Vase/Vase.jpg");
  if (!texture) {
    std::cerr << "Failed to load vase texture!" << std::endl;
    delete vase;
    return nullptr;
  }
  vase->textures.push_back(texture);

  vase->numVertex = vase->positions.size() / 3;
  vase->drawMode = GL_TRIANGLES;

  return vase;
}

Model* createBezierVaseInnerModel() {
  const int segments = 36;         // Circular segments
  const int height_segments = 50;  // Height segments
  float height = 3.0f;             // Vase Height

  // Control points for the Bezier curve (adjust these to shape the vase)
  float p0 = 1;     // Radius at base
  float p1 = 1.5f;  // Control point 1
  float p2 = 1.2f;  // Control point 2
  float p3 = 0.5f;  // Radius at neck

  /* TODO#1-4: Add a vase inner surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase2.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  Model* vase = new Model();
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<int> indices;

    // 生成內壁的頂點、法線和貼圖座標
    for (int h = 0; h <= height_segments; ++h) {
        float t = static_cast<float>(h) / height_segments;
        float y = height * t;
        float innerRadius = 0.95f * bezier(t, p0, p1, p2, p3);  // 縮小內壁半徑

        for (int s = 0; s <= segments; ++s) {
            float theta = static_cast<float>(s) / segments * 2.0f * M_PI;
            float x = innerRadius * cos(theta);
            float z = innerRadius * sin(theta);

            // 頂點座標
            positions.push_back(-x);  // X 座標反向
            positions.push_back(y);
            positions.push_back(-z); // Z 座標反向

            // 法線方向（內壁需要指向內部）
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z)); // 反轉法線
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            // 貼圖座標
            texcoords.push_back(static_cast<float>(s) / segments);
            texcoords.push_back(static_cast<float>(h) / height_segments);
        }
    }

    // 生成內壁的索引數據
    for (int h = 0; h < height_segments; ++h) {
        for (int s = 0; s < segments; ++s) {
            int curr = h * (segments + 1) + s;
            int next = curr + segments + 1;

            indices.push_back(curr);
            indices.push_back(curr + 1);
            indices.push_back(next);

            indices.push_back(next);
            indices.push_back(curr + 1);
            indices.push_back(next + 1);
        }
    }

    // 將頂點、法線和貼圖座標存入模型
    for (int i : indices) {
        vase->positions.push_back(positions[i * 3 + 0]);
        vase->positions.push_back(positions[i * 3 + 1]);
        vase->positions.push_back(positions[i * 3 + 2]);

        vase->normals.push_back(normals[i * 3 + 0]);
        vase->normals.push_back(normals[i * 3 + 1]);
        vase->normals.push_back(normals[i * 3 + 2]);

        vase->texcoords.push_back(texcoords[i * 2 + 0]);
        vase->texcoords.push_back(texcoords[i * 2 + 1]);
    }

    GLuint texture = createTexture("../assets/models/Vase/Vase2.jpg");
    if (!texture) {
        std::cerr << "Failed to load Vase2 texture!" << std::endl;
        delete vase;
        return nullptr;
    }
    vase->textures.push_back(texture);

    vase->numVertex = vase->positions.size() / 3;
    vase->drawMode = GL_TRIANGLES;

    return vase;
}

Model* createBezierVaseBottomModel() {
  /* TODO#1-5: Add a vase bottom surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase2.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  const int segments = 36;  // 圆周分段数
    float radius = 0.95f * 1.0f;  // 底部半径
    float y = 0.005f;         // 底部略高于地面

    Model* m = new Model();
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<int> indices;

    // 添加底部中心点
    positions.push_back(0.0f);
    positions.push_back(y);
    positions.push_back(0.0f);

    normals.push_back(0.0f);
    normals.push_back(1.0f);  // 法線朝上
    normals.push_back(0.0f);

    texcoords.push_back(0.5f);
    texcoords.push_back(0.5f);

    // 添加圆周顶点
    for (int s = 0; s <= segments; ++s) {
        float theta = static_cast<float>(s) / segments * 2.0f * M_PI;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);

        normals.push_back(0.0f);
        normals.push_back(1.0f);  // 法线朝上
        normals.push_back(0.0f);

        texcoords.push_back(0.5f + 0.5f * cos(theta));
        texcoords.push_back(0.5f + 0.5f * sin(theta));
    }

    // 生成索引数据
    for (int s = 0; s < segments; ++s) {
        int centerIndex = 0;      // 中心点索引
        int currentIndex = s + 1; // 当前圆周顶点索引
        int nextIndex = s + 2;    // 下一圆周顶点索引

        indices.push_back(centerIndex);  // 中心点
        indices.push_back(nextIndex);    // 下一顶点
        indices.push_back(currentIndex); // 当前顶点
    }

    // 转换顶点数据到模型
    for (int i : indices) {
        m->positions.push_back(positions[i * 3 + 0]);
        m->positions.push_back(positions[i * 3 + 1]);
        m->positions.push_back(positions[i * 3 + 2]);

        m->normals.push_back(normals[i * 3 + 0]);
        m->normals.push_back(normals[i * 3 + 1]);
        m->normals.push_back(normals[i * 3 + 2]);

        m->texcoords.push_back(texcoords[i * 2 + 0]);
        m->texcoords.push_back(texcoords[i * 2 + 1]);
    }

    // 添加纹理
    GLuint texture = createTexture("../assets/models/Vase/Vase2.jpg");
    if (!texture) {
        std::cerr << "Failed to load Vase2 texture!" << std::endl;
        delete m;
        return nullptr;
    }
    m->textures.push_back(texture);

    // 设置顶点数和绘图模式
    m->numVertex = static_cast<int>(m->positions.size() / 3);
    m->drawMode = GL_TRIANGLES;

    return m;
}

void loadModels() {
  /* TODO#2-2: Push the model to ctx.models
   *        1. Comment out this example models
   *        2. Push your model to ctx.models
   * Note:
   *    You can refer to the context class in context.h and model class in model.h
   * Hint:
        ctx.models.push_back();
   */
  ctx.models.clear();

  // 1. 加载骰子模型
  Model* dice = createDice();
  if (dice) ctx.models.push_back(dice);

  // 2. 加载瓶子模型
  Model* bottle = createBottle();
  if (bottle) ctx.models.push_back(bottle);

  // 3. 加载平面模型
  Model* plane = createPlane();
  if (plane) ctx.models.push_back(plane);

  // 4. 加载花瓶外部模型
  Model* vaseOuter = createBezierVaseModel();
  if (vaseOuter) ctx.models.push_back(vaseOuter);

  // 5. 加载花瓶内部模型
  Model* vaseInner = createBezierVaseInnerModel();
  if (vaseInner) ctx.models.push_back(vaseInner);

  // 6. 加载花瓶底部模型
  Model* vaseBottom = createBezierVaseBottomModel();
  if (vaseBottom) ctx.models.push_back(vaseBottom);

  // reference model code: 
  // Model* m = new Model();
  // float pos[] = {-1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1};
  // float nor[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
  // float tx[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};
  // for (int i = 0; i < 12; i++) {
  //   m->positions.push_back(pos[i]);
  //   m->normals.push_back(nor[i]);
  //   if (i < 8) m->texcoords.push_back(tx[i]);
  // }
  // m->textures.push_back(createTexture("../assets/models/Vase/Vase2.jpg"));
  // m->numVertex = 4;
  // m->drawMode = GL_QUADS;
  
  // ctx.models.push_back(m);
}

void setupObjects() {
  /* TODO#2-2: Set up the object by the model vector
   *          1. Comment out this example object
   *          2. push your models in the ctx.models to ctx.objects
   * Note:
   *    You can refer to the context class in context.h and objects structure in model.h
   * Hint:
   *    ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.5, 0.4, 3))));
   *    (*ctx.objects.rbegin())->material = mFlatwhite;
   */
  // ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0, 0.0, 0.0))));

  ctx.objects.clear();

  // dice (modelIndex = 0)
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(7.0, 0.5, 1.5))));
  (*ctx.objects.rbegin())->material = mFlatwhite;  // set up material = mFlatwhite

  // bottle (modelIndex = 1)
  ctx.objects.push_back(new Object(1, glm::translate(glm::identity<glm::mat4>(), glm::vec3(2.0, 0.0, 1.5))));
  (*ctx.objects.rbegin())->material = mShinyred;  // set up material = mShinyred

  // plane (modelIndex = 2)
  ctx.objects.push_back(new Object(2, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096f, 0.0f, 2.56f))));
  (*ctx.objects.rbegin())->material = mFlatwhite;

  // outer vase (modelIndex = 3)
  ctx.objects.push_back(new Object(3, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096f, 0.05, 2.56f))));
  (*ctx.objects.rbegin())->material = mFlatwhite;

  // inner vase (modelIndex = 4)
  ctx.objects.push_back(new Object(4, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096f, 0.05, 2.56f))));
  (*ctx.objects.rbegin())->material = mFlatwhite;

  // inner bottom vase (modelIndex = 5)
  ctx.objects.push_back(new Object(5, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096f, 0.05, 2.56f))));
  (*ctx.objects.rbegin())->material = mFlatwhite;
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW2 - `your student id`"
   *         Ex. HW2 - 311550000
   */
  glfwSetWindowTitle(window, "HW2 - 111550006");

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbacks use
  glfwSetWindowUserPointer(window, &camera);
  ctx.camera = &camera;
  ctx.window = window;

  loadMaterial();
  loadModels();
  loadPrograms();
  setupObjects();

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);

    ctx.spotLightDirection = glm::normalize(glm::vec3(3, 0.3, 3) - ctx.spotLightPosition);
    ctx.pointLightPosition = glm::vec3(6 * glm::cos(glm::radians(ctx._pointLightPosisionDegree)), 3.0f,
                                       6 * glm::sin(glm::radians(ctx._pointLightPosisionDegree)));
    ctx.programs[ctx.currentProgram]->doMainLoop();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_K:
        ctx._pointLightPosisionDegree += 1.0f;
        break;
      case GLFW_KEY_L:
        ctx._pointLightPosisionDegree -= 1.0f;
        break;
      case GLFW_KEY_I:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x - 0.1f, -2.0f, 8.0f);
        break;
      case GLFW_KEY_O:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x + 0.1f, -2.0f, 8.0f);
        break;

      default:
        break;
    }
    return;
  }
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F9: {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
          // Show the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
          // Hide the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
      }
      case GLFW_KEY_1:
        ctx.currentProgram = (ctx.currentProgram + 1) % ctx.programs.size();
        break;
      //case GLFW_KEY_2:
      //  ctx.currentProgram = 1;
      //  break;
      //case GLFW_KEY_3:
      //  ctx.currentProgram = 2; 
      case GLFW_KEY_2:
        ctx.directionLightEnable = !ctx.directionLightEnable;
        break;
      case GLFW_KEY_3:
        ctx.pointLightEnable = !ctx.pointLightEnable;
        break;
      case GLFW_KEY_4:
        ctx.spotLightEnable = !ctx.spotLightEnable;
        break;
      case GLFW_KEY_H:
        ctx.pointLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_J:
        ctx.pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
      case GLFW_KEY_Y:
        ctx.spotLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_U:
        ctx.spotLightColor = glm::vec3(0.0f, 0.8f, 0.0f);
        break;
      default:
        break;
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}
