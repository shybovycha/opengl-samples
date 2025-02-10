#include "ImGuiSfmlBackend.hpp"

std::function<void(ImGuiIO&)> beforeImGuiInitHandlerFn = [](ImGuiIO& io) {};
std::function<void(ImGuiIO&)> afterImGuiInitHandlerFn = [](ImGuiIO& io) {};

void initImGuiCursorMapping(ImGuiIO& io)
{
    auto backendData = reinterpret_cast<ImGui_SFML_BackendData*>(io.BackendRendererUserData);

    auto createCursor = [](sf::Cursor::Type cursorType) {
        auto cursor = std::make_shared<sf::Cursor>();
        cursor->loadFromSystem(cursorType);
        return std::move(cursor);
    };

    backendData->mouseCursors[ImGuiMouseCursor_Arrow] = createCursor(sf::Cursor::Arrow);
    backendData->mouseCursors[ImGuiMouseCursor_TextInput] = createCursor(sf::Cursor::Text);
    backendData->mouseCursors[ImGuiMouseCursor_ResizeAll] = createCursor(sf::Cursor::SizeAll);
    backendData->mouseCursors[ImGuiMouseCursor_ResizeNS] = createCursor(sf::Cursor::SizeVertical);
    backendData->mouseCursors[ImGuiMouseCursor_ResizeEW] = createCursor(sf::Cursor::SizeHorizontal);
    backendData->mouseCursors[ImGuiMouseCursor_ResizeNESW] = createCursor(sf::Cursor::SizeBottomLeftTopRight);
    backendData->mouseCursors[ImGuiMouseCursor_ResizeNWSE] = createCursor(sf::Cursor::SizeTopLeftBottomRight);
    backendData->mouseCursors[ImGuiMouseCursor_Hand] = createCursor(sf::Cursor::Hand);
}

