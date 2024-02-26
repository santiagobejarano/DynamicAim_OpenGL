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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int loadTexture(const char* path);
void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4);
void drawDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& deagle);
void drawBayonet(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& bayonet);
void drawReticle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& reticle2d);
void drawLogo(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& logo);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

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



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
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
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    //Model ourModel("D:/LEONARDO/VisualStudio/OpenGL/OpenGL/model/baphomet/baphomet.obj");
    Model deagle("model/deagle/deagle.gltf");
    Model m4("model/m4/m4.gltf");
    Model skybox("model/skybox/skybox.gltf"); 
    Model target("model/target/target.gltf");
    Model logo("model/logo/logo.gltf");
    Model bayonet("model/bayonet/bayonet.gltf");
    Model reticle2d("model/mira4/miragreen.gltf");

    float vertices[] = {
        // Coordenadas XYZ         // Normales XYZ          // Coordenadas de textura UV
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
    

    //SUELO
    // Ajustar el tamaño de este array para contener 10,000 posiciones (100 filas de 100)

    glm::vec3 cubePositions[10000];
    // Llenar el array con posiciones para crear 100 filas de 100 cubos cada una
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
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse1", 0); // Texture unit 0
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 7; //Optional. Modify the speed of the camera

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
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

        // Modificar el bucle de dibujo para iterar sobre los 10,000 cubos
        for (unsigned int i = 0; i < 10000; i++) {
            // Transformación del mundo para este cubo
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            ourShader.setMat4("model", model);

            // Dibuja el cubo
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //TARGET
        /*float angle = 90.0f;
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it downada
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

        ourShader.setMat4("model", model);
        targetModel.Draw(ourShader);*/

        // SKYBOX
        glm::mat4 skyboxMatrix = glm::mat4(1.0f);
        skyboxMatrix = glm::translate(skyboxMatrix, glm::vec3(50.0f, 0.0f, 50.0f));
        skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        skyboxMatrix = glm::scale(skyboxMatrix, glm::vec3(1000.0f));
        ourShader.setMat4("model", skyboxMatrix);
        skybox.Draw(ourShader);
  
        // Mira
        drawReticle(ourShader, view, projection, reticle2d);
      
        // logo
        drawLogo(ourShader, view, projection, logo);

        // Decidir qué arma dibujar
        // DEAGLE
        if (showDeagle) {
            drawDeagle(ourShader, view, projection, deagle);
        }
        // M4
        else if (showM4) {
            drawM4(ourShader, view, projection, m4);
        }
        // BAYONET
        else if (showBayonet) {
            drawBayonet(ourShader, view, projection, bayonet);
        }
     
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

    // Restricciones de movimiento en el eje Y ya aplicadas previamente
    // Restricciones en los ejes X y Z
    //newPosition.x = glm::clamp(newPosition.x, X_MIN_LIMIT, X_MAX_LIMIT);
    //newPosition.z = glm::clamp(newPosition.z, Z_MIN_LIMIT, Z_MAX_LIMIT);

    // Aplicar la nueva posición
    //camera.Position = newPosition;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
// ----------------------------------------------------------------------
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

void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4) {
    glm::mat4 armaMatrix = glm::mat4(1.0f);
    armaMatrix = glm::translate(armaMatrix, glm::vec3(0.28f, -0.7f, -0.1f));
    armaMatrix = glm::rotate(armaMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    armaMatrix = glm::scale(armaMatrix, glm::vec3(0.04f));
    armaMatrix = glm::inverse(view) * armaMatrix;
    shader.setMat4("model", armaMatrix);
    m4.Draw(shader);
}

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

void drawReticle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& reticle2d) {
    glm::mat4 reticleMatrix = glm::mat4(1.0f);
    reticleMatrix = glm::translate(reticleMatrix, glm::vec3(0.0f, 0.0f, -0.30f)); // Moviendo más cerca
    reticleMatrix = glm::rotate(reticleMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    reticleMatrix = glm::rotate(reticleMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    reticleMatrix = glm::scale(reticleMatrix, glm::vec3(0.003f)); // Haciendo la mira más pequeña
    reticleMatrix = glm::inverse(view) * reticleMatrix;
    shader.setMat4("model", reticleMatrix);
    reticle2d.Draw(shader);
}

void drawLogo(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& logo) {
    glm::mat4 logoMatrix = glm::mat4(1.0f);
    logoMatrix = glm::translate(logoMatrix, glm::vec3(11.0f, 5.1f, 20.0f)); // Moviendo más cerca
    logoMatrix = glm::scale(logoMatrix, glm::vec3(100.0f)); // Haciendo la mira más pequeña
    shader.setMat4("model", logoMatrix);
    logo.Draw(shader);
}