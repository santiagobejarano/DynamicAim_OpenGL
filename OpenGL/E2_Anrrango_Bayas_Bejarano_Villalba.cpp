#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>
#include <vector>
#include <random>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int loadTexture(const char* path);
void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4);
void drawDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& deagle);
void drawBayonet(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& bayonet);
void drawSkybox(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& skybox);

void shootRayFromCamera(Camera& camera, Model& target, glm::mat4& targetModelMatrix);
bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
bool intersectsTargetRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const Model& model);
void checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::mat4& targetModelMatrix, const Model& target);
void repositionTarget(glm::mat4& modelMatrix);

// settings FHD
//const unsigned int SCR_WIDTH = 1920; 
//const unsigned int SCR_HEIGHT = 1080;

// settings 2K
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// Límites para el movimiento de la cámara en el mapa 100x100
const float X_MIN_LIMIT = 0.0f;
const float X_MAX_LIMIT = 100.0f;
const float Z_MIN_LIMIT = 0.0f;
const float Z_MAX_LIMIT = 100.0f;

// camera
//Camera camera(glm::vec3(0.0f, 3.2f, 0.0f)); //Modelar armas
Camera camera(glm::vec3(10.0f, 3.2f, 50.0f)); // Armas listas
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// visualizar armas
bool showDeagle = false;
bool showM4 = false;
bool showBayonet = true;

Model target;
glm::mat4 targetModelMatrix = glm::mat4(1.0f);


