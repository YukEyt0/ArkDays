#include "sekai.h"

extern std::unique_ptr<ModelLibrary> _model_lib;
extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;
extern std::unique_ptr<EnemyLibrary> _enemy_lib;
extern std::unique_ptr<OperatorLibrary> _operator_lib;
extern std::unique_ptr<OperatorIconLibrary> _icon_lib;

sekai::sekai(const Options& options) : Application(options) {
    _model_lib.reset(new ModelLibrary(_assetRootDir));
    _texture_lib.reset(new TextrueLibrary(_assetRootDir));
    _shape_lib.reset(new ShapeLibrary);
    _player.reset(new ModelOnWorld(_model_lib->getModel(13),0));
    // init time
    _time = 0;
    // init skybox
    std::vector<std::string> skyboxTextureFullPaths;
    for (size_t i = 0; i < skyboxTextureRelPaths.size(); ++i) {
        skyboxTextureFullPaths.push_back(getAssetFullPath(skyboxTextureRelPaths[i]));
    }
    _skybox.reset(new SkyBox(skyboxTextureFullPaths));
    _map.reset(new Map(_assetRootDir));
    _enemy_lib.reset(new EnemyLibrary);
    _icon_lib.reset(new OperatorIconLibrary(_assetRootDir));
    _operator_lib.reset(new OperatorLibrary);
    _hpdrawler.reset(new HPDrawler);
    initOperators();
    // set input mode
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
    // init camera
    _cameras.resize(3);
    const float aspect = 1.0f * _windowWidth / _windowHeight;
    constexpr float znear = 0.1f;
    constexpr float zfar = 10000.0f;
    _cameras[0].reset(new PerspectiveCamera(
        glm::radians(50.0f), aspect, 0.1f, 10000.0f));
    _cameras[0]->transform.position.y = 20.0f;
    _cameras[1].reset(
        new PerspectiveCamera(
        glm::radians(50.0f), aspect, 0.1f, 10000.0f));
    _cameras[1]->transform.position = glm::vec3(3.0f*_mapscale, 10.0f * _mapscale, 5.5f*_mapscale);
    _cameras[1]->transform.rotation = glm::quat(0.5,-0.5,-0.5,-0.5);
    _cameras[2].reset(new PerspectiveCamera(
        glm::radians(50.0f), aspect, 0.1f, 10000.0f));
    initShaders();

    // init lights
    _ambientLight.reset(new AmbientLight);
    _ambientLight->intensity = 0.05f;
    _directionalLight.reset(new DirectionalLight);
    _directionalLight->intensity = 0.5f;
    _directionalLight->transform.rotation =
        glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(-1.0f)));

    _spotLight.reset(new SpotLight);
    _spotLight->intensity = 1.0f;
    _spotLight->angle = glm::radians(30.0f);
    _spotLight->transform.position = glm::vec3(0.0f, 0.0f, 2.5f);
    _spotLight->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    _material.reset(new Material);
    _material->ka = glm::vec3(0.03f, 0.03f, 0.03f);
    _material->kd = glm::vec3(1.0f, 1.0f, 1.0f);
    _material->ks = glm::vec3(1.0f, 1.0f, 1.0f);
    _material->ns = 10.0f;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init();
    _state = RUN;
}
void sekai::initShaders() {
    /*
    const char* vsCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"

        "out vec3 fPosition;\n"
        "out vec3 fNormal;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main() {\n"
        "    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
        "}\n";

    const char* fsCode =
        "#version 330 core\n"
        "in vec3 fPosition;\n"
        "in vec3 fNormal;\n"
        "out vec4 color;\n"

        "// material data structure declaration\n"
        "struct Material {\n"
        "    vec3 ka;\n"
        "    vec3 kd;\n"
        "    vec3 ks;\n"
        "    float ns;\n"
        "};\n"

        "// ambient light data structure declaration\n"
        "struct AmbientLight {\n"
        "    vec3 color;\n"
        "    float intensity;\n"
        "};\n"
        
        "// directional light data structure declaration\n"
        "struct DirectionalLight {\n"
        "    vec3 direction;\n"
        "    float intensity;\n"
        "    vec3 color;\n"
        "};\n"

        "// spot light data structure declaration\n"
        "struct SpotLight {\n"
        "    vec3 position;\n"
        "    vec3 direction;\n"
        "    float intensity;\n"
        "    vec3 color;\n"
        "    float angle;\n"
        "    float kc;\n"
        "    float kl;\n"
        "    float kq;\n"
        "};\n"

        "// uniform variables\n"
        "uniform Material material;\n"
        "uniform DirectionalLight directionalLight;\n"
        "uniform SpotLight spotLight;\n"
        "uniform vec3 viewPos;\n"
        "uniform AmbientLight ambientLight;\n"
        
        "vec3 calcDirectionalLight(vec3 normal,vec3 viewDir) {\n"
        "    vec3 lightDir = normalize(-directionalLight.direction);\n"
        "    vec3 ambient = material.ka * ambientLight.color * ambientLight.intensity;\n"
        "    vec3 diffuse = max(dot(lightDir, normal), 0.0f) * material.kd;\n"
        "    vec3 reflectDir = reflect(-lightDir,normal);\n"
        "    vec3 spec = pow(max(dot(viewDir,reflectDir),0.0f),material.ns)*material.ks;\n"
        "    return directionalLight.intensity * directionalLight.color * (diffuse+spec) + ambient;\n"
        "}\n"

        "vec3 calcSpotLight(vec3 normal,vec3 viewDir) {\n"
        "    vec3 lightDir = normalize(spotLight.position - fPosition);\n"
        "    float theta = acos(-dot(lightDir, normalize(spotLight.direction)));\n"
        "    vec3 diffuse = spotLight.color * max(dot(lightDir, normal), 0.0f) * material.kd;\n"
        
        "    float distance = length(spotLight.position - fPosition);\n"
        "    vec3 reflectDir = reflect(-lightDir,normal);\n"
        "    vec3 spec = spotLight.color*pow(max(dot(viewDir,reflectDir),0.0f),material.ns)*material.ks;\n"
        "    if (theta > spotLight.angle) {\n"
        "        diffuse = vec3(0.0f, 0.0f, 0.0f);\n"
        "        spec = vec3(0.0f,0.0f,0.0f);\n"
        "    }\n"
        "    return spotLight.intensity * (diffuse+spec);\n"
        "}\n"

        "void main() {\n"
        "    vec3 normal = normalize(fNormal);\n"
        "    vec3 viewDir = normalize(viewPos - fPosition);\n"
        "    vec3 phongShader = calcDirectionalLight(normal,viewDir) + calcSpotLight(normal,viewDir);\n"
        "    color = vec4(phongShader, 1.0f);\n"
        "}\n";

    _shader.reset(new GLSLProgram);
    _shader->attachVertexShader(vsCode);
    _shader->attachFragmentShader(fsCode);
    _shader->link();
    */
    const char* vsCode_select = 
    "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"

        "out vec3 worldPosition;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main() {\n"
        "    worldPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    gl_Position = projection * view * vec4(worldPosition, 1.0f);\n"
        "}\n";

    const char* fsCode_select = 
    "#version 330\n"
    "uniform int x;\n"
    "uniform int y;\n"
    "out vec4 outputF;\n"
    "void main() {\n"
    "   outputF = vec4(x/255.0,y/255.0,1.0,0);\n"
    "}\n";
    _select_shader.reset(new GLSLProgram);
    _select_shader->attachVertexShader(vsCode_select);
    _select_shader->attachFragmentShader(fsCode_select);
    _select_shader->link();

    const char* vsCode_textcoord =
            "#version 330 core\n"
            "layout(location = 0) in vec3 aPosition;\n"
            "layout(location = 1) in vec3 aNormal;\n"
            "out vec3 texCoord;\n"
            "out vec3 fPosition;\n"
            "out vec3 fNormal;\n"
            "uniform mat4 projection;\n"
            "uniform mat4 view;\n"
            "uniform mat4 model;\n"
            "void main() {\n"
            "   texCoord = aPosition;\n"
            "   fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
            "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
            "   gl_Position = projection * view * vec4(fPosition, 1.0f);\n"
            "}\n";

        const char* fsCode_textcoord =
        "#version 330 core\n"
        "in vec3 fPosition;\n"
        "in vec3 fNormal;\n"
        "in vec3 texCoord;\n"
            
        "out vec4 color;\n"

        "// material data structure declaration\n"
        "struct Material {\n"
        "    vec3 ka;\n"
        "    vec3 kd;\n"
        "    vec3 ks;\n"
        "    float ns;\n"
        "};\n"

        "// ambient light data structure declaration\n"
        "struct AmbientLight {\n"
        "    vec3 color;\n"
        "    float intensity;\n"
        "};\n"
        
        "// directional light data structure declaration\n"
        "struct DirectionalLight {\n"
        "    vec3 direction;\n"
        "    float intensity;\n"
        "    vec3 color;\n"
        "};\n"

        "// spot light data structure declaration\n"
        "struct SpotLight {\n"
        "    vec3 position;\n"
        "    vec3 direction;\n"
        "    float intensity;\n"
        "    vec3 color;\n"
        "    float angle;\n"
        "    float kc;\n"
        "    float kl;\n"
        "    float kq;\n"
        "};\n"

        "// uniform variables\n"
        "uniform Material material;\n"
        "uniform DirectionalLight directionalLight;\n"
        "uniform SpotLight spotLight;\n"
        "uniform vec3 viewPos;\n"
        "uniform AmbientLight ambientLight;\n"
        "uniform samplerCube cubemap;\n"
        
        "vec3 calcDirectionalLight(vec3 normal,vec3 viewDir) {\n"
        "    vec3 lightDir = normalize(-directionalLight.direction);\n"
        "    vec3 ambient = material.ka * texture(cubemap,texCoord).rgb * ambientLight.color * ambientLight.intensity;\n"
        "    vec3 diffuse = max(dot(lightDir, normal), 0.0f) * material.kd * texture(cubemap,texCoord).rgb;\n"
        "    vec3 reflectDir = reflect(-lightDir,normal);\n"
        "    vec3 spec = pow(max(dot(viewDir,reflectDir),0.0f),material.ns)*material.ks;\n"
        "    return directionalLight.intensity * directionalLight.color * (diffuse+spec) + ambient;\n"
        "}\n"

        "vec3 calcSpotLight(vec3 normal,vec3 viewDir) {\n"
        "    vec3 lightDir = normalize(spotLight.position - fPosition);\n"
        "    float theta = acos(-dot(lightDir, normalize(spotLight.direction)));\n"
        "    vec3 diffuse = spotLight.color * max(dot(lightDir, normal), 0.0f) * texture(cubemap,texCoord).rgb;\n"
        
        "    float distance = length(spotLight.position - fPosition);\n"
        "    vec3 reflectDir = reflect(-lightDir,normal);\n"
        "    vec3 spec = spotLight.color*pow(max(dot(viewDir,reflectDir),0.0f),material.ns)*material.ks;\n"
        "    if (theta > spotLight.angle) {\n"
        "        diffuse = vec3(0.0f, 0.0f, 0.0f);\n"
        "        spec = vec3(0.0f,0.0f,0.0f);\n"
        "    }\n"
        "    return spotLight.intensity * (diffuse+spec);\n"
        "}\n"

        "void main() {\n"
        "    vec3 normal = normalize(fNormal);\n"
        "    vec3 viewDir = normalize(viewPos - fPosition);\n"
        "    vec3 phongShader = calcDirectionalLight(normal,viewDir) + calcSpotLight(normal,viewDir);\n"
        "    color = vec4(phongShader, 1.0f);\n"
        "}\n";

    _textcoord_shader.reset(new GLSLProgram);
    _textcoord_shader->attachVertexShader(vsCode_textcoord);
    _textcoord_shader->attachFragmentShader(fsCode_textcoord);
    _textcoord_shader->link();
}

