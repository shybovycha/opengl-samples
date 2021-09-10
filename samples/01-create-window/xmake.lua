add_requires("sfml ~2.5.1", { alias = "sfml" })

target("01-window")
  set_languages("cxx20")
  set_kind("binary")

  add_packages("sfml")

  if is_plat("macosx") then
    -- this prevents "-[SFOpenGLView enableKeyRepeat]: unrecognized selector sent to instance 0x7fa5c2507970" runtime exception
    add_ldflags("-ObjC")

    -- this prevents linker errors
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("main.cpp")