void initImGuiFonts(ImGuiIO& io)
{
    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("media/Roboto-Thin.ttf", 16.0f, nullptr);
    io.Fonts->AddFontFromFileTTF("media/Roboto-Thin.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    io.Fonts->AddFontFromFileTTF("media/Roboto-Thin.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromFileTTF("media/Roboto-Thin.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
}

void initImGuiFontAtlas(ImGuiIO& io)
{
    unsigned char* pixelData;
    int width;
    int height;

    io.Fonts->GetTexDataAsRGBA32(&pixelData, &width, &height);

    auto fontsTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    fontsTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    fontsTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    fontsTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(width, height),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(pixelData));

    fontsTexture->textureHandle().makeResident();

    ImTextureID fontAtlasTextureId = static_cast<ImTextureID>(fontsTexture->textureHandle().handle());

    io.Fonts->SetTexID(fontAtlasTextureId);
}

void initImGuiConfigurationFlags(ImGuiIO& io)
{
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void initImGuiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

void initImGuiBackendData(ImGuiIO& io)
{
    auto backendData = new ImGui_SFML_BackendData();

    io.BackendRendererUserData = reinterpret_cast<void*>(backendData);
    io.BackendRendererName = "ImGui_SFML";
}

void initImGuiClipboard(ImGuiIO& io)
{
    io.SetClipboardTextFn = [](void* userData, const char* text) {
        sf::Clipboard::setString(sf::String::fromUtf8(text, text + std::strlen(text)));
    };

    io.GetClipboardTextFn = [](void* userData) -> const char* {
        auto utf8Text = sf::Clipboard::getString().toUtf8();
        auto plainText = std::string(utf8Text.begin(), utf8Text.end());
        return plainText.c_str();
    };
}

void initImGuiStyles(ImGuiIO& io)
{
    ImGui::StyleColorsLight();

#ifdef HIGH_DPI
    io.FontGlobalScale = 2.0f;

    auto style = &ImGui::GetStyle();
    style->ScaleAllSizes(2.0f);
#endif
}

void beforeImGuiInit(std::function<void(ImGuiIO&)> fn)
{
    beforeImGuiInitHandlerFn = fn;
}

void afterImGuiInit(std::function<void(ImGuiIO&)> fn)
{
    afterImGuiInitHandlerFn = fn;
}

void initImGuiShaders(ImGuiIO& io)
{
    const auto vertexShaderSourceStr = R"EOS(
        #version 460

        layout (location = 0) in vec2 Position;
        layout (location = 1) in vec2 UV;
        layout (location = 2) in vec4 Color;

        uniform mat4 projection;

        out vec2 Frag_UV;
        out vec4 Frag_Color;

        void main()
        {
            Frag_UV = UV;
            Frag_Color = Color;
            gl_Position = projection * vec4(Position.xy, 0, 1);
        }
    )EOS";

    const auto fragmentShaderSourceStr = R"EOS(
        #version 460

        #extension GL_ARB_bindless_texture : require

        in vec2 Frag_UV;
        in vec4 Frag_Color;

        layout(bindless_sampler) uniform sampler2D surfaceTexture;

        layout (location = 0) out vec4 Out_Color;

        void main()
        {
            Out_Color = Frag_Color * texture(surfaceTexture, Frag_UV);
        }
    )EOS";

    auto vertexShaderSource = globjects::Shader::sourceFromString(vertexShaderSourceStr);
    auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexShaderSource.get());
    auto vertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

    if (!vertexShader->compile())
    {
        std::cerr << "Error compiling ImGUI vertex shader" << std::endl;
        return;
    }

    auto fragmentShaderSource = globjects::Shader::sourceFromString(fragmentShaderSourceStr);
    auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentShaderSource.get());
    auto fragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

    if (!fragmentShader->compile())
    {
        std::cerr << "Error compiling ImGUI fragment shader" << std::endl;
        return;
    }

    auto shaderProgram = std::make_unique<globjects::Program>();
    shaderProgram->attach(vertexShader.get(), fragmentShader.get());
    shaderProgram->link();

    auto backendData = reinterpret_cast<ImGui_SFML_BackendData*>(io.BackendRendererUserData);

    backendData->textureUniform = shaderProgram->getUniform<gl::GLuint64>("surfaceTexture");
    backendData->textureUniform->set(static_cast<gl::GLuint64>(io.Fonts->TexID));

    backendData->projectionMatrix = shaderProgram->getUniform<glm::mat4>("projection");

    backendData->vertexShader = std::move(vertexShader);
    backendData->fragmentShader = std::move(fragmentShader);
    backendData->shaderProgram = std::move(shaderProgram);
}

void initImGuiBuffers(ImGuiIO& io)
{
    auto vertexBuffer = std::make_unique<globjects::Buffer>();
    auto indexBuffer = std::make_unique<globjects::Buffer>();

    auto vao = std::make_unique<globjects::VertexArray>();

    vao->bindElementBuffer(indexBuffer.get());

    auto stride = sizeof(ImDrawVert);
    auto positionOffset = offsetof(ImDrawVert, pos);
    auto uvOffset = offsetof(ImDrawVert, uv);
    auto colorOffset = offsetof(ImDrawVert, col);

    vao->binding(0)->setAttribute(0);
    vao->binding(0)->setBuffer(vertexBuffer.get(), positionOffset, stride);
    vao->binding(0)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT), false);
    vao->enable(0);

    vao->binding(1)->setAttribute(1);
    vao->binding(1)->setBuffer(vertexBuffer.get(), uvOffset, stride);
    vao->binding(1)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT), false);
    vao->enable(1);

    vao->binding(2)->setAttribute(2);
    vao->binding(2)->setBuffer(vertexBuffer.get(), colorOffset, stride);
    vao->binding(2)->setFormat(4, static_cast<gl::GLenum>(GL_UNSIGNED_BYTE), true);
    vao->enable(2);

    auto backendData = reinterpret_cast<ImGui_SFML_BackendData*>(io.BackendRendererUserData);

    backendData->vao = std::move(vao);
    backendData->indexBuffer = std::move(indexBuffer);
    backendData->vertexBuffer = std::move(vertexBuffer);
}

