# ESTree
Simple game engine capable of rendering scene, performing physics simulations and generating tree models using Lindenmayer sytem formal grammar.
Created as masters degree final project.

ESTree is using few libraries:
 * __BOOST__: general perfomance purposes
 * __GLFW, GLEW, OpenGL__: scene rendering
 * __BulletPhysics__: physics simulation (collisions, mouse picking, collapsing towers made of blocks) 
 * __RapidJSON__: parsing configuration files

Sample screenshot presenting current engine abilities regarding tree generation:

 * Tree model descibed by Lindenmayer rules using few fibbonacci properties:
  ![Sample screenshot](/images/fibbonacciTree_18_01_2018.PNG)
 
 * Sympodial tree model generated from rules described in "The Algorithmic Beauty of Plants":
  ![Sample screenshot](/images/sympodialTree3_18_01_2018.PNG)
