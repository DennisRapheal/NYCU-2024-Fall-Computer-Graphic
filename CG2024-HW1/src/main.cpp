#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm/glm.hpp>

#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define RADIAN_TO_ANGLE(x) (float)((x)*180.0f / M_PI)

#define CIRCLE_SEGMENT 64
#define SECTOR 36
#define STACK 18

#define ROTATE_SPEED 1.0f
#define MOVING_SPEED ROTATE_SPEED / 20.f
#define BALL_MOVING_SPEED 0.05f
#define SWING_SPEED 2.0f
#define ROLLING_SPEED 5.0f

#define CONSTRAIN_ANGLE -45

#define SILVER 0.75, 0.75, 0.75
#define CYAN 0, 1, 1
#define PURPLE 1, 0, 1

#define TOLERATE 0.75f
#define DISTANCE_TOLERANCE 3
enum class angle { NONE = 0, CLOCKWISE = -1, COUNTERCLOCKWISE = 1 };
enum class scalar { NONE = 0, MINUS = -1, PLUS = 1 };

// to check the putter is swing or not
bool isSwing = false;
// to check the ball is hit or not
bool isHit = false;

// Position of the hole on the plane (adjust as needed)
glm::vec3 holepos(0.0f, 0.0f, 0.0f); 

// Radius of the hole (adjust as needed)
float holeRadius = 0.5f; 

// Falling speed for the ball
const float FALLING_SPEED = 0.05f;

// Flag to check if the ball is falling
bool isFalling = false;

// the scalar for the putter translation
scalar delta_xzpos = scalar::NONE;
// the scalar for the ball translation
scalar delta_ballpos = scalar::NONE;
// the scalar for the ball rotation
angle delta_ball_rotate = angle::NONE;
// the scalar for the putter swing
angle delta_x_rotate = angle::NONE;
// the scalar for the putter yaw
angle delta_y_rotate = angle::NONE;

// the angle for the ball rotation
float ball_rotate = 0.0f;
// the angle for the putter swing
float x_rotate = 0.0f;
// the angle for the yaw rotation
float y_rotate = 0.0f;

// the position of the putter
glm::vec3 xzpos(0.0f, 0.0f, 0.0f);
// the forward vector for putter
glm::vec3 forward_vector(0.0f, 0.0f, 1.0f);
// the forward vector for the ball translation
glm::vec3 ball_forward(0.0f, 0.0f, 1.0f);
// x,y,z coordinate for ball rotation
glm::vec3 ball_rotate_normal(0, 1, 0);
// the position of the ball
glm::vec3 ballpos(2.0f, 0.25f, 2.0f);
// the position of the ball when it is hit at the start
glm::vec3 startpos(0, 0, 0);

// all the former rotations for the ball
glm::mat4 currentRotation = glm::identity<glm::mat4>();

void resizeCallback(GLFWwindow* window, int width, int height) {
	OpenGLContext::framebufferResizeCallback(window, width, height);
	auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (ptr) {
	ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
	}
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
	if (action == GLFW_REPEAT) return;

  // Press ESC to close the window.
if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
}
  /* TODO#4-1: Detect key-events, perform all the controls for the golfing
   *       1. Use switch && case to find the key you want.
   *       2. Define and modify some global variable to trigger update in rendering loop
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish rendering your putter and golfball first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */
    switch (key) {
      case GLFW_KEY_UP:
        if (action == GLFW_PRESS) {
          delta_xzpos = scalar::PLUS;
        } else if (action == GLFW_RELEASE) {
          delta_xzpos = scalar::NONE;
        }
        break;
      case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS) {
          delta_xzpos = scalar::MINUS;
        } else if (action == GLFW_RELEASE) {
          delta_xzpos = scalar::NONE;
        }
        break;
      case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS) {
          delta_y_rotate = angle::COUNTERCLOCKWISE;
        } else if (action == GLFW_RELEASE) {
          delta_y_rotate = angle::NONE;
        }
        break;
      case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) {
          delta_y_rotate = angle::CLOCKWISE;
        } else if (action == GLFW_RELEASE) {
          delta_y_rotate = angle::NONE;
        }
        break;
      case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
          delta_x_rotate = angle::CLOCKWISE; // Swing back
        } else if (action == GLFW_RELEASE) {
          delta_x_rotate = angle::COUNTERCLOCKWISE; // Swing forward
        }
        break;
      default:
        break;
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
  glfwWindowHint(GLFW_STENCIL_BITS, 8); // Add this line