void initImGuiDisplay(ImGuiIO& io, std::shared_ptr<sf::Window> windowPtr)
{
    io.DisplaySize = ImVec2 { static_cast<float>(windowPtr->getSize().x), static_cast<float>(windowPtr->getSize().y) };
}

bool initImGui(std::weak_ptr<sf::Window> windowPtr)
{
    auto window = windowPtr.lock();

    if (!window)
    {
        return false;
    }

    initImGuiContext();

    auto& io = ImGui::GetIO();

    beforeImGuiInitHandlerFn(io);

    initImGuiBackendData(io);
    initImGuiConfigurationFlags(io);
    initImGuiDisplay(io, std::move(window));
    initImGuiCursorMapping(io);
    initImGuiFonts(io);
    initImGuiFontAtlas(io);
    initImGuiClipboard(io);
    initImGuiStyles(io);
    initImGuiShaders(io);
    initImGuiBuffers(io);

    afterImGuiInitHandlerFn(io);

    return true;
}

ImGuiKey keycodeToImGuiMod(sf::Keyboard::Key code)
{
    switch (code)
    {
        case sf::Keyboard::Key::LControl:
        case sf::Keyboard::Key::RControl:
            return ImGuiMod_Ctrl;
        case sf::Keyboard::Key::LShift:
        case sf::Keyboard::Key::RShift:
            return ImGuiMod_Shift;
        case sf::Keyboard::Key::LAlt:
        case sf::Keyboard::Key::RAlt:
            return ImGuiMod_Alt;
        case sf::Keyboard::Key::LSystem:
        case sf::Keyboard::Key::RSystem:
            return ImGuiMod_Super;
        default:
            break;
    }

    return ImGuiKey_None;
}

