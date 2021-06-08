add_requires("sfml ~2.5.1", { alias = "sfml" })
add_requires("glm")
add_requires("vcpkg::globjects ~1.1.0", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })

target("01-hello-window")
  set_kind("binary")
  set_languages("cxx20")

  add_packages("sfml")

  if is_plat("macosx") then
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("samples/01-hello-window/main.cpp")

target("02-hello-opengl")
  set_kind("binary")
  set_languages("cxx20")

  add_packages("sfml", "glm", "globjects", "glbinding")

  if is_plat("macosx") then
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("samples/02-hello-opengl/main.cpp")

target("03-hello-camera")
  set_kind("binary")
  set_languages("cxx20")

  add_packages("sfml", "glm", "globjects", "glbinding")

  if is_plat("macosx") then
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("samples/03-hello-camera/main.cpp")