#ifdef __APPLE__
  // MacOS need explicit request legacy support
	OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  // OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
	GLFWwindow* window = OpenGLContext::getWindow();
	/* TODO#0: Change window title to "HW1 - `your student id`"
	*        Ex. HW1 - 311550000
	*/
	glfwSetWindowTitle(window, "HW1 - 111550006");
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

void drawUnitSphere() {
	/* TODO#2-1: Render a unit sphere
	* Hint:
	*       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
	*       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
	*       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
	*       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
	*       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
	* Note:
	*       You can refer to ppt "Draw Sphere" page and `SECTOR` and `STACK`
	*       You can use color `CYAN` and `PURPLE`
	*       You should set normal for lighting
	*       You should use GL_TRIANGLES_STRIP
	*/
	float x, y, z;                  // Vertex position
	float nx, ny, nz;               // Vertex normal
	float sectorStep = 2 * M_PI / SECTOR;
	float stackStep = M_PI / STACK;
	float sectorAngle, stackAngle;
	for(int i = 0; i < STACK; ++i) {
		stackAngle = M_PI / 2 - i * stackStep;          // Starting from pi/2 to -pi/2
		float stackAngleNext = stackAngle - stackStep;  // Unit sphere r = 1

		glBegin(GL_TRIANGLE_STRIP);
      for(int j = 0; j <= SECTOR; ++j) {
        sectorAngle = j * sectorStep;             // 0 to 2pi

        if(i < STACK/2) {
          glColor3f(CYAN); // CYAN
        } else {
          glColor3f(PURPLE); // PURPLE
        }
        // Current stack vertex
        x = cosf(stackAngle) * cosf(sectorAngle);
        y = cosf(stackAngle) * sinf(sectorAngle);
        z = sinf(stackAngle);
        nx = x;
        ny = y;
        nz = z;
        glNormal3f(nx, ny, nz); // how light interacts with the surface
        glVertex3f(x, y, z);    // set up the connected triangle in GL_TRIANGLE_STRIP

        // Next stack vertex
        x = cosf(stackAngleNext) * cosf(sectorAngle);
        y = cosf(stackAngleNext) * sinf(sectorAngle);
        z = sinf(stackAngleNext);
        nx = x;
        ny = y;
        nz = z;
        glNormal3f(nx, ny, nz);
        glVertex3f(x, y, z);
      }
		glEnd();
	}
}

void drawUnitCylinder() {
   /* TODO#2-2: Render a unit cylinder
   * Hint:
   *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
   *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
   *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
   * Note:
   *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
   *       You can use color `SILVER`
   *       You should set normal for lighting
   *       You should use GL_TRIANGLES
   */
    /* Draws a unit cylinder centered at the origin, extending from y = -0.5f to y = 0.5f */
    const float sectorStep = 2.0f * M_PI / CIRCLE_SEGMENT;
    
    // Draw the side of cylinder using GL_TRIANGLES
    glBegin(GL_TRIANGLES);
      glColor3f(SILVER);
      for(int i = 0; i < CIRCLE_SEGMENT; i++) {
          float angle1 = i * sectorStep;
          float angle2 = (i + 1) * sectorStep;
          float x1 = cosf(angle1);
          float z1 = sinf(angle1);
          float x2 = cosf(angle2); 
          float z2 = sinf(angle2);

          // First triangle of the quad
          glNormal3f(x1, 0.0f, z1); 
          glVertex3f(x1, 0.5f, z1); 
          glNormal3f(x1, 0.0f, z1);  
          glVertex3f(x1, -0.5f, z1);
          glNormal3f(x2, 0.0f, z2);    
          glVertex3f(x2, 0.5f, z2);     

          // Second triangle of the quad
          glNormal3f(x1, 0.0f, z1);  
          glVertex3f(x1, -0.5f, z1);
          glNormal3f(x2, 0.0f, z2);    
          glVertex3f(x2, -0.5f, z2);    
          glNormal3f(x2, 0.0f, z2);    
          glVertex3f(x2, 0.5f, z2);     
      }
    glEnd();

    // Draw bottom circle using GL_TRIANGLES
    glBegin(GL_TRIANGLES);
      glColor3f(SILVER);
      glNormal3f(0.0f, -1.0f, 0.0f);
      for(int i = 0; i < CIRCLE_SEGMENT; i++) {
          float angle1 = i * sectorStep;
          float angle2 = (i + 1) * sectorStep;
          float x1 = cosf(angle1);
          float z1 = sinf(angle1);
          float x2 = cosf(angle2);
          float z2 = sinf(angle2);

          glVertex3f(0.0f, -0.5f, 0.0f);
          glVertex3f(x1, -0.5f, z1);
          glVertex3f(x2, -0.5f, z2);
      }
    glEnd();

    // Draw top circle using GL_TRIANGLES
    glBegin(GL_TRIANGLES);
      glColor3f(SILVER);
      glNormal3f(0.0f, 1.0f, 0.0f);
      for(int i = 0; i < CIRCLE_SEGMENT; i++) {
          float angle1 = i * sectorStep;
          float angle2 = (i + 1) * sectorStep;
          float x1 = cosf(angle1);
          float z1 = sinf(angle1);
          float x2 = cosf(angle2);
          float z2 = sinf(angle2);

          glVertex3f(0.0f, 0.5f, 0.0f);
          glVertex3f(x1, 0.5f, z1);
          glVertex3f(x2, 0.5f, z2);
      }
    glEnd();
}