void sekai::handleInput() {
    if(_input.mouse.press.left==GLFW_PRESS)
        std::cout <<  _cameras[0]->transform.position.x << ", " << _cameras[0]->transform.position.y << ", " << _cameras[0]->transform.position.z << std::endl; 
    deal_rearrange();
    if (_input.keyboard.keyStates[GLFW_KEY_F1] == GLFW_PRESS&&!_operator_movement_controller.scpressed) {
        _operator_movement_controller.scpressed = true;
        newScreenShot(_windowWidth,_windowHeight,"../screenshot/");
    }
    if(_input.keyboard.keyStates[GLFW_KEY_F1] == GLFW_RELEASE&&_operator_movement_controller.scpressed) {
        _operator_movement_controller.scpressed =false;
    }
    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }
    switch(_state) {
        case RUN:
            if(_input.keyboard.keyStates[GLFW_KEY_P] == GLFW_RELEASE&&_operator_movement_controller.ppressed) {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
                _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
                glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
                _operator_movement_controller.ppressed = false;
            }
            if(_input.keyboard.keyStates[GLFW_KEY_P] == GLFW_PRESS&&!_operator_movement_controller.ppressed) {
                _operator_movement_controller.ppressed = true;
                _state=PAUSE;
            }
            if(_operator_movement_controller.f2pressed&&_input.keyboard.keyStates[GLFW_KEY_F2] == GLFW_RELEASE) {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
                _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
                glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
                _operator_movement_controller.f2pressed = false;
            }
            if(!_operator_movement_controller.f2pressed&&_input.keyboard.keyStates[GLFW_KEY_F2] == GLFW_PRESS) {
                _operator_movement_controller.f2pressed = true;
                _state=EDIT;
            }
            if (_input.keyboard.keyStates[GLFW_KEY_F5] == GLFW_PRESS&&!_operator_movement_controller.thirdperson) {
                _operator_movement_controller.thirdperson = true;
            }
            if (_input.keyboard.keyStates[GLFW_KEY_F5] == GLFW_RELEASE&&_operator_movement_controller.thirdperson) {
                _operator_movement_controller.view = (_operator_movement_controller.view+1)%3;
                _operator_movement_controller.thirdperson = false;
            }
            if(_operator_movement_controller.view==0)
                activeCameraIndex = 0;
            else if(_operator_movement_controller.view==1)
                activeCameraIndex = 2;
            else if(_operator_movement_controller.view==2)
                activeCameraIndex = 0;
            first_person_movement_deal();
            add_time(_deltaTime);
            add_enemy();
            deal_enemy_move();
            handle_attack();
            deal_damage();
            break;
        case PAUSE:
            if(_input.keyboard.keyStates[GLFW_KEY_P] == GLFW_RELEASE&&_operator_movement_controller.ppressed) {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
                _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
                glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
                activeCameraIndex = 1;
                _operator_movement_controller.ppressed = false;
            }
            if(_input.keyboard.keyStates[GLFW_KEY_P] == GLFW_PRESS&&!_operator_movement_controller.ppressed) {
                _operator_movement_controller.ppressed = true;
                _state=RUN;
            }
            if(!_operator_movement_controller.f2pressed&&_input.keyboard.keyStates[GLFW_KEY_F2] == GLFW_PRESS) {
                _operator_movement_controller.f2pressed = true;
                _state=EDIT;
            }
            handle_operator_movement_input();
            break;
        case EDIT:
            if(_operator_movement_controller.f2pressed&&_input.keyboard.keyStates[GLFW_KEY_F2] == GLFW_RELEASE) {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
                _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
                glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
                _operator_movement_controller.f2pressed=false;
                activeCameraIndex = 0;
            }
            if(!_operator_movement_controller.f2pressed&&_input.keyboard.keyStates[GLFW_KEY_F2] == GLFW_PRESS) {
                _state = RUN;
                _operator_movement_controller.f2pressed = true;
            }
            if(_input.keyboard.keyStates[GLFW_KEY_LEFT_CONTROL]!=GLFW_RELEASE) {
                if(_input.mouse.press.left==GLFW_PRESS&&!_operator_movement_controller.lpressed) {
                    _operator_movement_controller.lpressed = true;
                    if(!get_selected_object(_input.mouse.move.xNow,_input.mouse.move.yNow,selected_object)) {
                        selected_object = 0;
                    }  
                }
            }
            if(_input.mouse.press.left==GLFW_RELEASE&&_operator_movement_controller.lpressed) {
                _operator_movement_controller.lpressed = false;
            }
            if(_input.mouse.press.right==GLFW_PRESS&&!_operator_movement_controller.rpressed) {
                _operator_movement_controller.rpressed = true;
            }
            if(_input.mouse.press.right==GLFW_RELEASE&&_operator_movement_controller.rpressed) {
                _operator_movement_controller.rpressed = false;
                selected_object = 0;
            }
            break;
    }
    _input.forwardState();
}

