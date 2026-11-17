# RmlUi with a C API (fork)

This repository is a fork of [RmlUi](https://github.com/mikke89/RmlUi) that adds a C API, so that RmlUi can be used from languages that can call C but not C++. The first target is the [Odin](https://odin-lang.org) programming language (available in [RmlUi-odin](https://github.com/GeorgeGkas/RmlUi-odin) repo), but nothing in the API is specific to Odin. The original RmlUi README continues [further down](#rmlui---the-htmlcss-user-interface-library-evolved).

## Why this fork exists

RmlUi is a C++ library. Its API is made of classes, virtual functions, templates and C++ ownership rules, and none of that can be called from another language without a C++ compiler on the other side. Most languages with a foreign function interface, such as Odin, Zig, Rust, Go, C#, Python or plain C, can only call functions that use the C ABI.

The C API wraps RmlUi's C++ API in plain C99 functions and structs:

- Contexts, elements, documents, events and other objects are opaque handles such as `RmlContext*` and `RmlElement*`.
- Strings are passed in as `RmlStringView` (a pointer and a length, borrowed for the call) and returned as `RmlOwnedString` (heap memory that you free with `rml_owned_string_free`).
- Everything that C++ users implement by subclassing is implemented by filling in a struct of callbacks, each of which receives a `void* user_data`. This covers the system, render and file interfaces, font engines, text input handlers, custom elements, decorators, filters, font effects, event listeners, instancers, plugins and data model types.

The headers are written to be fed to binding generators. For example, [odin-c-bindgen](https://github.com/karl-zylinski/odin-c-bindgen) generates an Odin package for every exported function without manual edits.

## What the fork adds

- **The C API library, `rmlui_capi`.** The headers are in [`Include/RmlUi/CApi/`](Include/RmlUi/CApi), with the umbrella header [`Include/RmlUi/CApi.h`](Include/RmlUi/CApi.h), and the implementation is in [`Source/CApi/`](Source/CApi). It currently exports 468 functions, covering contexts, the element tree and form controls, events, data models and data bindings, style sheets, variants, the debugger, all of the interfaces listed above, and drawing geometry with textures and shaders from custom elements and decorators.
- **An optional backend wrapper, `rmlui_capi_backend`.** [`Backend.h`](Include/RmlUi/CApi/Backend.h) exposes the window, input and rendering backend from RmlUi's samples, for getting started without writing a renderer.
- **A test suite** in [`Tests/Source/CApi/`](Tests/Source/CApi). It uses the C API only through C callbacks, as a binding would, and also checks that every header compiles as strict C99.
- **Two fixes to the core library**, found while testing the C API:
  - Loading a style sheet from a file that doesn't exist returns null, instead of parsing a stream that never opened.
  - The debugger no longer crashes on shutdown after it has been switched from one context to another.

Apart from those two fixes, the core library is the same as upstream.

## Building

The C API is off by default. These CMake options control it:

| Option | Effect |
|---|---|
| `RMLUI_CAPI=ON` | Builds `rmlui_capi`. |
| `RMLUI_CAPI_BACKEND=ON` | Also builds `rmlui_capi_backend` for the backend selected by `RMLUI_BACKEND`. Requires `RMLUI_CAPI`. |
| `BUILD_SHARED_LIBS=ON` | Recommended when using RmlUi from another language, so that you link one shared library instead of the static C++ libraries and the C++ runtime. |
| `BUILD_TESTING=ON` | Also builds the C API tests, `rmlui_capi_tests`, when `RMLUI_CAPI` is on. As with the upstream tests, this needs the dependencies of the selected `RMLUI_BACKEND`. |

```sh
cmake -B build -DRMLUI_CAPI=ON -DBUILD_SHARED_LIBS=ON
cmake --build build
```

A minimal program using the backend wrapper looks like this, with error handling left out. Run it from the repository root so that the sample assets are found.

```c
#include <RmlUi/CApi.h>
#include <RmlUi/CApi/Backend.h>

int main(void)
{
    if (!rml_backend_initialize(rml_sv("Hello RmlUi"), 1024, 768, true))
        return 1;
    rml_set_system_interface(rml_backend_get_system_interface());
    rml_set_render_interface(rml_backend_get_render_interface());
    rml_initialise();

    RmlContext* ctx = rml_create_context(rml_sv("main"), (RmlVector2i){1024, 768}, NULL, NULL);
    rml_load_font_face(rml_sv("Samples/assets/LatoLatin-Regular.ttf"), false, RML_FONT_WEIGHT_AUTO, 0);
    RmlElementDocument* document = rml_context_load_document(ctx, rml_sv("Samples/assets/demo.rml"));
    rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);

    while (rml_backend_process_events(ctx, NULL, NULL, false))
    {
        rml_context_update(ctx);
        rml_backend_begin_frame();
        rml_context_render(ctx);
        rml_backend_present_frame();
    }

    rml_shutdown();
    rml_backend_shutdown();
    return 0;
}
```

## API conventions

The header comments document ownership and lifetimes for each function. The general rules are:

- Functions are named `rml_<object>_<action>`, types `Rml<Name>`, and constants `RML_<NAME>`.
- `RmlStringView` arguments are borrowed for the duration of the call and don't need to be null-terminated. `RmlOwnedString` results belong to you and must be freed with `rml_owned_string_free`.
- Callback structs are copied when the object is created, and `user_data` is passed back to every callback unchanged. Objects that you register with RmlUi, such as interfaces, instancers and plugins, must stay alive until after `rml_shutdown`.
- Elements belong to RmlUi once they are part of a document. An element that you have created but not inserted is *caller-owned*: either insert it or destroy it with `rml_element_release`. See [`Element.h`](Include/RmlUi/CApi/Element.h).
- If you pass a null or otherwise invalid argument, the function logs an error through the system interface and returns a zero value. This also happens in release builds, so mistakes in bindings show up in the log instead of crashing.

## Limitations

- **The C API wraps C++; it doesn't replace it.** It calls into the C++ core library, so a program that uses it still links RmlUi and needs a C++ runtime. Linking the shared library keeps that out of your language's build.
- **There is no stable ABI yet.** Functions and struct layouts can still change from one commit of this fork to the next. Pin a commit, and regenerate your bindings whenever you update.
- **It is single-threaded, like RmlUi.** Call the API from one thread. Callbacks run on that thread, during the API call that triggers them.
- **Lifetimes are manual.** The API can't detect dangling handles or use after free. For example, a data model handle used after its model has been removed isn't detected, and geometry, compiled shaders and texture sources must be destroyed before the render manager they belong to is released (see [`Render.h`](Include/RmlUi/CApi/Render.h)).
- **Some features are not wrapped:**
  - The Lua plugin. Its entry point, `Rml::Lua::Initialise`, is a C++ function, so another language can't start the plugin without a small C wrapper, which this fork doesn't provide yet.
  - Rendering into a texture. C++ code can bake effects into textures using the render manager's layers, scissor region and render state, but none of these are exposed. Custom elements and decorators can still draw geometry with textures and shaders through `Render.h`.
  - Custom XML node handlers.
  - Creating variant lists. Variant lists are only received, as event and transform arguments.
- **SVG and Lottie need no extra API.** When RmlUi is built with `RMLUI_SVG_PLUGIN` or `RMLUI_LOTTIE_PLUGIN`, `rml_initialise` starts the plugins, and you use them from RML and RCSS, with `<svg>`, `decorator: svg(...)` and `<lottie>`. This should work through the C API, but the C API tests don't cover it.
- **Bindings need to handle a few details:**
  - `rml_sv` is a `static inline` helper in [`Types.h`](Include/RmlUi/CApi/Types.h), so it has no symbol in the library. Bindings need their own equivalent.
  - The memory layout of `RmlMatrix4f` depends on the `RMLUI_MATRIX_ROW_MAJOR` build option. Check it at runtime with `rml_matrix_is_row_major()`.
  - `rml_variant_set_vector2i` stores a float vector, because RmlUi has no integer vector variant, so components larger than 2^24 lose precision.
  - `rml_element_has_property` returns true for every registered property, as in C++. Use `rml_element_has_local_property` to check whether the element itself sets the property.
- **Testing is limited so far.** The C API tests have been run on macOS (Apple Clang, arm64), including under AddressSanitizer, UndefinedBehaviorSanitizer and the macOS `leaks` tool. They don't run in CI yet, and Windows and Linux builds haven't been tested. The backend wrapper only has a test that checks its name. Odin bindings have been generated with odin-c-bindgen for every header except `Backend.h`, but haven't yet been compiled and run against the library.

## Use of AI

This fork was developed with extensive use of AI coding agents. The agents wrote the C API, its tests and its documentation. An agent also reviewed the finished C API for correctness and for coverage of the C++ API. That review found 28 issues, 2 in the core library and 26 in the C API, and all of them have been fixed and covered by regression tests.

Keep the following in mind when you use this fork:

- Every function exported by `rmlui_capi` is called by at least one test. However, tests only catch what they check for, and AI-written code can be wrong in ways that look plausible.
- None of this work has been reviewed by the RmlUi maintainers.
- Treat the C API as experimental. Rely on the header comments for the exact behaviour of each function, rather than assuming it matches the C++ API.

## Relationship with upstream

This fork follows [mikke89/RmlUi](https://github.com/mikke89/RmlUi). The C API currently sits on top of upstream commit `c29f20d7`. It is not part of upstream RmlUi, so please report problems with the C API on [this fork](https://github.com/GeorgeGkas/RmlUi/issues) rather than upstream. Problems in RmlUi itself that also happen with upstream belong in the [upstream issue tracker](https://github.com/mikke89/RmlUi/issues).

The fork keeps RmlUi's MIT license; see [`LICENSE.txt`](LICENSE.txt).

---

# RmlUi - The HTML/CSS User Interface Library Evolved

![RmlUi logo](https://github.com/mikke89/RmlUiDoc/raw/c7253748d1bcf6dd33d97ab4fe8b6731a7ee3dac/assets/rmlui.png)

RmlUi - now with added boosters taking control of the rocket, targeting *your* games and applications.

---

[![project chat](https://img.shields.io/badge/zulip-join_chat-brightgreen.svg)](https://rmlui.zulipchat.com)
[![Build](https://github.com/mikke89/RmlUi/actions/workflows/build.yml/badge.svg)](https://github.com/mikke89/RmlUi/actions/workflows/build.yml)
[![Build status](https://ci.appveyor.com/api/projects/status/x95oi8mrb001pqhh/branch/master?svg=true)](https://ci.appveyor.com/project/mikke89/rmlui/branch/master)

RmlUi is the C++ user interface package based on the HTML and CSS standards, designed as a complete solution for any project's interface needs. It is a fork of the [libRocket](https://github.com/libRocket/libRocket) project, introducing new features, bug fixes, and performance improvements.

RmlUi aims at being a light-weight and performant library with its own layout engine and few dependencies. In essence, RmlUi takes your HTML/CSS-like source files and turns them into vertices, indices and draw commands, and then you bring your own renderer to draw them. And of course there is full access to the element hierarchy/DOM, event handling, and all the interactivity and customizability you would expect. All of this directly from C++, or optionally from scripting languages using plugins. The core library compiles down to fractions of the size it takes to integrate a fully fledged web browser. 

RmlUi is based around the XHTML1 and CSS2 standards while integrating features from HTML5 and CSS3, and extends them with features suited towards real-time applications. Take a look at the [conformance](#conformance) and [enhancements](#enhancements) sections below for details.

Documentation is located at https://mikke89.github.io/RmlUiDoc/

## Features

- Cross-platform architecture: Windows, Linux, macOS, Android, iOS, Switch, and more.
- Dynamic layout system.
- Full animation and transform support.
- Efficient application-wide styling, with a custom-built templating engine.
- Fully featured control set: buttons, sliders, drop-downs, and more.
- Runtime visual debugging suite.

## Extensible

- Abstracted interfaces for plugging in to any game engine.
- Decorator engine allowing custom application-specific effects that can be applied to any element.
- Generic event system that binds seamlessly into existing projects.
- Easily integrated and extensible with the Lua scripting plugin.

## Controllable

- The user controls their own update loop, calling into RmlUi as desired.
- The library strictly runs as a result of calls to its API, never in the background.
- Input handling and rendering is performed by the user.
- The library generates vertices, indices, and textures for the user to render how they like.
- File handling and the font engine can optionally be fully replaced by the user.

## Conformance

RmlUi aims to support the most common and familiar features from HTML and CSS, while keeping the library light and performant. We do not aim to be fully compliant with CSS or HTML, in particular when it conflicts with lightness and performance. Users are generally expected to author documents specifically for RmlUi, but any experience and skills from web design should be transferable.

RmlUi supports most of CSS2 with some CSS3 features such as

- Animations and transitions
- Transforms (with full interpolation support)
- Flexbox layout
- Media queries
- Border radius
- Custom properties and variables
- Box shadows and mask images
- Gradients (linear, radial, and conic) as decorators
- Filters and backdrop filters (with all CSS filter functions)

and many of the common HTML elements including `<input>`,  `<textarea>`, and `<select>`.

For details, see
- [RCSS Property index](https://mikke89.github.io/RmlUiDoc/pages/rcss/property_index.html) for all supported properties and differences from CSS.
- [RML Element index](https://mikke89.github.io/RmlUiDoc/pages/rml/element_index.html) for all supported elements.

## Enhancements

RmlUi adds features and enhancements over CSS and HTML where it makes sense, most notably the following.

- [Data binding (model-view-controller)](https://mikke89.github.io/RmlUiDoc/pages/data_bindings.html). Synchronization between application data and user interface.
- [Decorators](https://mikke89.github.io/RmlUiDoc/pages/rcss/decorators.html). Full control over the styling of [all elements](https://mikke89.github.io/RmlUiDoc/pages/style_guide.html).
- [Sprite sheets](https://mikke89.github.io/RmlUiDoc/pages/rcss/sprite_sheets.html). Define and use sprites with easy high DPI support.
- [Templates](https://mikke89.github.io/RmlUiDoc/pages/rml/templates.html). Making windows look consistent.
- [Localization](https://mikke89.github.io/RmlUiDoc/pages/localisation.html). Translate any text in the document.
- [Spatial navigation](https://mikke89.github.io/RmlUiDoc/pages/rcss/user_interface.html#nav). Suitable for controllers.

## Dependencies

- [FreeType](https://www.freetype.org/). However, it can be fully replaced by a custom [font engine](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/interfaces/font_engine.html).
- The standard library.

In addition, a C++17 compatible compiler is required.


## Building RmlUi

### CMake

RmlUi can be built using CMake and your favorite compiler, see the [building documentation](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/building_with_cmake.html) for details and all the options. Windows binaries are also available for the [latest release](https://github.com/mikke89/RmlUi/releases/latest).

### vcpkg

RmlUi can also be fetched from a dependency manager such as [vcpkg](https://vcpkg.io/en/getting-started.html).

```
vcpkg install rmlui
```

That's it! See below for details on integrating RmlUi.

To build RmlUi with the included samples we can use git and CMake together with vcpkg to handle dependencies.

```
vcpkg install freetype glfw3
git clone https://github.com/mikke89/RmlUi.git
cd RmlUi
cmake -B Build -S . --preset samples -DRMLUI_BACKEND=GLFW_GL3 -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake"
cmake --build Build
```
Make sure to replace `<path-to-vcpkg>` as appropriate. This example uses the `GLFW_GL3` backend, other backends are available as shown below. When this completes, feel free to test the freshly built samples, such as the `invaders` sample (`rmlui_sample_invaders` target), and enjoy! The executables should be located somewhere in the `Build` directory.

To make all the samples available, you can additionally install `lua lunasvg rlottie harfbuzz` and pass `--preset samples-all` during CMake configuration.

### Conan

RmlUi is readily available from [ConanCenter](https://conan.io/center/recipes/rmlui).


## Integrating RmlUi

Here are the general steps to integrate the library into a C++ application, have a look at the [integration documentation](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/integrating.html) for details.

1. Build RmlUi as above or fetch the binaries, and [link it up](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/integrating.html#setting-up-the-build-environment) to your application.
2. Implement the abstract [render interface](Include/RmlUi/Core/RenderInterface.h), or fetch one of the backends listed below.
3. Initialize RmlUi with the interfaces, create a context, provide font files, and load a document.
4. Call into the context's update and render methods in a loop, and submit input events.
5. Compile and run!

Several [samples](Samples/) demonstrate everything from basic integration to more complex use of the library, feel free to have a look for inspiration.


## RmlUi Backends

To ease the integration of RmlUi, the library includes [many backends](Backends/) adding support for common renderers and platforms. The following terms are used here:

- ***Renderer***: Implements the [render interface](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/interfaces/render.html) for a given rendering API, and provides initialization code when necessary.
- ***Platform***: Implements the [system interface](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/interfaces/system.html) for a given platform (operating system or window library), and adds procedures for providing input to RmlUi contexts.
- ***Backend***: Combines a renderer and a platform for a complete windowing framework sample, implementing the basic [Backend interface](Backends/RmlUi_Backend.h).

The provided renderers and platforms are intended to be usable as-is by client projects without modifications, thereby circumventing the need to write custom interfaces. We encourage users to only make changes here when they are useful to all users, and then contribute back to the project. However, if they do not meet your needs it is also possible to copy them into your project for modifications. Feedback is welcome to find the proper abstraction level. The provided system and render interfaces are designed such that they can be derived from and further customized by the backend or end user.

The provided backends on the other hand are not intended to be used directly by client projects, but rather copied and modified as needed. They are intentionally light-weight and implement just enough functionality to make the [included samples](Samples/) run, while being simple to understand and build upon by users. See the manual for [backend integration details](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/integrating.html#backends).

### Renderers

| Renderer features | Basic rendering | Transforms | Clip masks | Filters | Shaders | Built-in image support                                            |
|-------------------|:---------------:|:----------:|:----------:|:-------:|:-------:|-------------------------------------------------------------------|
| OpenGL 3 (GL3)*   |       ✔️        |     ✔️     |     ✔️     |   ✔️    |   ✔️    | Uncompressed TGA                                                  |
| OpenGL 2 (GL2)    |       ✔️        |     ✔️     |     ✔️     |   ❌    |   ❌    | Uncompressed TGA                                                  |
| Vulkan (VK)       |       ✔️        |     ✔️     |     ✔️     |   ✔️    |   ✔️    | Uncompressed TGA                                                  |
| SDL GPU           |       ✔️        |     ✔️     |     ❌     |   ❌    |   ❌    | Based on [SDL_image](https://wiki.libsdl.org/SDL_image/FrontPage) |
| DirectX 11¹       |       ✔️        |     ✔️     |     ✔️     |   🟡    |   ✔️    | Uncompressed TGA                                                  |
| DirectX 12        |       ✔️        |     ✔️     |     ✔️     |   ✔️    |   ✔️    | Uncompressed TGA                                                  |

*\* Reference implementation* \
*¹ Filters are fully featured, but some open issues remain ([details](https://github.com/mikke89/RmlUi/pull/675#issuecomment-2821714716))*

**Basic rendering**: Render geometry with colors, textures, and rectangular clipping (scissoring). Sufficient for basic 2D layouts.\
**Transforms**: Enables the `transform` and `perspective` properties to take effect.\
**Clip masks**: Enables proper clipping of transformed elements, elements with border-radius.\
**Filters**: Support for the `filter` and `backdrop-filter` properties with all filter functions, such as blur and drop-shadow.\
**Shaders**: Support for all built-in decorators that require shaders, such as `linear-gradient` and `radial-gradient`. Other advanced rendering functions are also implemented, including render-to-texture and masking support for properties such as `box-shadow` and `mask-image`.\
**Built-in image support**: This only shows the supported formats built-in to the renderer, users are encouraged to derive from and extend the render interface to add support for their desired image formats.

### Platforms

| Platform | Basic windowing | Clipboard | High DPI | Touch | IME | Comments                                                                      |
|----------|:---------------:|:---------:|:--------:|:-----:|:---:|-------------------------------------------------------------------------------|
| SDL      |       ✔️        |    ✔️     |    ✔️    |  ✔️   | ✔️  | Supports SDL 2 and SDL 3. High DPI supported only on SDL 3.                   |
| GLFW     |       ✔️        |    ✔️     |    ✔️    |  ❌   | ❌  |                                                                               |
| Win32    |       ✔️        |    ✔️     |    ✔️    |  ❌   | ✔️  |                                                                               |
| X11      |       ✔️        |    ✔️     |    ❌    |  ❌   | ❌  |                                                                               |
| Wayland  |       ✔️        |    ❌     |    ❌    |  ❌   | ❌  |                                                                               |
| SFML     |       ✔️        |    🟡     |    ❌    |  ❌   | ❌  | Supports SFML 2 and SFML 3. Some issues with Unicode characters in clipboard. |

**Basic windowing**: Open windows, react to resize events, submit inputs to the RmlUi context.\
**Clipboard**: Read from and write to the system clipboard.\
**High DPI**: Scale the [dp-ratio](https://mikke89.github.io/RmlUiDoc/pages/rcss/syntax.html#dp-unit) of RmlUi contexts based on the monitor's DPI settings. React to DPI-changes, either because of changed settings or when moving the window to another monitor. \
**Touch**: Process touch events, enable dragging and inertial scrolling with touch movement. \
**IME**: [Input method editor](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/ime.html) support, for typing characters not available on QWERTY keyboards.

### Backends

| Platform \ Renderer |     OpenGL 2      |      OpenGL 3       |      Vulkan      |     SDL GPU     |     DirectX 11     |     DirectX 12     |
|---------------------|:-----------------:|:-------------------:|:----------------:|:---------------:|:------------------:|:------------------:|
| Win32               | ✔️<br>`Win32_GL2` |                     | ✔️<br>`Win32_VK` |                 | ✔️<br>`Win32_DX11` | ✔️<br>`Win32_DX12` |
| X11                 |  ✔️<br>`X11_GL2`  |                     |                  |                 |                    |                    |
| Wayland             |                   | ✔️<br>`Wayland_GL3` |                  |                 |                    |                    |
| SFML                | ✔️<br>`SFML_GL2`  |                     |                  |                 |                    |                    |
| GLFW                | ✔️<br>`GLFW_GL2`  |  ✔️<br>`GLFW_GL3`   | ✔️<br>`GLFW_VK`  |                 | ✔️<br>`GLFW_DX11`  | ✔️<br>`GLFW_DX12`  |
| SDL¹                |  ✔️<br>`SDL_GL2`  |  ✔️²<br>`SDL_GL3`   |  ✔️<br>`SDL_VK`  | ✔️<br>`SDL_GPU` |                    |  ✔️<br>`SDL_DX12`  |

¹ SDL backends extend their respective renderers to provide image support based on SDL_image.\
² Supports Emscripten compilation target.

When building the samples, the backend can be selected by setting the CMake option `RMLUI_BACKEND` to a combination of a platform and renderer, according to the above table.


## Example document

This example demonstrates a basic document with [data bindings](https://mikke89.github.io/RmlUiDoc/pages/data_bindings.html), which is loaded and displayed using the C++ code below.

#### Document

`hello_world.rml`

```html
<rml>
<head>
    <title>Hello world</title>
    <link type="text/rcss" href="rml.rcss"/>
    <link type="text/rcss" href="window.rcss"/>
</head>
<body data-model="animals">
    <h1>RmlUi</h1>
    <p>Hello <span id="world">world</span>!</p>
    <p data-if="show_text">The quick brown fox jumps over the lazy {{animal}}.</p>
    <input type="text" data-value="animal"/>
</body>
</rml>
```
The `{{animal}}` text and the `data-if`, `data-value` attributes represent data bindings and will synchronize with the application data.

#### Style sheet

`window.rcss`

```css
body {
    font-family: LatoLatin;
    font-size: 18px;
    color: #02475e;
    background: #fefecc;
    text-align: center;
    padding: 2em 1em;
    position: absolute;
    border: 2px #ccc;
    width: 500px;
    height: 200px;
    margin: auto;
}

h1 {
    color: #f6470a;
    font-size: 1.5em;
    font-weight: bold;
    margin-bottom: 0.7em;
}

p { 
    margin: 0.7em 0;
}

input.text {
    background-color: #fff;
    color: #555;
    border: 2px #999;
    padding: 5px;
    tab-index: auto;
    cursor: text;
    box-sizing: border-box;
    width: 200px;
    font-size: 0.9em;
}
```

RmlUi defines no styles internally, thus the `input` element is styled here, and [`rml.rcss` is included](Samples/assets/rml.rcss) for proper layout of common tags.


#### C++ Initialization and loop

`main.cpp`

```cpp
#include <RmlUi/Core.h>

class MyRenderInterface : public Rml::RenderInterface
{
    // RmlUi sends vertices, indices and draw commands through this interface for your
    // application to render how you'd like.
    /* ... */
};

struct ApplicationData {
    bool show_text = true;
    Rml::String animal = "dog";
} my_data;

int main(int argc, char** argv)
{
    // Initialize the window and graphics API being used, along with your game or application.

    /* ... */

    MyRenderInterface render_interface;

    // Install the custom interfaces.
    Rml::SetRenderInterface(&render_interface);

    // Now we can initialize RmlUi.
    Rml::Initialise();

    // Create a context to display documents within.
    Rml::Context* context =
        Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));

    // Tell RmlUi to load the given fonts.
    Rml::LoadFontFace("LatoLatin-Regular.ttf");
    // Fonts can be registered as fallback fonts, as in this case to display emojis.
    Rml::LoadFontFace("NotoEmoji-Regular.ttf", true);

    // Set up data bindings to synchronize application data.
    if (Rml::DataModelConstructor constructor = context->CreateDataModel("animals"))
    {
        constructor.Bind("show_text", &my_data.show_text);
        constructor.Bind("animal", &my_data.animal);
    }

    // Now we are ready to load our document.
    Rml::ElementDocument* document = context->LoadDocument("hello_world.rml");
    document->Show();

    // Replace and style some text in the loaded document.
    Rml::Element* element = document->GetElementById("world");
    element->SetInnerRML(reinterpret_cast<const char*>(u8"🌍"));
    element->SetProperty("font-size", "1.5em");

    bool exit_application = false;
    while (!exit_application)
    {
        // We assume here that we have some way of updating and retrieving inputs internally.
        if (my_input->KeyPressed(KEY_ESC))
            exit_application = true;

        // Submit input events such as MouseMove and key events (not shown) to the context.
        if (my_input->MouseMoved())
            context->ProcessMouseMove(mouse_pos.x, mouse_pos.y, 0);

        // Update the context to reflect any changes resulting from input events, animations,
        // modified and added elements, or changed data in data bindings.
        context->Update();

        // Prepare the application for rendering, such as by clearing the window. This calls
        // into the RmlUi backend interface, replace with your own procedures as appropriate.
        Backend::BeginFrame();
        
        // Render the user interface. All geometry and other rendering commands are now
        // submitted through the render interface.
        context->Render();

        // Present the rendered content, such as by swapping the swapchain. This calls into
        // the RmlUi backend interface, replace with your own procedures as appropriate.
        Backend::PresentFrame();
    }

    Rml::Shutdown();

    return 0;
}
```

#### Rendered output

![Hello world document](Samples/assets/hello_world.png)

Users can now edit the text field to change the animal. The data bindings ensure that both the document text and the application string `my_data.animal` are automatically modified accordingly.


## Gallery

### Game menu

![Game menu](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/menu_screen.png?raw=true)

### RmlUi 'invaders' sample

![Game interface](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/invader.png?raw=true)

### LichtFeld Studio

[LichtFeld Studio](https://lichtfeld.io). Open-source software for 3D Gaussian Splatting reconstruction and editing. The whole user interface is made with RmlUi, from the panels and asset manager to the built-in Python editor.

![LichtFeld Studio](https://raw.githubusercontent.com/MrNeRF/LichtFeld-Studio/02492ad3955c5967e9d660e99811ea37298e9694/lichtfeld-studio-rmlui-gallery.webp)

### The Thing: Remastered

[The Thing: Remastered](https://nightdivestudios.com/the-thing-remastered/) by [Nightdive Studios](https://www.nightdivestudios.com/). Restoration of the cult-classic 2002 third-person survival horror shooter game inspired by the 1982 film, The Thing. User interface made with RmlUi.

![The Thing: Remastered collage](https://github.com/user-attachments/assets/de716d13-a770-4237-b4b3-b7f41b13494d)

### Alchemist

[Alchemist](https://docs.fivem.net/docs/alchemist/) by [Cfx.re](https://cfx.re/) (Rockstar Games). Tool to convert assets from GTAV Legacy to GTAV Enhanced.

<img width="3606" height="1043" alt="2025-11-25-alchemist" src="https://github.com/user-attachments/assets/cc5dc760-5eb4-4a9a-a6f2-f914a87452f8" />

### Gearmulator

[Gearmulator](https://github.com/dsp56300/gearmulator) by [The Usual Suspects](https://dsp56300.wordpress.com/). Family of emulators for classic VA synths of the late 90s/00s with the Motorola 56300 family DSPs.

<img width="1784" height="878" alt="Image" src="https://github.com/user-attachments/assets/f430750a-f186-42a2-9f03-70dfe01c4a7d" />

<img width="1785" height="877" alt="Image" src="https://github.com/user-attachments/assets/b217cf9d-184d-4fa5-8ad5-96135b6bb768" />

### WOTInspector

[WOTInspector](https://armor.wotinspector.com/en/pc/). Tool for World of Tanks players helping to learn and understand game mechanics. User interface made with RmlUi.

<img width="960" height="640" alt="ai_4 0 0_960x640_2025-11-24_16-18-18" src="https://github.com/user-attachments/assets/69644844-2f9b-4440-8b9f-bd8317ef3064" />

<img width="960" height="640" alt="ai_4 0 0_960x640_2018-06-24_05-52-44 (1)" src="https://github.com/user-attachments/assets/2c4a2160-5c21-4389-8c55-6d64bc46c7c9" />

### ROSE Online

[ROSE Online](https://www.roseonlinegame.com/). A free-to-play 3D fantasy MMORPG. User interface made with RmlUi.

Character selection.

<img width="1172" height="1003" alt="2025-12-18-13-20-56-capture" src="https://github.com/user-attachments/assets/413b6a00-a26e-4f59-848f-618376bc09da" /><br/>

World map with custom map rendering and interaction.

https://github.com/user-attachments/assets/a8ce9ece-8b63-45dd-84ba-df9c4187ad88

Crafting with animations and drag & drop.

https://github.com/user-attachments/assets/c336e19b-3448-4d92-ad4a-72cf7ec7185c

### Killing Time: Resurrected 

[Killing Time: Resurrected](https://nightdivestudios.com/killing-time-resurrected/) by [Nightdive Studios](https://www.nightdivestudios.com/). Remastered version of the classic shooter game. User interface made with RmlUi.

![Killing Time: Resurrected collage](https://raw.githubusercontent.com/mikke89/RmlUiDoc/7ca874cc506986a789e2c9a317a4e23f359d2316/assets/gallery/killing_time_resurrected_collage.webp)

### Unvanquished 

[Unvanquished](https://unvanquished.net/). A first-person shooter game with real-time strategy elements. Menus and HUD in RmlUi.

![Unvanquished 0.54 collage](https://user-images.githubusercontent.com/5490330/230487771-5108a273-8b76-4216-8324-d9e5af102622.jpg)

### vkQuake + RmlUi 

[vkQuake + RmlUi](https://github.com/bradenleague/vkQuake-RmlUi). A vkQuake fork that replaces Quake's menu and HUD systems with RmlUi. \[[teaser trailer](https://youtu.be/vEAeOuetTQ0)\]

<img width="2247" height="1228" alt="vkQuake + RmlUi" src="https://github.com/user-attachments/assets/0ef7f033-6852-490b-9fe4-14100995b6a7" />

### alt:V installer

[alt:V](https://altv.mp/) installer. A multiplayer client for GTA V.

![alt:V installer collage](https://user-images.githubusercontent.com/5490330/230487770-275fe98f-753f-4b35-b2e1-1e20a798f5e8.png)

### TruckersMP

[TruckersMP](https://truckersmp.com/). A multiplayer mod for truck simulators. Chat box and player panel in RmlUi.

![TruckersMP chat box](https://raw.githubusercontent.com/mikke89/RmlUiDoc/8ce505124daec1a9fdff0327be495fc2e43a37cf/assets/gallery/truckers_mp.webp)

![TruckersMP player panel](https://raw.githubusercontent.com/mikke89/RmlUiDoc/3e37fdcd85b72e20ee15b4f30033ca580866b48d/assets/gallery/truckers_mp_player_list.webp)

### Installer software by [@xland](https://github.com/xland)

![xland installer collage](https://user-images.githubusercontent.com/5490330/230487763-ec4d28e7-7ec6-44af-89f2-d2cbad8f44c1.png)

### RmlUi 'data_binding' sample

Simple game demonstrating the use of data bindings.

![Data binding sample](https://raw.githubusercontent.com/mikke89/RmlUiDoc/df1651db94e69f2977bc0344864ec061b56b104e/assets/gallery/data_binding.png)

### RmlUi 'effects' sample

Applying advanced filters.

![Effects sample](https://github.com/mikke89/RmlUiDoc/blob/master/assets/images/effects-sample-filters.png?raw=true)

[Effects sample video](https://github.com/mikke89/RmlUi/assets/5490330/bdc0422d-867d-4090-9d48-e7159e3adc18)

### RmlUi collage of advanced effects

![Collage of advanced effects](https://github.com/user-attachments/assets/71840d6f-903e-45fe-9e34-a02ed1ddae07)

### RmlUi 'demo' sample

Form controls from the 'demo' sample.

![Form controls](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/forms.png?raw=true)

Sandbox from the 'demo' sample, try it yourself!

![Sandbox](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/sandbox.png?raw=true)

Transitions on mouse hover (entirely in RCSS).

![Transition](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/transition.gif)

Animated transforms (entirely in RCSS).

![Transform](https://github.com/mikke89/RmlUiDoc/blob/3f319d8464e73b821179ff8d20537013af5b9810/assets/gallery/transform.gif)

### RmlUi flexbox layout

![Flexbox](https://github.com/mikke89/RmlUiDoc/blob/4cf0c6ac23b822174e69e5f1413b71254230c619/assets/images/flexbox-example.png?raw=true)

### RmlUi visual testing framework

For built-in automated layout tests.

![Visual testing framework](https://github.com/mikke89/RmlUiDoc/blob/c7253748d1bcf6dd33d97ab4fe8b6731a7ee3dac/assets/gallery/visual_tests_flex.png?raw=true)

### RmlUi 'animation' sample

[Animation sample](https://user-images.githubusercontent.com/5490330/230486839-de3ca062-6641-48e0-aa6a-ef2b26c3aad5.webm)

### Transitions and transforms on a game menu

[Game main menu](https://user-images.githubusercontent.com/5490330/230487193-cd07b565-2e9b-4570-aa37-7dd7746dd9c9.webm)

### Camera movement in a game menu

[Transforms applied to game menu](https://user-images.githubusercontent.com/5490330/230487217-f499dfca-5304-4b99-896d-07791926da2b.webm)

### RmlUi 'lottie' sample

Vector animations with the [Lottie plugin](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/lottie.html).

![Lottie animation](https://github.com/mikke89/RmlUiDoc/blob/086385e119f0fc6e196229b785e91ee0252fe4b4/assets/gallery/lottie.gif)

### RmlUi 'svg' sample

Vector images with the [SVG plugin](https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/svg.html).

![SVG image](https://github.com/mikke89/RmlUiDoc/blob/2908fe50acf7861e729ce113eafa8cf7610bf08a/assets/gallery/svg_plugin.png)

### See more

Additional screenshots from thirdparty projects can be found in:

- [Gallery discussion thread](https://github.com/mikke89/RmlUi/discussions/184).
- [Thirdparty projects discussion thread](https://github.com/mikke89/RmlUi/discussions/186) and links therein.


## License

### RmlUi license

RmlUi is published under the ***MIT license***, see [LICENSE.txt](LICENSE.txt).

### Third-party licenses

The library includes third-party source code and assets with their own licenses, as detailed below.

#### Third-party source code included in RmlUi Core

See [Include/RmlUi/Core/Containers/LICENSE.txt](Include/RmlUi/Core/Containers/LICENSE.txt) - all MIT licensed.

#### Third-party font assets included in RmlUi Debugger

See [Source/Debugger/LICENSE.txt](Source/Debugger/LICENSE.txt) - SIL Open Font License.

#### Additional sample assets *(in Samples/)*

See
- [Samples/assets/LICENSE.txt](Samples/assets/LICENSE.txt)
- [Samples/basic/bitmap_font/data/LICENSE.txt](Samples/basic/bitmap_font/data/LICENSE.txt)
- [Samples/basic/harfbuzz/data/LICENSE.txt](Samples/basic/harfbuzz/data/LICENSE.txt)
- [Samples/basic/lottie/data/LICENSE.txt](Samples/basic/lottie/data/LICENSE.txt)
- [Samples/basic/svg/data/LICENSE.txt](Samples/basic/svg/data/LICENSE.txt)

#### Library included with the Vulkan backend *(in Backends/RmlUi_Vulkan/)*

See [Backends/RmlUi_Vulkan/LICENSE.txt](Backends/RmlUi_Vulkan/LICENSE.txt) - MIT license.

#### Library included with the DirectX 12 backend *(in Backends/RmlUi_DirectX/)*

See [Backends/RmlUi_DirectX/LICENSE.txt](Backends/RmlUi_DirectX/LICENSE.txt) - MIT license.

#### Library included with the SDL GPU backend *(in Backends/RmlUi_SDL_GPU/)*

See [Backends/RmlUi_SDL_GPU/SDL_shadercross/LICENSE.txt](Backends/RmlUi_SDL_GPU/SDL_shadercross/LICENSE.txt) - Zlib license.

#### Libraries included with the test suite *(in Tests/Dependencies/)*

See [Tests/Dependencies/LICENSE.txt](Tests/Dependencies/LICENSE.txt).

#### Additional test suite assets *(in Tests/Data/VisualTests/)*

See [Tests/Data/VisualTests/LICENSE.txt](Tests/Data/VisualTests/LICENSE.txt).
