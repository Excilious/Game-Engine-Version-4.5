#include "Engine.h"

Engine::Engine()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,MSAA_SAMPLES);

    //glfwGetPrimaryMonitor()
    GLFWwindow* EngineWindow = glfwCreateWindow(this->Width,this->Height,"Fusion",NULL,NULL);
    glfwMakeContextCurrent(EngineWindow);
    gladLoadGL();
    glViewport(0,0,this->Width,this->Height);

    UpdateVariables("");
    glCullFace(GL_FRONT);
    glfwSwapInterval(0);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    if (!glfwInit())                                            {MessageBox(NULL,TEXT("Failed to load GLFW window."),TEXT("Fusion Engine"),MB_ICONERROR|MB_OK); glfwTerminate();}
    if (EngineWindow == NULL)                                   {glfwTerminate();} 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))    {glfwTerminate();}
    this->EngineWindow = EngineWindow;
}

void OnWindowResizedCallback(GLFWwindow* EngineWindow,int Width,int Height) {glViewport(0,0,Width,Height);}
void Engine::Release()                                                      {if (std::get<bool>(GetFromRegister("exit"))) {this->Destroy(); glfwTerminate(); exit(0);}}

void Engine::CalculateDeltaTime()
{
    this->CurrentTime   = glfwGetTime();
    this->DeltaTime     = this->CurrentTime - this->PreviousTime;
    this->Counter++;
    if (this->DeltaTime >= 1.0f/30.0f) {this->FramesPerSecond = (1.0f/this->DeltaTime) * this->Counter; this->Counter = 0; this->PreviousTime = this->CurrentTime;}
}

void Engine::LoadWorld(std::string WorldDirectory)
{
    SetFromRegister("world",WorldDirectory);
    this->LastWorldInstance = WorldDirectory;
    std::ifstream BaseFile(std::get<std::string>(GetFromRegister("world")));
    if (!BaseFile.good())
    {
        SysPrint("Cannot find "+WorldDirectory+"! Exiting Fusion...",3);
        MessageBox(NULL,TEXT("Cannot find wrdx file! Ensure that the file is within the correct directory."),TEXT("Fusion Engine"),MB_ICONERROR|MB_OK);
        this->Destroy();
    }
    else {BaseFile.close();}
    this->EngineParser.FetchFromDatapack(std::get<std::string>(GetFromRegister("world")));
}

void Engine::Initilise(GLFWwindow* EngineWindow)
{
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (std::get<std::string>(GetFromRegister("world")) != "-1")
    {
        this->EngineCamera.InitiliseCamera(this->Width,this->Height,glm::vec3(0.0f,1.0f,0.0f));
        this->EngineSkybox.InitiliseSkybox(this->Width,this->Height,this->NearView,this->FarView,&this->EngineParser,this->FieldOfView);
        this->SkyboxShader.CreateNewShader(
            this->EngineParser.GetGlobalDatapackData("Skybox.vert").c_str(),
            this->EngineParser.GetGlobalDatapackData("Skybox.frag").c_str(),
            true
        );
        this->BSPShader.CreateNewShader(
            this->EngineParser.GetGlobalDatapackData("BSP.vert").c_str(),
            this->EngineParser.GetGlobalDatapackData("BSP.frag").c_str(),
            true
        );
      
        this->BinarySpaceScene.LoadBinarySpacePartition(std::get<std::string>(GetFromRegister("bsp")));
        this->BinarySpaceScene.SetupBinarySpacePartition(&this->EngineParser);
        this->FPSCounter.CreateLabel("-1",glm::vec2(0.0f,0.0f),1.25f,glm::vec3(1.0f));
        this->EnginePlayer.LoadPlayer(&this->EngineParser,&this->EngineCamera);
        this->EngineWorld.GenerateWorld(this->EngineWindow,&this->EngineParser,this->NearView,this->FarView);

        this->MenuFrame.CreateFrame(glm::vec2(0.0f,0.0f),glm::vec2(Width*4,Height*4),Width,Height,glm::vec3(0.0f,0.0f,0.0f),0.0f,this->EngineParser.GetGlobalDatapackData("Frame.vert"),this->EngineParser.GetGlobalDatapackData("Frame.frag"));
        this->MenuFrame.AddTransparancy(0.75f);

        glfwSetWindowSizeCallback(this->EngineWindow,OnWindowResizedCallback);
        this->SkyboxShader.ActivateShader();
        this->InitilisationComplete         = true;
        this->EnginePlayer.PlayerPosition   = this->BinarySpaceScene.SpawnPlayerInBSP();
        this->EngineCamera.Position         = this->EnginePlayer.PlayerPosition;
    }
}