ImGuiKey keycodeToImGuiKey(sf::Keyboard::Key code)
{
    switch (code)
    {
        case sf::Keyboard::Key::Tab:
            return ImGuiKey_Tab;
        case sf::Keyboard::Key::Left:
            return ImGuiKey_LeftArrow;
        case sf::Keyboard::Key::Right:
            return ImGuiKey_RightArrow;
        case sf::Keyboard::Key::Up:
            return ImGuiKey_UpArrow;
        case sf::Keyboard::Key::Down:
            return ImGuiKey_DownArrow;
        case sf::Keyboard::Key::PageUp:
            return ImGuiKey_PageUp;
        case sf::Keyboard::Key::PageDown:
            return ImGuiKey_PageDown;
        case sf::Keyboard::Key::Home:
            return ImGuiKey_Home;
        case sf::Keyboard::Key::End:
            return ImGuiKey_End;
        case sf::Keyboard::Key::Insert:
            return ImGuiKey_Insert;
        case sf::Keyboard::Key::Delete:
            return ImGuiKey_Delete;
        case sf::Keyboard::Key::Backspace:
            return ImGuiKey_Backspace;
        case sf::Keyboard::Key::Space:
            return ImGuiKey_Space;
        case sf::Keyboard::Key::Enter:
            return ImGuiKey_Enter;
        case sf::Keyboard::Key::Escape:
            return ImGuiKey_Escape;
        case sf::Keyboard::Key::Apostrophe:
            return ImGuiKey_Apostrophe;
        case sf::Keyboard::Key::Comma:
            return ImGuiKey_Comma;
        case sf::Keyboard::Key::Hyphen:
            return ImGuiKey_Minus;
        case sf::Keyboard::Key::Period:
            return ImGuiKey_Period;
        case sf::Keyboard::Key::Slash:
            return ImGuiKey_Slash;
        case sf::Keyboard::Key::Semicolon:
            return ImGuiKey_Semicolon;
        case sf::Keyboard::Key::Equal:
            return ImGuiKey_Equal;
        case sf::Keyboard::Key::LBracket:
            return ImGuiKey_LeftBracket;
        case sf::Keyboard::Key::Backslash:
            return ImGuiKey_Backslash;
        case sf::Keyboard::Key::RBracket:
            return ImGuiKey_RightBracket;
        case sf::Keyboard::Key::Grave:
            return ImGuiKey_GraveAccent;
        // case : return ImGuiKey_CapsLock;
        // case : return ImGuiKey_ScrollLock;
        // case : return ImGuiKey_NumLock;
        // case : return ImGuiKey_PrintScreen;
        case sf::Keyboard::Key::Pause:
            return ImGuiKey_Pause;
        case sf::Keyboard::Key::Numpad0:
            return ImGuiKey_Keypad0;
        case sf::Keyboard::Key::Numpad1:
            return ImGuiKey_Keypad1;
        case sf::Keyboard::Key::Numpad2:
            return ImGuiKey_Keypad2;
        case sf::Keyboard::Key::Numpad3:
            return ImGuiKey_Keypad3;
        case sf::Keyboard::Key::Numpad4:
            return ImGuiKey_Keypad4;
        case sf::Keyboard::Key::Numpad5:
            return ImGuiKey_Keypad5;
        case sf::Keyboard::Key::Numpad6:
            return ImGuiKey_Keypad6;
        case sf::Keyboard::Key::Numpad7:
            return ImGuiKey_Keypad7;
        case sf::Keyboard::Key::Numpad8:
            return ImGuiKey_Keypad8;
        case sf::Keyboard::Key::Numpad9:
            return ImGuiKey_Keypad9;
        // case : return ImGuiKey_KeypadDecimal;
        case sf::Keyboard::Key::Divide:
            return ImGuiKey_KeypadDivide;
        case sf::Keyboard::Key::Multiply:
            return ImGuiKey_KeypadMultiply;
        case sf::Keyboard::Key::Subtract:
            return ImGuiKey_KeypadSubtract;
        case sf::Keyboard::Key::Add:
            return ImGuiKey_KeypadAdd;
        // case : return ImGuiKey_KeypadEnter;
        // case : return ImGuiKey_KeypadEqual;
        case sf::Keyboard::Key::LControl:
            return ImGuiKey_LeftCtrl;
        case sf::Keyboard::Key::LShift:
            return ImGuiKey_LeftShift;
        case sf::Keyboard::Key::LAlt:
            return ImGuiKey_LeftAlt;
        case sf::Keyboard::Key::LSystem:
            return ImGuiKey_LeftSuper;
        case sf::Keyboard::Key::RControl:
            return ImGuiKey_RightCtrl;
        case sf::Keyboard::Key::RShift:
            return ImGuiKey_RightShift;
        case sf::Keyboard::Key::RAlt:
            return ImGuiKey_RightAlt;
        case sf::Keyboard::Key::RSystem:
            return ImGuiKey_RightSuper;
        case sf::Keyboard::Key::Menu:
            return ImGuiKey_Menu;
        case sf::Keyboard::Key::Num0:
            return ImGuiKey_0;
        case sf::Keyboard::Key::Num1:
            return ImGuiKey_1;
        case sf::Keyboard::Key::Num2:
            return ImGuiKey_2;
        case sf::Keyboard::Key::Num3:
            return ImGuiKey_3;
        case sf::Keyboard::Key::Num4:
            return ImGuiKey_4;
        case sf::Keyboard::Key::Num5:
            return ImGuiKey_5;
        case sf::Keyboard::Key::Num6:
            return ImGuiKey_6;
        case sf::Keyboard::Key::Num7:
            return ImGuiKey_7;
        case sf::Keyboard::Key::Num8:
            return ImGuiKey_8;
        case sf::Keyboard::Key::Num9:
            return ImGuiKey_9;
        case sf::Keyboard::Key::A:
            return ImGuiKey_A;
        case sf::Keyboard::Key::B:
            return ImGuiKey_B;
        case sf::Keyboard::Key::C:
            return ImGuiKey_C;
        case sf::Keyboard::Key::D:
            return ImGuiKey_D;
        case sf::Keyboard::Key::E:
            return ImGuiKey_E;
        case sf::Keyboard::Key::F:
            return ImGuiKey_F;
        case sf::Keyboard::Key::G:
            return ImGuiKey_G;
        case sf::Keyboard::Key::H:
            return ImGuiKey_H;
        case sf::Keyboard::Key::I:
            return ImGuiKey_I;
        case sf::Keyboard::Key::J:
            return ImGuiKey_J;
        case sf::Keyboard::Key::K:
            return ImGuiKey_K;
        case sf::Keyboard::Key::L:
            return ImGuiKey_L;
        case sf::Keyboard::Key::M:
            return ImGuiKey_M;
        case sf::Keyboard::Key::N:
            return ImGuiKey_N;
        case sf::Keyboard::Key::O:
            return ImGuiKey_O;
        case sf::Keyboard::Key::P:
            return ImGuiKey_P;
        case sf::Keyboard::Key::Q:
            return ImGuiKey_Q;
        case sf::Keyboard::Key::R:
            return ImGuiKey_R;
        case sf::Keyboard::Key::S:
            return ImGuiKey_S;
        case sf::Keyboard::Key::T:
            return ImGuiKey_T;
        case sf::Keyboard::Key::U:
            return ImGuiKey_U;
        case sf::Keyboard::Key::V:
            return ImGuiKey_V;
        case sf::Keyboard::Key::W:
            return ImGuiKey_W;
        case sf::Keyboard::Key::X:
            return ImGuiKey_X;
        case sf::Keyboard::Key::Y:
            return ImGuiKey_Y;
        case sf::Keyboard::Key::Z:
            return ImGuiKey_Z;
        case sf::Keyboard::Key::F1:
            return ImGuiKey_F1;
        case sf::Keyboard::Key::F2:
            return ImGuiKey_F2;
        case sf::Keyboard::Key::F3:
            return ImGuiKey_F3;
        case sf::Keyboard::Key::F4:
            return ImGuiKey_F4;
        case sf::Keyboard::Key::F5:
            return ImGuiKey_F5;
        case sf::Keyboard::Key::F6:
            return ImGuiKey_F6;
        case sf::Keyboard::Key::F7:
            return ImGuiKey_F7;
        case sf::Keyboard::Key::F8:
            return ImGuiKey_F8;
        case sf::Keyboard::Key::F9:
            return ImGuiKey_F9;
        case sf::Keyboard::Key::F10:
            return ImGuiKey_F10;
        case sf::Keyboard::Key::F11:
            return ImGuiKey_F11;
        case sf::Keyboard::Key::F12:
            return ImGuiKey_F12;
        default:
            break;
    }

    return ImGuiKey_None;
}