void sekai::renderFrame() {
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    auto *camera = _cameras[activeCameraIndex].get();
    if(activeCameraIndex==2) {
        camera->transform = _cameras[0]->transform;
        auto pos = camera->transform.position;
        auto front = glm::cross(camera->transform.getDefaultUp(),camera->transform.getRight());
        auto up = camera->transform.getDefaultUp();
        auto deltdir = glm::normalize(glm::normalize(-front) + glm::normalize(up));  
        camera->transform.position += 2*_mapscale * deltdir;
        camera->transform.lookAt(pos);
    }
    const glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
    const glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();
    _spotLight->transform = _cameras[0]->transform;

    _skybox->draw(projection, view);
    _textcoord_shader->use();
    _textcoord_shader->setUniformMat4("projection", projection);
    _textcoord_shader->setUniformMat4("view", view);
    _textcoord_shader->setUniformVec3("viewPos",_cameras[activeCameraIndex]->transform.position);
    _textcoord_shader->setUniformVec3("spotLight.position", _spotLight->transform.position);
    _textcoord_shader->setUniformVec3("spotLight.direction", _spotLight->transform.getFront());
    _textcoord_shader->setUniformFloat("spotLight.intensity", _spotLight->intensity);
    _textcoord_shader->setUniformVec3("spotLight.color", _spotLight->color);
    _textcoord_shader->setUniformFloat("spotLight.angle", _spotLight->angle);
    _textcoord_shader->setUniformVec3(
        "directionalLight.direction", _directionalLight->transform.getFront());
    _textcoord_shader->setUniformFloat("directionalLight.intensity", _directionalLight->intensity);
    _textcoord_shader->setUniformVec3("directionalLight.color", _directionalLight->color);
    _textcoord_shader->setUniformVec3("ambientLight.color",_ambientLight->color);
    _textcoord_shader->setUniformFloat("ambientLight.intensity",_ambientLight->intensity);
    _map->draw(_textcoord_shader);
    /*
    _shader->use();
    _shader->setUniformMat4("projection", projection);
    _shader->setUniformMat4("view", view);
    _shader->setUniformVec3("viewPos",_cameras[activeCameraIndex]->transform.position);
    _shader->setUniformVec3("material.kd", _material->kd);
    _shader->setUniformVec3("material.ks", _material->ks);
    _shader->setUniformVec3("material.ka", _material->ka);
    _shader->setUniformFloat("material.ns", _material->ns);
    _shader->setUniformVec3("spotLight.position", _spotLight->transform.position);
    _shader->setUniformVec3("spotLight.direction", _spotLight->transform.getFront());
    _shader->setUniformFloat("spotLight.intensity", _spotLight->intensity);
    _shader->setUniformVec3("spotLight.color", _spotLight->color);
    _shader->setUniformFloat("spotLight.angle", _spotLight->angle);
    _shader->setUniformVec3(
        "directionalLight.direction", _directionalLight->transform.getFront());
    _shader->setUniformFloat("directionalLight.intensity", _directionalLight->intensity);
    _shader->setUniformVec3("directionalLight.color", _directionalLight->color);
    _shader->setUniformVec3("ambientLight.color",_ambientLight->color);
    _shader->setUniformFloat("ambientLight.intensity",_ambientLight->intensity);
    */
    if(activeCameraIndex == 2)
        draw_thrd_person(_textcoord_shader);
    draw_enemies(_textcoord_shader);
    draw_placed_operators(_textcoord_shader);
    draw_projectile(_textcoord_shader);
    draw_hp();
    if(_state==PAUSE) {
        draw_icons();
    }
    if(_state==EDIT) {
        renderEditGUI();
    }
}