int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");

    // load models
    Model deagle("model/deagle/deagle.gltf");
    Model m4("model/m4/m4.gltf");
    Model skybox("model/skybox/skybox.gltf");
    Model logo("model/logo/logo.gltf");
    Model bayonet("model/bayonet/bayonet.gltf");
    
    target = Model("model/target/target.gltf");

    // model target
    targetModelMatrix = glm::translate(targetModelMatrix, glm::vec3(10.0f, 2.0f, 50.0f)); // Posición inicial
    targetModelMatrix = glm::rotate(targetModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    targetModelMatrix = glm::rotate(targetModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    targetModelMatrix = glm::scale(targetModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f)); // Escala inicial

    float vertices[] = {
       // Coordenadas XYZ         // Normales XYZ           // Coordenadas de textura UV
       // Cara trasera
       -0.6f, -0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.5343f, 0.51f,    // Esquina inferior izquierda
        0.6f, -0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.7291f, 0.3264f,    // Esquina inferior derecha    
        0.6f,  0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.7291f, 0.3264f, // Esquina superior derecha              
        0.6f,  0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.7291f, 0.3264f, // Esquina superior derecha
       -0.6f,  0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.5343f, 0.51f, // Esquina superior izquierda
       -0.6f, -0.6f, -0.6f,       0.0f,  0.0f, -1.0f,       0.5343f, 0.51f,    // Esquina inferior izquierda                

       // Cara frontal
       -0.6f, -0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.5343f, 0.51f,    // Esquina inferior izquierda
        0.6f,  0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.7291f, 0.3264f, // Esquina superior derecha
        0.6f, -0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.7291f, 0.3264f,    // Esquina inferior derecha        
        0.6f,  0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.7291f, 0.3264f, // Esquina superior derecha
       -0.6f, -0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.5343f, 0.51f,    // Esquina inferior izquierda
       -0.6f,  0.6f,  0.6f,       0.0f,  0.0f,  1.0f,       0.5343f, 0.3264f, // Esquina superior izquierda        

       // Cara izquierda
       -0.6f,  0.6f,  0.6f,       -1.0f, 0.0f,  0.0f,       0.7291f, 0.3264f, // Esquina superior derecha
       -0.6f, -0.6f, -0.6f,       -1.0f, 0.0f,  0.0f,       0.925f, 0.51f,      // Esquina inferior izquierda
       -0.6f,  0.6f, -0.6f,       -1.0f, 0.0f,  0.0f,       0.925f, 0.3264f,   // Esquina superior izquierda       
       -0.6f, -0.6f, -0.6f,       -1.0f, 0.0f,  0.0f,       0.925f, 0.51f,      // Esquina inferior izquierda
       -0.6f,  0.6f,  0.6f,       -1.0f, 0.0f,  0.0f,       0.7291f, 0.3264f, // Esquina superior derecha
       -0.6f, -0.6f,  0.6f,       -1.0f, 0.0f,  0.0f,       0.7291f, 0.51f,    // Esquina inferior derecha

       // Cara derecha
        0.6f,  0.6f,  0.6f,       1.0f,  0.0f,  0.0f,       0.5343f, 0.3264f, // Esquina superior izquierda
        0.6f,  0.6f, -0.6f,       1.0f,  0.0f,  0.0f,       0.3385f, 0.3264f, // Esquina superior derecha      
        0.6f, -0.6f, -0.6f,       1.0f,  0.0f,  0.0f,       0.3385f, 0.51f,    // Esquina inferior derecha          
        0.6f, -0.6f, -0.6f,       1.0f,  0.0f,  0.0f,       0.3385f, 0.51f,    // Esquina inferior derecha
        0.6f, -0.6f,  0.6f,       1.0f,  0.0f,  0.0f,       0.5343f, 0.51f,    // Esquina inferior izquierda
        0.6f,  0.6f,  0.6f,       1.0f,  0.0f,  0.0f,       0.5343f, 0.3264f, // Esquina superior izquierda

        // Cara inferior          
        -0.6f, -0.6f, -0.6f,       0.0f, -1.0f,  0.0f,       0.5343f, 0.7435f, // Esquina superior derecha
         0.6f, -0.6f,  0.6f,       0.0f, -1.0f,  0.0f,       0.7291f, 0.51f,    // Esquina inferior izquierda
         0.6f, -0.6f, -0.6f,       0.0f, -1.0f,  0.0f,       0.7291f, 0.7435f, // Esquina superior izquierda        
         0.6f, -0.6f,  0.6f,       0.0f, -1.0f,  0.0f,       0.7291f, 0.51f,    // Esquina inferior izquierda
        -0.6f, -0.6f, -0.6f,       0.0f, -1.0f,  0.0f,       0.5343f, 0.7435f, // Esquina superior derecha
        -0.6f, -0.6f,  0.6f,       0.0f, -1.0f,  0.0f,       0.5343f, 0.51f,    // Esquina inferior derecha

        // Cara superior
        -0.6f,  0.6f, -0.6f,       0.0f,  1.0f,  0.0f,       0.5343f, 0.51f,  // Esquina superior izquierda
         0.6f,  0.6f, -0.6f,       0.0f,  1.0f,  0.0f,       0.925f, 0.51f,  // Esquina superior derecha
         0.6f,  0.6f,  0.6f,       0.0f,  1.0f,  0.0f,       0.7291f, 0.3264f, // Esquina inferior derecha                 
         0.6f,  0.6f,  0.6f,       0.0f,  1.0f,  0.0f,       0.7291f, 0.3264f, // Esquina inferior derecha
        -0.6f,  0.6f,  0.6f,       0.0f,  1.0f,  0.0f,       0.5343f, 0.51f,  // Esquina inferior izquierda  
        -0.6f,  0.6f, -0.6f,       0.0f,  1.0f,  0.0f,       0.5343f, 0.3264f  // Esquina superior izquierda  
    };

    // SUELO PROVISIONAL
    // Optimización para pruebas funcionales
    // Falta modelo de isla
    glm::vec3 cubePositions[10000];
    int index = 0; // Índice para llenar el arreglo
    for (int j = 0; j < 100; j++) { // 10 filas
        for (int i = 0; i < 100; i++) { // 10 cubos por fila
            cubePositions[index++] = glm::vec3(i, 0.0f, j); // Ajusta 'i' y 'j' para cambiar la columna y la fila, respectivamente
        }
    }

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int diffuseMap1 = loadTexture("textures/concreto.jpg");

    // shader configuration
    ourShader.use();
    ourShader.setInt("material.diffuse1", 0); // Texture unit 0
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 7; //Optional. Modify the speed of the camera

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        glActiveTexture(GL_TEXTURE0); // Cambia a la unidad de textura de los cubos
        glBindTexture(GL_TEXTURE_2D, diffuseMap1);       

        // view / projection / transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glBindVertexArray(cubeVAO);

        // Dibujar 10.000 cubos, para 
        for (unsigned int i = 0; i < 10000; i++) {
            // Transformación del mundo para este cubo
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            ourShader.setMat4("model", model);
            // Dibuja el cubo
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //TARGET
        /*glm::mat4 targetMatrix = glm::mat4(1.0f);
        targetMatrix = glm::translate(targetMatrix, glm::vec3(10.0f, 2.0f, 50.0f));
        targetMatrix = glm::rotate(targetMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        targetMatrix = glm::rotate(targetMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        targetMatrix = glm::scale(targetMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setMat4("model", targetMatrix);
        target.Draw(ourShader);*/

        // SKYBOX
        drawSkybox(ourShader, view, projection, skybox);

        // Decidir qué arma dibujar
        // Dibujar Deagle
        if (showDeagle) { 
            drawDeagle(ourShader, view, projection, deagle);
        }
        // Dibujar M4
        else if (showM4) {
            drawM4(ourShader, view, projection, m4);
        }
        // Dibujar Bayonet
        else if (showBayonet) {
            drawBayonet(ourShader, view, projection, bayonet);
        }   

        // Verificar la acción de disparo
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            shootRayFromCamera(camera, target, targetModelMatrix);
        }

        ourShader.setMat4("model", targetModelMatrix);
        target.Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float currentCameraY = camera.Position.y; // Guardar la posición Y actual de la cámara
    glm::vec3 newPosition = camera.Position;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Mantener la altura constante
    camera.Position.y = currentCameraY; // Restablecer la posición Y de la cámara a su valor original

    // Alternar entre las armas
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        showDeagle = false;
        showM4 = true;
        showBayonet = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        showDeagle = true;
        showM4 = false;
        showBayonet = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        showDeagle = false;
        showM4 = false;
        showBayonet = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // El botón izquierdo del mouse fue presionado, realizar disparo
        glm::mat4 targetModelMatrix; // Deberías tener esta matriz definida en algún lugar relevante
        shootRayFromCamera(camera, target, targetModelMatrix);
    }

    // Restricciones de movimiento en el eje Y ya aplicadas previamente
    // Restricciones en los ejes X y Z
    //newPosition.x = glm::clamp(newPosition.x, X_MIN_LIMIT, X_MAX_LIMIT);
    //newPosition.z = glm::clamp(newPosition.z, Z_MIN_LIMIT, Z_MAX_LIMIT);

    // Aplicar la nueva posición
    //camera.Position = newPosition;
}