void processSfmlEventWithImGui(sf::Event& evt)
{
    auto& io = ImGui::GetIO();

    if (evt.type == sf::Event::KeyPressed)
    {
        const ImGuiKey mod = keycodeToImGuiMod(evt.key.code);

        // The modifier booleans are not reliable when it's the modifier
        // itself that's being pressed. Detect these presses directly.
        if (mod != ImGuiKey_None)
        {
            io.AddKeyEvent(mod, true);
        }
        else
        {
            io.AddKeyEvent(ImGuiMod_Ctrl, evt.key.control);
            io.AddKeyEvent(ImGuiMod_Shift, evt.key.shift);
            io.AddKeyEvent(ImGuiMod_Alt, evt.key.alt);
            io.AddKeyEvent(ImGuiMod_Super, evt.key.system);
        }

        const ImGuiKey key = keycodeToImGuiKey(evt.key.code);
        io.AddKeyEvent(key, true);
        io.SetKeyEventNativeData(key, static_cast<int>(evt.key.code), -1);
    }
    else if (evt.type == sf::Event::KeyReleased)
    {
        const ImGuiKey mod = keycodeToImGuiMod(evt.key.code);

        // The modifier booleans are not reliable when it's the modifier
        // itself that's being pressed. Detect these presses directly.
        if (mod != ImGuiKey_None)
        {
            io.AddKeyEvent(mod, false);
        }
        else
        {
            io.AddKeyEvent(ImGuiMod_Ctrl, evt.key.control);
            io.AddKeyEvent(ImGuiMod_Shift, evt.key.shift);
            io.AddKeyEvent(ImGuiMod_Alt, evt.key.alt);
            io.AddKeyEvent(ImGuiMod_Super, evt.key.system);
        }

        const ImGuiKey key = keycodeToImGuiKey(evt.key.code);
        io.AddKeyEvent(key, false);
        io.SetKeyEventNativeData(key, static_cast<int>(evt.key.code), -1);
    }

    if (evt.type == sf::Event::TextEntered)
    {
        if (evt.text.unicode >= ' ' || evt.text.unicode != 127)
        {
            io.AddInputCharacter(evt.text.unicode);
        }
    }

    if (evt.type == sf::Event::MouseButtonPressed || evt.type == sf::Event::MouseButtonReleased)
    {
        int mouseButton = evt.mouseButton.button;

        if (mouseButton >= 0 && mouseButton < 3)
        {
            io.AddMouseButtonEvent(mouseButton, evt.type == sf::Event::MouseButtonPressed);
        }
    }

    if (evt.type == sf::Event::MouseWheelScrolled)
    {
        if (evt.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel || (evt.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel && io.KeyShift))
        {
            io.AddMouseWheelEvent(0, evt.mouseWheelScroll.delta);
        }
        else if (evt.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
        {
            io.AddMouseWheelEvent(evt.mouseWheelScroll.delta, 0);
        }
    }

    if (evt.type == sf::Event::MouseMoved)
    {
        io.AddMousePosEvent(evt.mouseMove.x, evt.mouseMove.y);
    }
}

bool renderImGui(std::weak_ptr<sf::Window> windowPtr, float deltaTime)
{
    auto window = windowPtr.lock();

    if (!window)
    {
        std::cout << "Could not obtain window pointer" << std::endl;
        return false;
    }

    auto& io = ImGui::GetIO();

    auto backendData = reinterpret_cast<ImGui_SFML_BackendData*>(io.BackendRendererUserData);

    ImGui::Render();

    auto drawData = ImGui::GetDrawData();

    if (drawData->CmdListsCount < 1)
    {
        std::cout << "no commands to render" << std::endl;
        return true;
    }

    int frameWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int frameHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

    if (frameWidth <= 0 || frameHeight <= 0)
    {
        std::cout << "invalid frame size" << std::endl;
        return false;
    }

    ::glViewport(0, 0, static_cast<GLsizei>(frameWidth), static_cast<GLsizei>(frameHeight));

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(
        static_cast<gl::GLenum>(GL_SRC_ALPHA),
        static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA),
        static_cast<gl::GLenum>(GL_ONE),
        static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA));
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /*
    Some systems might define different clip space origin - top left or  bottom left or whatever.
    To accomodate for this, one can use the following snippet:

    GLenum current_clip_origin = 0;
    glGetIntegerv(GL_CLIP_ORIGIN, reinterpret_cast<GLint*>(&current_clip_origin));

    if (current_clip_origin == GL_UPPER_LEFT)
        clip_origin_lower_left = false;

    Then, to use upper left corner as clip space origin, just swap the TOP and BOTTOM arguments of the orthogonal projection matrix.

    The existing one (x, x + height, y + width, y) is used for bottom-left clip space origin.
    */
    auto orthoProjectionMatrix = glm::ortho(
        drawData->DisplayPos.x,
        drawData->DisplayPos.x + drawData->DisplaySize.x,
        drawData->DisplayPos.y + drawData->DisplaySize.y,
        drawData->DisplayPos.y);

    backendData->projectionMatrix->set(orthoProjectionMatrix);

    backendData->vao->bind();
    backendData->shaderProgram->use();

    ImVec2 clipOffset = drawData->DisplayPos;
    ImVec2 clipScale = drawData->FramebufferScale;

    for (auto i = 0; i < drawData->CmdListsCount; ++i)
    {
        const ImDrawList* cmdList = drawData->CmdLists[i];

        backendData->vertexBuffer->setData(
            cmdList->VtxBuffer.Size * sizeof(ImDrawVert),
            reinterpret_cast<gl::GLvoid*>(cmdList->VtxBuffer.Data),
            static_cast<gl::GLenum>(GL_STREAM_DRAW));

        backendData->indexBuffer->setData(
            cmdList->IdxBuffer.Size * sizeof(ImDrawIdx),
            reinterpret_cast<gl::GLvoid*>(cmdList->IdxBuffer.Data),
            static_cast<gl::GLenum>(GL_STREAM_DRAW));

        for (auto t = 0; t < cmdList->CmdBuffer.Size; ++t)
        {
            const ImDrawCmd* cmd = &cmdList->CmdBuffer[t];

            if (cmd->UserCallback != nullptr)
            {
                if (cmd->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    std::cout << "requested to reset render state" << std::endl;
                }
                else
                {
                    std::cout << "requested user callback" << std::endl;
                    cmd->UserCallback(cmdList, cmd);
                }
            }
            else
            {
                ImVec4 clipRect {
                    (cmd->ClipRect.x - clipOffset.x) * clipScale.x,
                    (cmd->ClipRect.y - clipOffset.y) * clipScale.y,
                    (cmd->ClipRect.z - clipOffset.x) * clipScale.x,
                    (cmd->ClipRect.w - clipOffset.y) * clipScale.y
                };

                if (clipRect.x < frameWidth && clipRect.y < frameHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
                {
                    // enabling the scissor gives the wonky no-clear effect
                    /*::glScissor(
                        static_cast<int>(clipRect.x),
                        static_cast<int>(frameHeight - clipRect.w),
                        static_cast<int>(clipRect.z - clipRect.x),
                        static_cast<int>(clipRect.w - clipRect.y)
                    );*/

                    // backendData->textureUniform->set(reinterpret_cast<gl::GLuint64>(cmd->GetTexID()));

                    backendData->vao->drawElements(
                        static_cast<gl::GLenum>(GL_TRIANGLES),
                        static_cast<GLsizei>(cmd->ElemCount),
                        sizeof(ImDrawIdx) == 2 ? static_cast<gl::GLenum>(GL_UNSIGNED_SHORT) : static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                        reinterpret_cast<void*>(cmd->IdxOffset * sizeof(ImDrawIdx)));
                }
                else
                {
                    std::cout << "out of clipping space" << std::endl;
                }
            }
        }
    }

    backendData->vao->unbind();
    backendData->shaderProgram->release();

    /*{
        auto mousePosition = sf::Mouse::getPosition();
        auto windowPosition = window->getPosition();

        io.MousePos = ImVec2(mousePosition.x - windowPosition.x, mousePosition.y - windowPosition.y);

        io.MouseDown[0] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        io.MouseDown[1] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
        io.MouseDown[2] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
    }*/

    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
    {
        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();

        if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
        {
            window->setMouseCursorVisible(false);
        }
        else
        {
            window->setMouseCursorVisible(true);

            if (auto sfmlCursor = std::weak_ptr<sf::Cursor>(backendData->mouseCursors[cursor]).lock())
            {
                window->setMouseCursor(*sfmlCursor);
            }
            else if (auto pointerCursor = std::weak_ptr<sf::Cursor>(backendData->mouseCursors[ImGuiMouseCursor_Arrow]).lock())
            {
                window->setMouseCursor(*pointerCursor);
            }
        }
    }

    return true;
}