bool sekai::CollisionDetection(std::unique_ptr<EnemyOnWorld>& ref_enemy,int idx) {
    auto pos = ref_enemy->_enemy->_model->_transform.position;
    auto pos_x = pos.x;
    auto pos_y = pos.z;
    for(auto& it : _placed_operators) {
        auto o_x = it->_operator->_model->_transform.position.x;
        auto o_y = it->_operator->_model->_transform.position.z;
        if(glm::length(glm::vec2(pos_x,pos_y)-glm::vec2(o_x,o_y)) <= _mapscale/2.0f) {
            if(it->_block_buffer.size()<it->_operator->_block) {
                it->_block_buffer.emplace_back(idx);
                return true;
            }
        }
    }
    return false;
}

void sekai::first_person_movement_deal() {
    constexpr float cameraMoveSpeed = 20.0f;
    constexpr float cameraRotateSpeed = 0.2f;
    if (activeCameraIndex == 1)
        return;

    Camera* camera = _cameras[0].get();

    auto front = glm::cross(camera->transform.getDefaultUp(),camera->transform.getRight());

    if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE&&PlayerCollisionDetection()) {
        #ifdef Debug
            std::cout << "W" << std::endl;
        #endif
        camera->transform.position +=  front * _deltaTime * cameraMoveSpeed;
    }
    
    if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE&&PlayerCollisionDetection()) {
        #ifdef Debug
            std::cout << "A" << std::endl;
        #endif
        camera->transform.position -= camera->transform.getRight() * _deltaTime * cameraMoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE&&PlayerCollisionDetection()) {
        #ifdef Debug
            std::cout << "S" << std::endl;
        #endif
        camera->transform.position -= front * _deltaTime * cameraMoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE&&PlayerCollisionDetection()) {
        #ifdef Debug
            std::cout << "D" << std::endl;
        #endif
        camera->transform.position += camera->transform.getRight() * _deltaTime * cameraMoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_SPACE] != GLFW_RELEASE) {
        camera->transform.position += camera->transform.getDefaultUp() * _deltaTime * cameraMoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_LEFT_SHIFT] != GLFW_RELEASE) {
        camera->transform.position -= camera->transform.getDefaultUp() * _deltaTime * cameraMoveSpeed;
    }

    if (_input.mouse.move.xNow != _input.mouse.move.xOld) {
        #ifdef Debug
            std::cout << "mouse move in x direction" << std::endl;
        #endif
        float mouse_movement_in_x_direction = (_input.mouse.move.xNow - _input.mouse.move.xOld) * cameraRotateSpeed;
        float angle = glm::radians(mouse_movement_in_x_direction);
        camera->transform.rotation = glm::angleAxis(angle,-glm::vec3(0.0f, 1.0f, 0.0f)) * camera->transform.rotation;
    }

    if (_input.mouse.move.yNow != _input.mouse.move.yOld) {
        #ifdef Debug
            std::cout << "mouse move in y direction" << std::endl;
        #endif
        float mouse_movement_in_y_direction = (_input.mouse.move.yNow - _input.mouse.move.yOld) * cameraRotateSpeed;  
        float angle = glm::radians(mouse_movement_in_y_direction);
        camera->transform.rotation = glm::angleAxis(angle,-camera->transform.getRight()) * camera->transform.rotation;
    }

    
    }

