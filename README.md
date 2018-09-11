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

 * Simple bush model:
  ![Sample screenshot](/images/withHdr.PNG)
 
 * Sympodial tree model generated from rules described in "The Algorithmic Beauty of Plants":
  ![Sample screenshot](/images/sympodialTree3_18_01_2018.PNG)
  
 * Ternary tree models generated from rules described in "The Algorithmic Beauty of Plants":
  ![Sample screenshot](/images/ternaryTreeA.PNG)
  ![Sample screenshot](/images/ternaryTreeB.PNG)
  ![Sample screenshot](/images/ternaryTreeE.PNG)
  
 * Meterial works sample:
   ![Sample screenshot](/images/materialsSample.PNG)
   
 Usage:
 Scene navigation: RMB + WSAD (Space and left alt to move vertically)
 Object selection/pick up: LMB
 Console toggle: ~
 
 Application has number of commands available in built-in user console:
|Command|Parameter|Description|Expected value|
|---|---|---|:---:|
|help|–|Displays available commands|–|
|depth|–|Controlls visibility of depth preview component rendered from directional lights perspective|on/off|
|hdr|–|Enables/disables HDR feature|on/off|
|normals|–|Enables/disables normals view on selected mesh|on/off|
|wiring|-|Enables/disables mesh wiring on selected mesh|on/off|
|clr|–|Clears user console|–|
|rm|–|Removes selected object from scene|–|
|mv|–|Moves selected object by given vector|vec3 (x,y,z)|
|tree||Creates tree object||
||pos|Tree position|vec3 (x,y,z)|
||file|Configuration filename located in LindenmayerRules directory|string|
||name|Object name|string|
||bark|Bark texture filename located in Textures directory|string|
||leaves|Leaves texture filename located in Textures directory|string|
|cube||Creates cube object||
||pos|Tree position|vec3 (x,y,z)|
||name|Object name|string|
|export|file|Exports selected model to .obj file in models directory|string|
|rtree||Re-executes selected tree generation||
||pos|Tree position|vec3 (x,y,z)|
||file|Configuration filename located in LindenmayerRules directory|string|
||name|Object name|string|
||bark|Bark texture filename located in Textures directory|string|
||leaves|Leaves texture filename located in Textures directory|string|
|light|Creates cube being movable lightsource||
||pos|Cube position|vec3 (x,y,z)|
||name|Object name|string|
||strength|Light strenght|1/2/3|
