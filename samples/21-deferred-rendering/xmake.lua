add_requires("sfml ~2.5.1", { alias = "sfml" })
add_requires("glm")
add_requires("vcpkg::globjects", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })
add_requires("assimp")

target("21-deferred-rendering")
  set_languages("cxx20")
  set_kind("binary")

  add_packages("sfml", "glm", "globjects", "glbinding", "assimp")

  if is_plat("macosx") then
    -- this prevents "-[SFOpenGLView enableKeyRepeat]: unrecognized selector sent to instance 0x7fa5c2507970" runtime exception
    add_ldflags("-ObjC")

    -- this prevents linker errors
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("src/main.cpp", "src/common/AbstractMesh.cpp", "src/common/AbstractMeshBuilder.cpp", "src/common/AbstractSkyboxBuilder.cpp", "src/common/AssimpModel.cpp", "src/common/CubemapSkyboxBuilder.cpp", "src/common/MultimeshModel.cpp", "src/common/SimpleSkyboxBuilder.cpp", "src/common/SingleMeshModel.cpp", "src/common/Skybox.cpp")
  
  set_pcxxheader("src/stdafx.hpp")
  
  add_includedirs("src/")

  after_build(function (target)
    os.cp("$(scriptdir)/../media", path.join(path.directory(target:targetfile()), "media"))
    os.cp("$(scriptdir)/media", path.join(path.directory(target:targetfile()), "media"))
  end)
