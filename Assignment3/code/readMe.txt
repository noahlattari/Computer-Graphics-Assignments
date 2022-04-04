Noah Lattari
Assignment 3:

If you're on a Mac, compile with gcc -o assignment3 -w -framework glut -framework opengl bundle.c

When you first spawn in you can use the mouse to drag the camera to see better, it will
Be behind a building but just drag it slightly left or right to view the scene better.

What works:
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

What doesn't work:
-Collision detection with drone. If you look at my code I kinda attempted it but had to
Stop because I have two exams Tuesday so I got to study for those.
-Vertex shader