void drawHoleCylinder() {
    const float sectorStep = 2.0f * M_PI / CIRCLE_SEGMENT;
    float depth = 0.3f; // Depth of the hole cylinder
    // Top and bottom y-coordinates
    float yTop = holepos.y;
    float yBottom = holepos.y - depth;
    glBegin(GL_TRIANGLES);
        for(int i = 0; i < CIRCLE_SEGMENT; i++) {
            float angle1 = i * sectorStep;
            float angle2 = (i + 1) * sectorStep;

            // Calculate normals
            float nx1 = cosf(angle1);
            float nz1 = sinf(angle1);
            float nx2 = cosf(angle2);
            float nz2 = sinf(angle2);

            // Calculate vertices relative to holepos and holeRadius
            float x1 = holepos.x + holeRadius * nx1;
            float z1 = holepos.z + holeRadius * nz1;
            float x2 = holepos.x + holeRadius * nx2;
            float z2 = holepos.z + holeRadius * nz2;

            // First triangle of the quad
            glNormal3f(nx1, 0.0f, nz1);
            glVertex3f(x1, yTop, z1);

            glNormal3f(nx1, 0.0f, nz1);
            glVertex3f(x1, yBottom, z1);

            glNormal3f(nx2, 0.0f, nz2);
            glVertex3f(x2, yTop, z2);

            // Second triangle of the quad
            glNormal3f(nx1, 0.0f, nz1);
            glVertex3f(x1, yBottom, z1);

            glNormal3f(nx2, 0.0f, nz2);
            glVertex3f(x2, yBottom, z2);

            glNormal3f(nx2, 0.0f, nz2);
            glVertex3f(x2, yTop, z2);
        }
    glEnd();

    // **Draw the top circle (cap) of the cylinder**
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(SILVER);
        glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal
        glVertex3f(holepos.x, holepos.y, holepos.z); // Center of the circle
        for(int i = 0; i <= CIRCLE_SEGMENT; ++i) {
            float angle = i * sectorStep;
            float x = holepos.x + holeRadius * cosf(angle);
            float z = holepos.z + holeRadius * sinf(angle);
            glVertex3f(x, holepos.y, z);
        }
    glEnd();

    // **Draw the bottom circle (cap) of the cylinder**
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(SILVER);
        glNormal3f(0.0f, -1.0f, 0.0f); // Downward normal
        glVertex3f(holepos.x, yBottom, holepos.z); // Center of the bottom circle
        for(int i = 0; i <= CIRCLE_SEGMENT; ++i) {
            float angle = i * sectorStep;
            float x = holepos.x + holeRadius * cosf(angle);
            float z = holepos.z + holeRadius * sinf(angle);
            glVertex3f(x, yBottom, z);
        }
    glEnd();
}