void sekai::place_operator() {
        auto o = std::make_unique<OperatorOnWorld>();
        o.swap(_operators[operator_cursor]);
        o->_operator->_model->_transform.position = _map->point2WorldPosition(_operator_movement_controller.x,_operator_movement_controller.y);
        if(bitmap[_operator_movement_controller.x][_operator_movement_controller.y]!=0)
            o->_operator->_model->_transform.position+=glm::vec3(0,_mapscale,0);
        o->_state=OperatorOnWorld::PLACED;
        o->_pos_x = _operator_movement_controller.x;
        o->_pos_y = _operator_movement_controller.y;
        o->hp = o->_operator->_hp;
        o->_operator->_model->_transform.scale *= glm::vec3(0.3f);
        _placed_operators.emplace_back(std::move(o));
        _operators.erase(_operators.begin()+operator_cursor);
        if(_operators.size()!=0)
            dec_cursor();
    }

void sekai::handle_operator_movement_input() {
        switch (_operator_state) {
            case SELECTING:
                if(_input.keyboard.keyStates[GLFW_KEY_LEFT]==GLFW_PRESS&&_operators.size()!=0 && !_operator_movement_controller.lpressed) {
                    dec_cursor();
                    _operator_movement_controller.lpressed = true;
                }
                if(_input.keyboard.keyStates[GLFW_KEY_RIGHT]==GLFW_PRESS&&_operators.size()!=0&& !_operator_movement_controller.rpressed) {
                    inc_cursor();
                    _operator_movement_controller.rpressed = true;
                }
                if(_operator_movement_controller.lpressed&&_input.keyboard.keyStates[GLFW_KEY_LEFT]==GLFW_RELEASE)
                    _operator_movement_controller.lpressed = false;
                if(_operator_movement_controller.rpressed&&_input.keyboard.keyStates[GLFW_KEY_RIGHT]==GLFW_RELEASE)
                    _operator_movement_controller.rpressed = false;
                if(_input.mouse.press.left==GLFW_PRESS&&!_operator_movement_controller.rpressed&&!_operator_movement_controller.lpressed) {
                    processSelection(_input.mouse.move.xNow,_input.mouse.move.yNow,_operator_movement_controller.x,_operator_movement_controller.y);
                    _operator_state = PLACING;
                    std::cout << _operator_movement_controller.x << ", " << _operator_movement_controller.y << std::endl;
                    _operator_movement_controller.mlpressed = true;
                }
                break;
            case PLACING:
                if(_input.mouse.press.left==GLFW_RELEASE&&_operator_movement_controller.mlpressed) {
                    int block_type = bitmap[_operator_movement_controller.x][_operator_movement_controller.y];
                    auto operator_type = _operators[operator_cursor]->_operator->_type;
                    std::cout << block_type << ", " << operator_type << std::endl;
                    auto valid = (block_type==0&&(operator_type==OperatorType::DEFENDER||operator_type==OperatorType::VANGUARD))
                    || (block_type==2&&(operator_type==OperatorType::SNIPER||operator_type==OperatorType::CASTER||operator_type==OperatorType::MEDIC));
                    if(!valid) {
                        _operator_state = SELECTING;
                    }
                    _operator_movement_controller.mlpressed = false;
                }
                if(_input.mouse.press.right==GLFW_PRESS&&!_operator_movement_controller.mrpressed) {
                    _operator_movement_controller.mrpressed = true;
                }
                if(_operator_movement_controller.mrpressed&&_input.mouse.press.right==GLFW_RELEASE) {
                    _operator_state = SELECTING;
                    _operator_movement_controller.mrpressed = false;
                }
                if(!_operator_movement_controller.mlpressed&&_input.mouse.press.left==GLFW_PRESS) {
                    _operator_state = DIRECTING;
                    _operator_movement_controller.mlpressed = true;
                }
                break;
            case DIRECTING:
                if(_input.mouse.press.left==GLFW_RELEASE&&_operator_movement_controller.mlpressed) {
                    place_operator();
                    _operator_movement_controller.mlpressed = false;
                    _operator_state = SELECTING;
                }
                break;
            default:
                break;
        }

    }