void shootRayFromCamera(Camera& camera, Model& target, glm::mat4& targetModelMatrix) {
    glm::vec3 rayOrigin = camera.Position;
    glm::vec3 rayDirection = camera.Front; // Asumiendo que 'Front' es la dirección en la que la cámara está mirando
    checkRayIntersection(rayOrigin, rayDirection, targetModelMatrix, target);
}

bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
    const float EPSILON = 0.0000001f;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = glm::cross(rayDir, edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // El rayo es paralelo al triángulo.
    f = 1.0 / a;
    s = rayOrigin - v0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(rayDir, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // En este punto sabemos que hay una intersección en la línea del rayo, pero no si el rayo realmente la intersecta.
    t = f * glm::dot(edge2, q);
    if (t > EPSILON) // Intersección con el rayo
        return true;

    return false;
}

bool intersectsTargetRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const Model& model) {
    for (const Mesh& mesh : model.meshes) {
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
            glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
            glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;

            float t = 0.0f;
            if (intersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, t)) {
                return true; // Intersecta
            }
        }
    }
    return false; // No intersecta
}

void checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::mat4& targetModelMatrix, const Model& target) {
    if (intersectsTargetRayTriangle(rayOrigin, rayDirection, target)) {
        // Intersecta con el target, reposiciona el target usando la matriz de modelo
        repositionTarget(targetModelMatrix);
    }
}

