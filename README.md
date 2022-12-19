# neocis_assignment

## Summary

The technologies used for this project include: Visual Studio 2019, C++, and the DirectX11 API for rendering the model in a desktop application for Windows. Although I leveraged the internet for guidance on DirectX11 and creating my own graphics engine, I personally developed features such as mouse input (drag to rotate the object) and file input (reading from a CSV file) to shape the object geometry. Additionally, I created a method to display the object in either solid or wireframe form, as well as various other features specific to this project. I have left comments throughout the source code to explain the methods I wrote.

Most of the graphics implementation is located in the [graphics engine](/source_code/GraphicsEngine.cpp).

The class to load and define an object's geometry in memory is located [here](/source_code/AppWindow.cpp) as the objectInfo class.

The methods that handle mouse input are found [here](/source_code/InputSystem.cpp).

The following project had been completed in the span of a weekend. It consists of everything from researching graphics rendering methods, to learning the necessary linear algebra techniques, to finally implementing a working 3D model of a shape whose geometry is defined by a CSV file.  


If I had more time to work on this project, I would add the following features:
- Fix the shading/rendering artifacts.
- Add a camera to so that I can use normals to calculate how much light the surface most visible to the viewer receives.
- More pronounced edges and vertices.
- A small 3D compass in the bottom corner to help orient the viewer (as found in any 3D CAD software).


## Example of code running

Here are two screenshots displaying both modes of operation in the software. The same file is loaded in for both images.

### Solid Pyramid
![here is an image](/images/solid_pyramid.JPG)

### Wireframe Pyramid
![here is the other image](/images/Wireframe_pyramid.JPG)

## How to run the code in this repository

1. Download the executable folder in its entirety (missing files may cause the code not to run properly).
2. Double click on the Neocis Software Assignment inside of [executable](/executable) application
3. You will see the text: 
> Enter '1' for wireframe or '0' for solid shape:
4. Type your desired response in the terminal and press enter
5. You will then see the text:
> Enter full filepath of the object file
6. Paste in the **FULL FILEPATH** (starting from C:)
7. The code should run!
8. You can click and drag on the new window to change the orientation of the object

*NOTE: The extension on the files are .txt"*

*NOTE 2: The example object files to load in are located [here](/executable/example_object_files)*