void Engine::ShowMenu()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    if (MenuToggled)                                {this->MenuFrame.RenderInstance();}
    if (std::get<bool>(GetFromRegister("showfps"))) {this->FPSCounter.SetNewText(std::to_string(this->FramesPerSecond)); this->FPSCounter.RenderText();}
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void Engine::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    float ConsoleCurrentInput = glfwGetTime();
    if (glfwGetKey(this->EngineWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS && (ConsoleCurrentInput-this->ConsoleInputLast) > 0.25f) 
    {ConsoleOpen = !ConsoleOpen; MenuToggled = !MenuToggled; this->ConsoleInputLast = ConsoleCurrentInput;}

    if (std::get<std::string>(GetFromRegister("world")) != "-1" && this->InitilisationComplete) 
    {
        this->EngineCamera.UpdateMatrix(this->FieldOfView,this->NearView,this->FarView);
        this->EngineCamera.UpdateCameraPosition(this->XPosition,this->YPosition);
        this->EngineWorld.RenderWorld(this->EngineWindow,&this->EngineCamera,this->NearView,this->FarView,this->FieldOfView,this->EngineSkybox.CubemapTexture,this->DeltaTime,this->EnginePlayer.PlayerSpeed);
        this->EngineSkybox.RenderSkybox(this->EngineCamera.Position,this->EngineCamera.Orientation,this->EngineCamera.UpPosition,&this->SkyboxShader);
        this->EnginePlayer.Movement(this->EngineWindow,this->EngineCamera.Orientation,this->EngineCamera.Position);
        this->BinarySpaceScene.RenderAllFaces(this->BSPShader.Program,this->EngineCamera.ViewMatrix,this->EngineCamera.ProjectionMatrix);

        glm::vec3 NewPosition = this->BinarySpaceScene.TraceSphere(this->EngineCamera.Position,this->EnginePlayer.PlayerPosition,20.0f);
        this->EngineCamera.Position         = NewPosition;
        this->EnginePlayer.PlayerPosition   = NewPosition;

    }

    if (this->LastWorldInstance != std::get<std::string>(GetFromRegister("world")))
    {
        this->LastWorldInstance = std::get<std::string>(GetFromRegister("world"));
        this->LoadWorld(std::get<std::string>(GetFromRegister("world")));
    }
    this->ShowMenu();

    this->EnginePlayer.PlayerFriction       = std::get<float>(GetFromRegister("frict"));
    this->EnginePlayer.PlayerSpeed          = std::get<float>(GetFromRegister("speed"));
    this->EnginePlayer.PlayerAcceleration   = std::get<float>(GetFromRegister("accel"));
    this->EnginePlayer.PlayerDeceleration   = std::get<float>(GetFromRegister("decel"));
    this->FieldOfView                       = std::get<float>(GetFromRegister("fov"));
    this->NearView                          = std::get<float>(GetFromRegister("near"));
    this->FarView                           = std::get<float>(GetFromRegister("far"));

    CommandUpdateInput(this->EngineWindow);
    CommandRenderText(this->EngineWindow);
    glfwGetCursorPos(this->EngineWindow,&XPosition,&YPosition);
    glfwSwapBuffers(this->EngineWindow);

    this->Release();
    this->CalculateDeltaTime();
    glfwPollEvents();
}

void Engine::Render()
{
    this->LoadWorld("../../Fusion/Base.zip");
    CommandGenerateBackFrame(this->Width,this->Height,
        this->EngineParser.GetGlobalDatapackData("Frame.vert"),
        this->EngineParser.GetGlobalDatapackData("Frame.frag")
    );

    SysPrint("This application is in beta!",2);
    SysPrint("Fusion Engine, GL Version "+std::to_string(VERSION_MAJOR)+std::to_string(VERSION_MINOR),1);
    SysPrint("Resolution: "+std::to_string(this->Width)+"x"+std::to_string(this->Height),1);
}

void Engine::Destroy()
{
    CommandDelete();
    this->BinarySpaceScene.DestroyScene();
    this->SkyboxShader.DestroyShader();
    this->EngineWorld.DestroyWorld();
    this->EnginePlayer.Destroy();
    this->MenuFrame.DestroyInstance();

    glfwDestroyWindow(this->EngineWindow);
    glfwTerminate();
    exit(0);
}