void sekai::draw_placed_operators(std::unique_ptr<GLSLProgram>& ref_shader) {
        for(auto& it : _placed_operators) {
            auto& model = it->_operator->_model;
            auto& textrue = _texture_lib->getTextureRef(model->_textrue_idx);
            if(model->enable_textrue) {
               textrue->bind(0);
                ref_shader->setUniformInt("cubemap",0);
            }
            ref_shader->setUniformVec3("material.ka", model->_material.ka);
            ref_shader->setUniformVec3("material.ks", model->_material.ks);
            ref_shader->setUniformVec3("material.kd", model->_material.kd);
            ref_shader->setUniformFloat("material.ns", model->_material.ns);
            ref_shader->setUniformMat4("model",model->_transform.getLocalMatrix());
            model->draw();
            if(model->enable_textrue) {
                textrue->unbind();
            }
        }
    }

void sekai::handle_attack() {
    for(int i = 0; i<_placed_operators.size();++i) {
        auto& it = _placed_operators[i];
        if(it->counter<=0) {
            auto type = it->_operator->_type;
            if(type==OperatorType::DEFENDER||type==OperatorType::VANGUARD) {
                for(auto e : it->_block_buffer) {
                    auto& enemy = _enemies[e];
                    if(enemy->counter<=0) {
                        std::unique_ptr<Attack> attack;
                        attack.reset(new Attack(e,i));
                        attack->_style = Attack::HIT;
                        _enemy_attack.emplace_back(std::move(attack));
                        enemy->counter = enemy->_enemy->_cool_down;
                    } else {
                        enemy->counter-=_deltaTime;
                    }
                }
                if(it->_block_buffer.size()!=0) {
                    std::unique_ptr<Attack> attack;
                    attack.reset(new Attack(i,it->_block_buffer[0]));
                    attack->_style = Attack::HIT;
                    _operator_attack.emplace_back(std::move(attack));
                    it->counter=it->_operator->_cool_down;
                    continue;
                }
            }
            if(type==OperatorType::VANGUARD||type==OperatorType::SNIPER||type==OperatorType::CASTER) {
                for(int j = 0; j<_enemies.size();j++) {
                    auto &e = _enemies[j];
                    if(e->_state==EnemyOnWorld::DEAD)
                        continue;
                    auto pose = glm::vec2(e->_enemy->_model->_transform.position.x,e->_enemy->_model->_transform.position.z);
                    auto poso = glm::vec2(it->_operator->_model->_transform.position.x,it->_operator->_model->_transform.position.z);
                    if(glm::length(pose-poso)<=it->_operator->_range*_mapscale) {
                        std::unique_ptr<Attack> attack;
                        attack.reset(new Attack(i,j));
                        switch (type) {
                            case OperatorType::VANGUARD:
                                attack->_proj.reset(new Projectile(e->_enemy->_model,it->_operator->_model->_transform.position,Projectile::CUBE));
                                break;
                            case OperatorType::SNIPER:
                                attack->_proj.reset(new Projectile(e->_enemy->_model,it->_operator->_model->_transform.position,Projectile::CONE));
                                break;
                            case OperatorType::CASTER:
                                attack->_proj.reset(new Projectile(e->_enemy->_model,it->_operator->_model->_transform.position,Projectile::SPHERE));
                                break;
                            default:
                                attack->_proj.reset(new Projectile(e->_enemy->_model,it->_operator->_model->_transform.position,Projectile::CONE));
                                break;
                        }
                        _operator_attack.emplace_back(std::move(attack));
                        it->counter=it->_operator->_cool_down;
                        break;
                    }
                }
            }
            if(type==OperatorType::MEDIC) {
                for(int j = 0; j<_placed_operators.size();++j) {
                    auto &o = _placed_operators[j];
                    if(o->hp<o->_operator->_hp) {
                        std::unique_ptr<Heal> attack;
                        attack.reset(new Heal(i,j));
                        attack->_proj.reset(new Projectile(o->_operator->_model,it->_operator->_model->_transform.position,Projectile::SPHERE));
                        _heal.emplace_back(std::move(attack));
                        it->counter=it->_operator->_cool_down;
                        break;
                    }
                }
            }
        } else {
            it->counter-=_deltaTime;
        }
    }
}