glm::vec3 aiVector3DToGlmVec3(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

void repositionTarget(glm::mat4& modelMatrix) {
    std::random_device rd; // Obtener un número aleatorio del dispositivo
    std::mt19937 gen(rd()); // Semilla
    std::uniform_real_distribution<> dis(-10.0, 10.0); // Rango

    // Generar nueva posición
    glm::vec3 newPosition(dis(gen), dis(gen), dis(gen));

    modelMatrix = glm::translate(modelMatrix, glm::vec3(10.0f, 2.0f, 50.0f));

    // Crear una nueva matriz de modelo para el target basada en la nueva posición
    modelMatrix = glm::mat4(1.0f); // Inicializar a matriz identidad
    modelMatrix = glm::translate(modelMatrix, newPosition); // Aplicar la nueva posición
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

    // Asumiendo que no necesitas cambiar la orientación o escala del target. Si lo necesitas, aquí puedes aplicarlo.
}







// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Dibujar Deagle
void drawDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& deagle) {
    glm::mat4 pistolaMatrix = glm::mat4(1.0f);
    pistolaMatrix = glm::translate(pistolaMatrix, glm::vec3(0.21f, -0.4f, -0.38f));
    pistolaMatrix = glm::rotate(pistolaMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pistolaMatrix = glm::rotate(pistolaMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    pistolaMatrix = glm::scale(pistolaMatrix, glm::vec3(0.06f));
    pistolaMatrix = glm::inverse(view) * pistolaMatrix;
    shader.setMat4("model", pistolaMatrix);
    deagle.Draw(shader);
}

// Dibujar M4
void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4) {
    glm::mat4 armaMatrix = glm::mat4(1.0f);
    armaMatrix = glm::translate(armaMatrix, glm::vec3(0.28f, -0.7f, -0.1f));
    armaMatrix = glm::rotate(armaMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    armaMatrix = glm::scale(armaMatrix, glm::vec3(0.04f));
    armaMatrix = glm::inverse(view) * armaMatrix;
    shader.setMat4("model", armaMatrix);
    m4.Draw(shader);
}

// Dibujar Bayonet
void drawBayonet(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& bayonet) {
    glm::mat4 cuchilloMatrix = glm::mat4(1.0f);
    cuchilloMatrix = glm::translate(cuchilloMatrix, glm::vec3(0.38f, -0.4f, -1.0f));
    cuchilloMatrix = glm::rotate(cuchilloMatrix, glm::radians(215.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cuchilloMatrix = glm::rotate(cuchilloMatrix, glm::radians(18.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cuchilloMatrix = glm::scale(cuchilloMatrix, glm::vec3(0.05f));
    cuchilloMatrix = glm::inverse(view) * cuchilloMatrix;
    shader.setMat4("model", cuchilloMatrix);
    bayonet.Draw(shader);
}

// Dibujar Skybox
void drawSkybox(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& skybox) {
    glm::mat4 skyboxMatrix = glm::mat4(1.0f);
    skyboxMatrix = glm::translate(skyboxMatrix, glm::vec3(50.0f, 0.0f, 50.0f));
    skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    skyboxMatrix = glm::scale(skyboxMatrix, glm::vec3(1000.0f));
    shader.setMat4("model", skyboxMatrix);
    skybox.Draw(shader);
}