void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);

  // for solid
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 5, 10));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Enable depth and stencil testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_STENCIL_TEST);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif
    /* TODO#4-1: Update putter
     *       1. Update xzpos with forward_vector.
     *       2. Update y_rotate.
     *       3. Update x_rotate(Swing). Remember that the constrain of `CONSTRAIN_ANGLE`
     *       4. Calculate forward_vector for next rendering loop.
     *
     * Hint:
     *      glm::normalize (
     * Note:
     *       You can use `ROTATE_SPEED` and `MOVING_SPEED` and `SWING_SPEED`
     * as the speed constant. If the rotate/movement/swing speed is too slow or too fast, please change `ROTATE_SPEED`
     * or `MOVING_SPEED` or `SWING_SPEED` value. You should finish keyCallback first and use variables include
     * "delta_" to update.
     */
    // 1. Update xzpos with forward_vector.
    if (delta_xzpos != scalar::NONE) {
      float move_step = MOVING_SPEED * static_cast<int>(delta_xzpos);
      xzpos += move_step * glm::vec3(forward_vector.x, 0.0f, forward_vector.z);
    }

    // 2. Update y_rotate.
    if (delta_y_rotate != angle::NONE) {
      y_rotate += ANGLE_TO_RADIAN(ROTATE_SPEED) * static_cast<int>(delta_y_rotate);
    }

    // 3. Update x_rotate(Swing). Remember that the constrain of `CONSTRAIN_ANGLE`
    if (delta_x_rotate != angle::NONE) {
      float swing_step = ANGLE_TO_RADIAN(SWING_SPEED) * static_cast<int>(delta_x_rotate);
      x_rotate += swing_step;
      // Clamp x_rotate between CONSTRAIN_ANGLE and 0
      if (x_rotate < ANGLE_TO_RADIAN(CONSTRAIN_ANGLE))
        x_rotate = ANGLE_TO_RADIAN(CONSTRAIN_ANGLE);
      if (x_rotate > 0)
        x_rotate = 0;
    }

    // 4. Calculate forward_vector for next rendering loop.
    forward_vector = glm::normalize(glm::vec3(sinf(y_rotate), 0.0f, cosf(y_rotate)));
    
    /* TODO#4-2: Hit detection and Update golfball
     *       1. Find the position of the hitting part and the ball to calculate distance
     *       2. Determine whether the ball is hit(distance < TOLERANCE && putter is swinging)
     *          
     *       if the ball is hit:
     *       3. Update ballpos with ball_forward.
     *       4. Update ball_rotate.
     *       5. Calculate the new ball_forward with forward_vector.
     *       6. Calculate the new ball_rotate_normal
     *       7. Calculate the new startpos
     * 
     *       Implement ball stop:
     *       8. Determine whether the ball has to stop(distance >= DISTANCE_TOLERANCE)
     *       9. Update currentRotation
     * Hint:
     *      glm::mat4
     *      glm::translate
     *      glm::rotate
     *      glm::scale
     *      glm::length
     *      glm::normalize (
     * Note:
     *       You can use `BALL_MOVING_SPEED` and `ROLLING_SPEED`
     * as the speed constant. If the rotate/movement speed is too slow or too fast, please change `ROTATE_SPEED`
     * or `BALL_MOVING_SPEED` value. You should finish keyCallback first and use variables include
     * "delta_" to update.
     */
    // 1. Find the position of the hitting part and the ball to calculate distance
    glm::mat4 model = glm::mat4(1.0f); // initial an identity matrix
    model = glm::translate(model, xzpos); // creates a translation matrix based on the provided position vector and multiplies it with the existing model matrix.
    model = glm::rotate(model, y_rotate, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, x_rotate, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec4 hitting_part_local_pos = glm::vec4(0.0f, 0.25f, 0.0f, 1.0f);
    glm::vec3 hitting_part_world_pos = glm::vec3(model * hitting_part_local_pos);

    // 2. Determine whether the ball is hit(distance < TOLERATE && putter is swinging)
    float distance = glm::length(hitting_part_world_pos - ballpos);

    isSwing = (delta_x_rotate == angle::CLOCKWISE);

    if (distance < TOLERATE && isSwing && !isHit) {
        // The ball is hit
        isHit = true;
        // Calculate the new ball_forward with forward_vector.
        ball_forward = forward_vector;
        // Calculate the new ball_rotate_normal
        ball_rotate_normal = glm::normalize(glm::cross(glm::vec3(0, 1, 0), ball_forward));
        // Calculate the new startpos
        startpos = ballpos;
    }

    // Update golfball if it is hit
    if (isHit&& !isFalling) {
        // Update ballpos with ball_forward.
        ballpos += BALL_MOVING_SPEED * ball_forward;

        // Update ball_rotate.
        currentRotation = glm::rotate(currentRotation, ANGLE_TO_RADIAN(ROLLING_SPEED), ball_rotate_normal);

        // Determine whether the ball has to stop(distance >= DISTANCE_TOLERANCE)
        float moved_distance = glm::length(ballpos - startpos);
        if (moved_distance >= DISTANCE_TOLERANCE) {
            // Stop the ball
            isHit = false;
        }

        // **Check if the ball is over the hole**
        float dx = ballpos.x - holepos.x;
        float dz = ballpos.z - holepos.z;
        float distanceToHole = sqrt(dx * dx + dz * dz);

        if (distanceToHole <= holeRadius) {
            // The ball is over the hole
            isFalling = true;
            isHit = false; // Stop moving forward
        }
    }

    // **Animate the ball falling into the hole**
    if (isFalling) {
        ballpos.y -= FALLING_SPEED;

        // Optional: Stop the ball from falling indefinitely
        if (ballpos.y < -5.0f) { // Adjust as needed
            // Reset the ball position or remove it
            ballpos.y = -5.0f;
            // Optionally reset the ball to the starting position
            ballpos = glm::vec3(2.0f, 0.25f, 2.0f);
            isFalling = false;
        }
    }
    // 4-2 END
    
    // Draw the hole shape
    glPushMatrix();
      // **Step 1: Draw the hole shape into the stencil buffer**
      // Configure stencil to always pass and replace stencil value with 1 where we draw
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

      // Disable color and depth buffer writes
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);

      // Draw the hole shape (2D circle)
      drawHoleCylinder();

      // Enable color and depth buffer writes
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);

      // **Step 2: Draw the plane, but only where stencil buffer is not set**
      glStencilFunc(GL_EQUAL, 0, 0xFF); // Only draw where stencil value is 0
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Keep stencil values unchanged
    glPopMatrix();
    
   

    // Render the plane
    glPushMatrix();
        glScalef(7, 1, 7);
        glBegin(GL_TRIANGLE_STRIP);
            glColor3f(0.6f, 0.8f, 0.2f);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-5.0f, 0.0f, -5.0f);
            glVertex3f(-5.0f, 0.0f, 5.0f);
            glVertex3f(5.0f, 0.0f, -5.0f);
            glVertex3f(5.0f, 0.0f, 5.0f);
        glEnd();
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    // **Optional: Draw the hole cylinder to simulate depth**

    