void sekai::deal_damage(){
    for(auto it = _enemy_attack.begin(); it!=_enemy_attack.end();) {
        auto& model = *it;
        if(model->_style==Attack::MOVE)
            ++it;
        else {
            auto atk = _enemies[model->attacker_idx]->_enemy->_atk;
            auto& op = _placed_operators[model->defender_idx];
            auto def = op->_operator->_def;
            auto dmg = (atk-def>0)? atk-def:atk*0.05;
            op->hp-=dmg;
            if(op->hp<0) {
                op->_state=OperatorOnWorld::DEAD;
            }
            it = _enemy_attack.erase(it);
        }
    }

    for(auto it = _operator_attack.begin(); it!=_operator_attack.end();) {
        auto& model = *it;
        auto atk = _placed_operators[model->attacker_idx]->_operator->_atk;
        auto &en = _enemies[model->defender_idx];
        auto def = en->_enemy->_def;
        auto dmg = (atk-def>0)? atk-def:0;
        if (en->_state==EnemyOnWorld::DEAD)
            model->_style = Attack::HIT;
        if(model->_style==Attack::MOVE)
            ++it;
        else {
            auto atk = _placed_operators[model->attacker_idx]->_operator->_atk;
            auto &en = _enemies[model->defender_idx];
            auto def = en->_enemy->_def;
            auto dmg = (atk-def>0)? atk-def:0;
            en->hp-=dmg;
            if(en->hp<0) {
                en->_state = EnemyOnWorld::DEAD;
            }
            it = _operator_attack.erase(it);
        }
    }
    for(auto it = _heal.begin(); it!=_heal.end();) {
        auto& model = *it;
        if(model->_style==Heal::MOVE)
            ++it;
        else {
            auto atk = _placed_operators[model->attacker_idx]->_operator->_atk;
            auto& op = _placed_operators[model->defender_idx];
            op->hp = (op->hp+atk>op->_operator->_hp) ? op->_operator->_hp : op->hp+atk;
            it = _heal.erase(it);
        }
    }
}

void sekai::draw_projectile(std::unique_ptr<GLSLProgram>& ref_shader) {
    for(auto &it : _operator_attack) {
        if(it->_style==Attack::HIT)
            continue;
        it->_proj->_model->_transform.scale = glm::vec3(0.1f,0.1f,1.0f) * _mapscale*0.5f;
        it->_proj->_model->_transform.lookBack(it->_proj->_target->_transform.position);
        it->_proj->_model->_transform.position -= glm::normalize(it->_proj->_model->_transform.getFront()) * 60.0f * _deltaTime;
        ref_shader->setUniformMat4("model",it->_proj->_model->_transform.getLocalMatrix());
        it->_proj->draw();
        if(it->_proj->hit())
            it->_style = Attack::HIT;
    }

    for(auto &it: _heal) {
        if(it->_style==Heal::HIT)
            continue;
        it->_proj->_model->_transform.scale = glm::vec3(0.1f,0.1f,1.0f)* _mapscale*0.5f;
        it->_proj->_model->_transform.lookBack(it->_proj->_target->_transform.position);
        it->_proj->_model->_transform.position -= glm::normalize(it->_proj->_model->_transform.getFront()) * 30.0f * _deltaTime;
        _shader->setUniformMat4("model",it->_proj->_model->_transform.getLocalMatrix());
        it->_proj->draw();
        if(it->_proj->hit())
            it->_style = Heal::HIT;
    }
}

void sekai::deal_rearrange() {
    for(auto it = _placed_operators.begin(); it!=_placed_operators.end();) {
        auto& op = *it;
        for(auto jt = op->_block_buffer.begin();jt!=op->_block_buffer.end();) {
            auto& en = _enemies.at(*jt);
            if(en->_state==EnemyOnWorld::DEAD) {
                int idx = *jt;
                jt = op->_block_buffer.erase(jt);
            } else 
                ++jt;
        }
        if(op->_state==OperatorOnWorld::DEAD) {
            for(auto jt : op->_block_buffer) {
                _enemies.at(jt)->_state = EnemyOnWorld::MOVE;
            }
            it = _placed_operators.erase(it);
        } else 
            ++it;
    }

}

void sekai::draw_thrd_person(std::unique_ptr<GLSLProgram>& ref_shader) {
    update_player_location();
    auto& textrue = _texture_lib->getTextureRef(_player->_textrue_idx);
    if(_player->enable_textrue) {
        textrue->bind(0);
        ref_shader->setUniformInt("cubemap",0);
    }
    ref_shader->setUniformVec3("material.ka", _player->_material.ka);
    ref_shader->setUniformVec3("material.ks", _player->_material.ks);
    ref_shader->setUniformVec3("material.kd", _player->_material.kd);
    ref_shader->setUniformFloat("material.ns", _player->_material.ns);
    ref_shader->setUniformMat4("model",_player->_transform.getLocalMatrix());
    if(_player->enable_textrue) {
        textrue->unbind();
    }
    _player->draw();
}

bool sekai::PlayerCollisionDetection() {
    update_player_location();
    for(auto it : ModelOnWorld::_model_map) {
        if(it.second->id==_player->id)
            continue;
        if(_player->CollisionDetection(it.second)) {
            std::cout << "Collision with id: " << it.first << " Location: " 
            << it.second->_transform.position.x << ", "
            << it.second->_transform.position.y << ", "
            << it.second->_transform.position.z << std::endl;
            return false;
        }   
    }
    return true;
}

