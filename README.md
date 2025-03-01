# Ice2D
A framework for 2D graphics, sound, and basic keyboard/mouse input for windows, fully written in c++. The framework uses the DirectX family of libraries, and much of this is left exposed intentionally. The main purpose is to streamline the creation of a window and the DirectX setup, while also automatically releasing resources upon destruction. Only the Windows SDK is used, so there is no need to download any additional libraries to use the framework. This documentation is not exhaustive, but covers the basics. A lot of the actual drawing functions are just exposed Direct2D, so check its documentation for specifics.

## Setting up a project
See the `sample_game.cpp` file for a basic example. The `Ice2D.h` header includes all the necessary header files. Your main class should should inherit from Ice2D::Application. This class should override `Setup()`, `Update()`, and `Draw()`. `Setup()` is run once after the window opens, `Update()` and `Draw()` are run each frame, in that order. `Draw()` should return the HRESULT from EndDraw(), which is called on the render target. Make sure to call `BeginDraw()` before issuing any drawing commands to the render target, which can be obtained from the `GetRT()` method. To draw anything simply call the render target's methods, which are all documented in Direct2D.

## Resources
All the resources I implemented inherit from `Ice2D::IBasicResource`, which will be automatically released by the ResourceManager object passed into the constructor. One of the simplest resources, which I will use as an example, is the `Ice2D::SolidBrush`. To construct just create the object and pass the resource manager and a color into the constructor. The resource manager is just called manager in the Application class or you can create a seperate one for things like multiple render targets. Then to use it in a Direct2D method, call the `Get()` method to get the underlying resource pointer. The brush will be automatically released when it leaves scope or when the application closes. This means the brush can be a static object, but I don't recomment this approach unless necessary.

## Ice2D::Application
The contructor takes in the hInstance, width and height, a title, and some optional window style parameters. This class inherits from `Ice2D::Graphics`, which contains the windows and input stuff. It contains the main loop and also keeps track of the game time.

## Ice2D::ResourceManager
This contains a render target and all the necessary factories to create DirectX objects. It keeps track of all the resources made and releases them if necessary. All Ice2D applications need this class, as every resource takes a pointer to this object in its constructor.

## Images
The `Images.h` header contains 5 classes and 2 interfaces. `Ice2D::D2DImage` is can be drawn easily directly to a render target, but is difficult to modify. If you want to modifiy pixels, use the RawImage class, which can be copied to an `Ice2D::D2DImage` object for rendering. Call `Lock()` before using any of the methods for modifying pixels. Call `Unlock()` after you copy the RawImage to a `Ice2D::D2DImage` (using `CopyRaw()`), but it isn't strictly necessary. To render a `Ice2D::D2DImage`, use the render target's `DrawBitmap()` method. Both classes can load images from a file. The supported formats are BMP, GIF, ICO, JPEG, JPEG XR, PNG, TIFF, Windows Media Photo (outdated, use JPEG XR), and DDS.

## Animations
There are two animation classes in the `Images.h` header. `Ice2D::ImageSequence` is useful for when all the frames of the animation are seperate image files, since it takes an array of D2DImages. `Ice2D::AnimationSheet` is useful for a sheet style animation that needs to be sliced. They both inherit from `Ice2D::IBasicAnimation`, as they share similar functionality aside from rendering. `Start()` will not play the animation by itself, it just "enables" it. `Stop()` disables it. Call `Advance()`, which will swap the image to the correct frame based on when `Start()` was called. `PlayOnce()` automatically disables the animation after one cycle. Render as if it were a normal image with `Get()`.

## Brushes
Brushes are necessary for most drawing calls. `Ice2D::SolidBrush` will draw in a solid color. `Ice2D::BitmapBrush` will reveal parts of an image as it draws. `Ice2D::LinearBrush` and `Ice2D::RadialBrush` reveal a gradient as they draw, which can be defined easily with an `Ice2D::GradientStops` object. If you modify the `Ice2D::GradientStops` object after the first `Get()` call, call `Recreate()` to update.

## Ice2D::PathGeometry and Ice2D::Mesh
These are basically just wrappers of the Direct2D objects. Use `Ice2D::PathGeometry` to define a path, whether its a polygon or some curved shape. Use `Ice2D::Mesh` for efficient rendering of filled triangles, call `Close()` when done adding triangles. This is useful if you want to make a 3D renderer or something. Both can be passed into the render target directly with `Get()`. Draw with either `DrawGeometry()` or `FillMesh()`, respectively.

## Ice2D::TextFormat
Pretty simple resource. The fontName parameter should be the name of the font family, e.g., `L"Arial"` or `L"Impact"`. The framework doesn't have direct support for custom fonts, look at DirectWrite's documentation for how to import custom font resources, which should work fine with the rest of the framework.

## Sound
To play a sound, use the `Ice2D::Voice` and `Ice2D::Sound` classes. The `Ice2D::Sound` object represents the actual audio data, which can be loaded from a file. The `Ice2D::Voice` class is a single voice that audio data can be submitted to. Use `SubmitBuffer()` to add the audio data from a `Ice2D::Sound` object. Make sure the voice has the correct format passed in the constructor, use the `GetFormat()` from the Ice2D::Sound object to do this. For now, the framework only supports parsing .wav files. If you need to use a different format, or my parser doesn't work for some reason (it's worked for me so far, but your file is weird), you'll probably need to use a library to parse the file. The data can still be sent to an `Ice2D::Voice`, but you'll have to create the WAVEFORMATEX yourself, so check the XAudio2 documentation for this.

## Building
Include these dependencies:
```
d2d1.lib
dwrite.lib
XAudio2.lib
```
I used MSVC (ISO C++14 Standard), nothing special. Windows SDK version 10.0. Subsystem should be Windows. Use Unicode character set. This is intended to be built as a static library.
