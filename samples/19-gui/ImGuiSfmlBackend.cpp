#include "ImGuiSfmlBackend.hpp"

void initImGuiKeyMappings(ImGuiIO& io)
{
    io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
    io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
    io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
    io.KeyMap[ImGuiKey_PageUp] = sf::Keyboard::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = sf::Keyboard::PageDown;
    io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
    io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
    io.KeyMap[ImGuiKey_Insert] = sf::Keyboard::Insert;
    io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
    io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
    io.KeyMap[ImGuiKey_Space] = sf::Keyboard::Space;
    io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
    io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
    io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
    io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
    io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
    io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
    io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
    io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;
}

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

    ImTextureID fontAtlasTextureId = reinterpret_cast<void*>(fontsTexture->textureHandle().handle());

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
    io.SetClipboardTextFn = [](void* userData, const char* text)
    {
        sf::Clipboard::setString(sf::String::fromUtf8(text, text + std::strlen(text)));
    };

    io.GetClipboardTextFn = [](void* userData) -> const char*
    {
        auto utf8Text = sf::Clipboard::getString().toUtf8();
        auto plainText = std::string(utf8Text.begin(), utf8Text.end());
        return plainText.c_str();
    };
}

void initImGuiStyles(ImGuiIO& io)
{
    ImGui::StyleColorsLight();
}

void initImGuiShaders(ImGuiIO& io)
{
    const auto vertexShaderSourceStr = R"EOS(
        #version 410 core

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
        #version 410 core

        in vec2 Frag_UV;
        in vec4 Frag_Color;

        uniform sampler2D surfaceTexture;

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
    backendData->projectionMatrix = shaderProgram->getUniform<glm::mat4>("projection");

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

    backendData->vao = std::move(vao);
    backendData->indexBuffer = std::move(indexBuffer);
    backendData->vertexBuffer = std::move(vertexBuffer);

    backendData->vertexShader = std::move(vertexShader);
    backendData->fragmentShader = std::move(fragmentShader);
    backendData->shaderProgram = std::move(shaderProgram);
}

void initImGuiDisplay(ImGuiIO& io, std::shared_ptr<sf::Window> windowPtr)
{
    io.DisplaySize = ImVec2{ static_cast<float>(windowPtr->getSize().x), static_cast<float>(windowPtr->getSize().y) };
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

    initImGuiBackendData(io);
    initImGuiConfigurationFlags(io);
    initImGuiDisplay(io, std::move(window));
    initImGuiKeyMappings(io);
    initImGuiCursorMapping(io);
    initImGuiFonts(io);
    initImGuiFontAtlas(io);
    initImGuiClipboard(io);
    initImGuiStyles(io);
    initImGuiShaders(io);

    return true;
}

void processSfmlEventWithImGui(sf::Event& evt)
{
    auto& io = ImGui::GetIO();

    if (evt.type == sf::Event::KeyPressed || evt.type == sf::Event::KeyReleased)
    {
        auto keyCode = evt.key.code;

        if (keyCode >= 0 && keyCode < IM_ARRAYSIZE(io.KeysDown))
        {
            io.KeysDown[keyCode] = (evt.type == sf::Event::KeyPressed);
        }

        io.KeyCtrl = evt.key.control;
        io.KeyShift = evt.key.shift;
        io.KeyAlt = evt.key.alt;
        io.KeySuper = evt.key.system;
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
            io.MouseDown[mouseButton] = (evt.type == sf::Event::MouseButtonPressed);
        }
    }

    if (evt.type == sf::Event::MouseWheelScrolled)
    {
        if (evt.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel || (evt.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel && io.KeyShift))
        {
            io.MouseWheel += evt.mouseWheelScroll.delta;
        }
        else if (evt.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
        {
            io.MouseWheelH += evt.mouseWheelScroll.delta;
        }
    }

    if (evt.type == sf::Event::MouseMoved)
    {
        io.MousePos = ImVec2(evt.mouseMove.x, evt.mouseMove.y);
    }
}

bool renderImGui(std::weak_ptr<sf::Window> windowPtr, float deltaTime)
{
    auto window = windowPtr.lock();

    if (!window)
    {
        return false;
    }

    auto& io = ImGui::GetIO();

    auto backendData = reinterpret_cast<ImGui_SFML_BackendData*>(io.BackendRendererUserData);

    ImGui::Render();

    auto drawData = ImGui::GetDrawData();

    if (drawData->CmdListsCount < 1)
    {
        return true;
    }

    int frameWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int frameHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

    if (frameWidth <= 0 || frameHeight <= 0)
    {
        return false;
    }

    ::glViewport(0, 0, static_cast<GLsizei>(frameWidth), static_cast<GLsizei>(frameHeight));

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(
        static_cast<gl::GLenum>(GL_SRC_ALPHA),
        static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA),
        static_cast<gl::GLenum>(GL_ONE),
        static_cast<gl::GLenum>(GL_ONE_MINUS_SRC_ALPHA)
    );
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
        drawData->DisplayPos.y
    );

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
            static_cast<gl::GLenum>(GL_STREAM_DRAW)
        );

        backendData->indexBuffer->setData(
            cmdList->IdxBuffer.Size * sizeof(ImDrawIdx),
            reinterpret_cast<gl::GLvoid*>(cmdList->IdxBuffer.Data),
            static_cast<gl::GLenum>(GL_STREAM_DRAW)
        );

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
                    cmd->UserCallback(cmdList, cmd);
                }
            }
            else
            {
                ImVec4 clipRect{
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

                    backendData->textureUniform->set(reinterpret_cast<gl::GLuint64>(cmd->GetTexID()));

                    backendData->vao->drawElements(
                        static_cast<gl::GLenum>(GL_TRIANGLES),
                        static_cast<GLsizei>(cmd->ElemCount),
                        sizeof(ImDrawIdx) == 2 ? static_cast<gl::GLenum>(GL_UNSIGNED_SHORT) : static_cast<gl::GLenum>(GL_UNSIGNED_INT),
                        reinterpret_cast<void*>(cmd->IdxOffset * sizeof(ImDrawIdx))
                    );
                }
            }
        }
    }

    backendData->vao->unbind();

    backendData->shaderProgram->release();

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