void sekai::draw_hp() {
    auto& shader = _hpdrawler->_shader;
    const glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
    const glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();
    auto pos = _cameras[activeCameraIndex]->transform.position;
    shader->use();
    shader->setUniformMat4("projection",projection);
    shader->setUniformMat4("view",view);
    for(auto& it:_enemies) {
        if(it->_state==EnemyOnWorld::DEAD)
            continue;
        auto model = it->_enemy->_model->_transform;
        model.position+=glm::vec3(0.0f,0.3*_mapscale,0.0f);
        model.scale *= glm::vec3(10.0f,0.5f,1.0f);
        model.lookAt(pos);
        _hpdrawler->draw(model.getLocalMatrix(), it->hp,it->_enemy->_hp);
    }
}

void sekai::update_player_location() {
    _player->_transform = _cameras[0]->transform;
    _player->_transform.lookAt(_player->_transform .position + glm::cross(_player->_transform .getDefaultUp(),_player->_transform.getRight()));
    _player->_transform.scale *= glm::vec3(_mapscale / 10.0f / (_player->_object->getBoundingBox().max.y - _player->_object->getBoundingBox().min.y));
}

void sekai::draw_object_select() {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        _select_shader->use();
        const glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
        const glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();
        _select_shader->setUniformMat4("projection", projection);
        _select_shader->setUniformMat4("view", view);
        for(auto it:ModelOnWorld::_model_map) {
            int x = it.first%255;
            int y = (it.first/255)%255;
            _select_shader->setUniformInt("x",x);
            _select_shader->setUniformInt("y",y);
            auto model = it.second->_transform.getLocalMatrix();
            _select_shader->setUniformMat4("model",model);
            it.second->draw_select();
        }
        _select_shader->unuse();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

bool sekai::get_selected_object(int xx, int yy, int &id) {
        draw_object_select();
        GLint viewport[4];
        unsigned char res[4];
        glGetIntegerv(GL_VIEWPORT,viewport);
        glReadPixels(xx,viewport[3]-yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,&res);
        id = res[0] + 255*res[1];
        return !(res[3]==255);
}

void sekai::renderEditGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    if(selected_object!=0) {
        edit_object();
    } else {
        edit_light();
    }
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void sekai::edit_light() {
    
    ImGui::NewFrame();

    const auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin("Editing Lights", nullptr, flags)) {
        ImGui::End();
    } else {
        ImGui::Text("Render Mode");
        ImGui::Separator();

        ImGui::ColorEdit3("ka##3", (float*)&_material->ka);
        ImGui::ColorEdit3("kd##3", (float*)&_material->kd);
        ImGui::ColorEdit3("ks##3", (float*)&_material->ks);
        ImGui::SliderFloat("ns##3", &_material->ns, 1.0f, 50.0f);
        ImGui::NewLine();

        ImGui::Text("ambient light");
        ImGui::Separator();
        ImGui::SliderFloat("intensity##1", &_ambientLight->intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("color##1", (float*)&_ambientLight->color);
        ImGui::NewLine();

        ImGui::Text("directional light");
        ImGui::Separator();
        ImGui::SliderFloat("intensity##2", &_directionalLight->intensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Location##2", &_directionalLight->transform.rotation.x,-1.0f,1.0f);
        ImGui::SliderFloat("Location##3", &_directionalLight->transform.rotation.y,-1.0f,1.0f);
        ImGui::SliderFloat("Location##4", &_directionalLight->transform.rotation.z,-1.0f,1.0f);
        ImGui::ColorEdit3("color##2", (float*)&_directionalLight->color);
        ImGui::NewLine();

        ImGui::Text("spot light");
        ImGui::Separator();
        ImGui::SliderFloat("intensity##3", &_spotLight->intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("color##3", (float*)&_spotLight->color);
        ImGui::SliderFloat(
            "angle##3", (float*)&_spotLight->angle, 0.0f, glm::radians(180.0f), "%f rad");
        ImGui::NewLine();

        ImGui::End();
    }
    ImGui::Render();
}

void sekai::edit_object(){
    assert(selected_object!=0);
    auto& model = ModelOnWorld::_model_map.at(selected_object);
    
    ImGui::NewFrame();

    const auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin("Editing Object", nullptr, flags)) {
        ImGui::End();
    } else {
        ImGui::Text("Edit Material");
        ImGui::Separator();
        ImGui::ColorEdit3("ka##3", (float*)&(model->_material.ka));
        ImGui::ColorEdit3("kd##3", (float*)&(model->_material.kd));
        ImGui::ColorEdit3("ks##3", (float*)&(model->_material.ks));
        ImGui::SliderFloat("ns##3", &((model->_material.ns)), 1.0f, 50.0f);
        ImGui::NewLine();

        ImGui::Text("Edit Textrue");
        ImGui::Separator();
        ImGui::Checkbox("Enable Textrue", &model->enable_textrue);
        std::vector<std::string> items = {
            "Grass",
            "Stone",
            "Wood",
            "Rock",
            "Redzone",
            "Bluezone",
            "nuclear"

        };
        static int item_current = 0;
        if(ImGui::BeginCombo("Set Texture", items[item_current].c_str())) {
            for(int n = 0; n <items.size();++n) {
                bool is_selected = (item_current == n);
                if (ImGui::Selectable(items[n].c_str(),is_selected)) {
                    item_current = n;
                    model->_textrue_idx = n;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::NewLine();
        ImGui::End();
    }

    ImGui::Render();
}