// ------------------ End of Stencil Buffer Setup ------------------



    /* TODO#3-1: Render the putter
     *       1. Do rotate and translate for the putter.
     *       2. Render the hitting part.
     *       3. Render the rod part.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       You may implement drawUnitCylinder first
     *       You should try and think carefully about changing the order of rotate and translate
     *       You can just use initial value for rotate and translate the whole putter before finishing TODO#3 and TODO#4
     */

	// Putter Matrix
    glPushMatrix();

        // Apply transformations
        float pivotOffsetX = 0.0f;
        glTranslatef(xzpos.x, xzpos.y, xzpos.z);            // Move to putter's position
        glTranslatef(pivotOffsetX, 0.0f, 0.0f);
        glRotatef(RADIAN_TO_ANGLE(y_rotate), 0.0f, 1.0f, 0.0f); // Yaw rotation
        glTranslatef(-pivotOffsetX, 0.0f, 0.0f);

        float pivotOffsetY = 4.0f;
        glTranslatef(0.0f, pivotOffsetY, 0.0f);             // Translate up to pivot point
        glRotatef(RADIAN_TO_ANGLE(x_rotate), 1.0f, 0.0f, 0.0f); // Swing rotation around pivot
        glTranslatef(0.0f, -pivotOffsetY, 0.0f);  

        // Render the hitting part
        glPushMatrix();
            glTranslatef(-0.5f, 0.4f, 0.0f);
            glRotatef(RADIAN_TO_ANGLE(M_PI), 0.0f, 1.0f, 1.0f);
            glScalef(1.0f, 0.5f, 0.4f); // Adjust scaling as needed
            drawUnitCylinder();
        glPopMatrix();

        // Render the rod part
        glPushMatrix();
            glTranslatef(0.0f, 3.0f, 0.0f); // Position rod at the top of the hitting part
            glScalef(0.15f, 6.0f, 0.15f); // Adjust scaling as needed
            drawUnitCylinder();
        glPopMatrix();

    // Pop the matrix to restore the previous state
    glPopMatrix();
    /* TODO#3-2: Render the golf ball
     *       1. Do rotate and translate for the ball.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     *       glMultMatrixf(https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glMultMatrix.xml)
     *       glm::value_ptr()
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       You may implement drawUnitSphere first
     *       You should try and think carefully about changing the order of rotate and translate
     *       You can just use initial value for rotate and translate the whole putter before finishing TODO#3 and TODO#4
     */
	// Push the current matrix to the stack
	glPushMatrix();

      // Translate to the ball's current position
      glTranslatef(ballpos.x, ballpos.y, ballpos.z);

      // Scaling 
      glScalef(0.25f, 0.25f, 0.25f);

      // Apply the current rotation matrix to simulate spinning
      glMultMatrixf(glm::value_ptr(currentRotation));

      // Render the unit sphere representing the golf ball
      drawUnitSphere();
        
	// Pop the matrix to restore the previous state
	glPopMatrix();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
