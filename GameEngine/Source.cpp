#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "cLoadModels.h"
#include "FlyCam.h"
#include "cShaderCompiler.h"
#include "cVAOManager.h"
#include "cScene.h"
#include "IPlayer.h"
#include "cPlayer.h"
#include "cAiEnemy.h"
#include "cLightManager.h"
#include "cLightMover.h"
#include "cPhysicsUpdated.h"
#include "cRenderModel.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Refer cShaderCompiler class for more info.
    cShaderCompiler shader;
    GLuint shaderProgram = shader.CompileShader();

    // Import Scene
    cScene scene;
    scene.CreateScene("../sceneFileWithNormals.txt");
    //scene.ExportMaterial(shaderProgram, scene.numberOfMeshesToLoad);            // Considering number of materials = number of meshes to load

    cVAOManager VAOManager;
    for (int i = 0; i != scene.pModels.size(); i++) {
        VAOManager.GettingModelReadyToRender(scene.pModels[i]);         // This thing is new just because I created whole new VAO thing which creates several different VAOs and now I can render a single model multiple times
    }

    cRenderModel renderer;

    cLightManager lightManager;
    lightManager.LoadLights("../lightsFile.txt");
    // Camera Initialization
    FlyCam flyCam(800, 600, glm::vec3(41.00f, -30.99f, 10.87f), 180.0f);
    flyCam.camSpeed = 0.1f;
    flyCam.camSensi = 1.5f;
    flyCam.camRoll = -96.50f;
    flyCam.camYaw = -172.88f;
    flyCam.camPitch = 39.2f;
    cLightMover lightMover(lightManager, flyCam, 5);
    lightMover.lightSpeed = 100.0f;

    float deltaTime = 0;
    float startTime, endTime;

    for (int modelIndex = 0; modelIndex != scene.numberOfMeshesToLoad; modelIndex++) {
        scene.pModels[modelIndex].pTransformedVertices = new cLoadModels::sTransformedVertex[scene.pModels[modelIndex].numberOfVertices];
    
        glm::mat4 model = scene.pModels[modelIndex].CreateModelMatrix(shaderProgram, scene.pModels[modelIndex]);      // Creation of model matrix with arguements passed in sceneFile.txt
        scene.pModels[modelIndex].GenerateTransformedVertices(model);
    }   // Used for initializing the pTransformedVertices, Nothing new xD

    // Starting physics
    //cPhysicsUpdated physicsEngine(scene);
    //physicsEngine.StartPhysics(scene);

    startTime = glfwGetTime();
    //cPlayer bunny(scene.pModels[0]);
    //bunny.SetSpeed(0.06f);
    //
    //cPlayer dragon(scene.pModels[2]);
    //dragon.SetSpeed(0.06f);

    float initialAttenuationLight4 = lightManager.lights[4].atten.z;
    float initialAttenuationLight5 = lightManager.lights[5].atten.z;

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Input handling
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwGetWindowSize(window, &flyCam.camWidth, &flyCam.camHeight);

        std::string cameraPositionInTitle = "Camera Location: (" + to_string(flyCam.camLocation.x) + ", " + to_string(flyCam.camLocation.y) + ", " + to_string(flyCam.camLocation.z) + " Camera Roll: " + to_string(flyCam.camRoll) + " Cam yaw: " + to_string(flyCam.camYaw) + " Cam pitch: " + to_string(flyCam.camPitch);
        glfwSetWindowTitle(window, cameraPositionInTitle.c_str());

        endTime = glfwGetTime();
        deltaTime = endTime - startTime;


        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(21.30f, 2.56f, 1.59f);
            flyCam.camRoll = -87.50f;
            flyCam.camYaw = -182.75f;
            flyCam.camPitch = 57.76f;
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(13.52f, -9.31f, 4.28f);
            flyCam.camRoll = -451.0f;
            flyCam.camYaw = -172.37f;
            flyCam.camPitch = 85.25f;
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(-17.38f, -10.74f, 4.62f);
            flyCam.camRoll = -23.0f;
            flyCam.camYaw = -195.70f;
            flyCam.camPitch = 173.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(41.00f, -30.99f, 10.87f);
            flyCam.camRoll = -96.50f;
            flyCam.camYaw = -172.88f;
            flyCam.camPitch = 39.2f;
        }


        flyCam.cameraMatrix(45.0f, 0.1f, 1000.0f, shaderProgram, "camMatrix", window);

        // Rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);   // Enable depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program and draw the triangle
        glUseProgram(shaderProgram);


        if (deltaTime > 1.5f) {
            // Just wanted to make the scene little dynamic so implemented this, so that lights will flicker
            lightManager.lights[4].atten.z *= 0.1f;
            lightManager.lights[5].atten.z *= 0.1f;
            startTime = endTime;
        }
        else if (deltaTime <= 1.0f) {
            lightManager.lights[4].atten.z = initialAttenuationLight4;
            lightManager.lights[5].atten.z = initialAttenuationLight5;
        }


        // ------------------------------------------------------------------------------------------------------------------------------
        // You can call movement functions from light mover class for lights here now and then call turn on lights function of light manager
        lightManager.TurnOnLights(shaderProgram, 10);
        // ------------------------------------------------------------------------------------------------------------------------------
        // You can create player objects here and make them move from here
        // ------------------------------------------------------------------------------------------------------------------------------

        
        shader.SetSceneView(window);        // Press 1, 2 or 3 for different viewModes like wireframe, fill or point
        


        for (int i = 0; i != scene.pModels.size(); i++) {
            if (scene.pModels[i].bIsWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                shader.SetSceneView(window);        // Press 1, 2 or 3 for different viewModes like wireframe, fill or point
            }
            renderer.Render(shaderProgram, &scene.pModels[i]);
        }

        //renderer.DrawDebugSphere(&scene.pModels[0], glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), 1, shaderProgram);

        // Swap buffers and poll IO events (keys pressed/released, mouse moved, etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Cleanup
    VAOManager.VAOVBOCleanup();
    
    // Terminate GLFW
    glfwTerminate();
    return 0;
}