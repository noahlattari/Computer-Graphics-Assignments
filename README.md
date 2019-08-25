# :rocket: **Computer Graphics OpenGL Projects** :rocket:

These are a collection of my projects in my Computer Graphics class. Each assignment involves manipulating a 3D flying drone. 

To run assignment 3 type `gcc -o assignment3 -w -framework glut -framework opengl bundle.c`

![ThirdPerson](https://i.imgur.com/9JqMjYG.png)

When you first spawn in you can use the mouse to drag the camera to see better, it will
Be behind a building but just drag it slightly left or right to view the scene better.

-Texture mapped drone with all functionality from A1 including 2 cameras. A 
third person camera (f2), and a first person camera (f3) which has a zoom
and tilt function. (must be in f3 to tilt and zoom).

-Collision detection with ground and buildings. Note for buildings you have
to be pretty close to it, it's not extremely accurate and wont destroy the 
second a pixel touches the building, just fly straight into it and you
will see it.

-Texture mapped buildings and streets

-Second drone that moves in a simple pattern. Yeah I'm not sure why he moves at lightning
Speed but I'm not sure how to fix it : (

-Torpedo can be fired from drone. It may be hard to see because its fast but hit f3 to go
first person and make sure you're facing the blue sky, then hit space. Hold it down to
See it better.

-Help key. Just hit f1.

