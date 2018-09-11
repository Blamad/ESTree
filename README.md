# ESTree
Simple game engine capable of rendering scene, performing physics simulations and generating tree models using Lindenmayer sytem formal grammar.
Created as masters degree project.

ESTree is using few libraries:
 * __BOOST__: general perfomance purposes
 * __GLFW, GLM, GLEW, OpenGL__: scene rendering
 * __BulletPhysics__: physics simulation (collisions, mouse picking, collapsing towers made of blocks) 
 * __RapidJSON__: parsing configuration files
 * __FreeType__: text rendering

Sample screenshot presenting current engine abilities regarding tree generation:

 * Simple bush model: <br/>
  ![Sample screenshot](/images/withHdr.PNG) 
 * Ternary tree models generated from rules described in "The Algorithmic Beauty of Plants":<br/>
  ![Sample screenshot](/images/ternaryTreeA.PNG)
  ![Sample screenshot](/images/ternaryTreeE.PNG)
   
 Usage:<br/>
 Scene navigation: RMB + WSAD (Space and left alt to move vertically)<br/>
 Object selection/pick up: LMB<br/>
 Console toggle: ~<br/>
 
 Application has number of commands available in built-in user console such as:
 * adding and removing objects from scene,
 * creating movable light sources,
 * generating Lindenmayer trees from configuration files,
 * exporting models to .obj files,
 * help listing all actual commands.