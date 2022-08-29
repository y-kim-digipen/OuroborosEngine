# OuroborosEngine
ECS based render engine / GAM400

### Requirement

This program needs 'Vulkan SDK' in your driver. (https://vulkan.lunarg.com/sdk/home#windows)

### User Interface Description 

- Move Camera with Key W,A,S,D to move foward, left, right, backward
	* W : Move Foward
	* A : Move Left
	* S : Move Right
	* D : Move Backward
- Rotate Camera with Arrow Keys
	* G , J : Modify Yaw
	* Y , H : Modify Pitch

- Click File -> File browser on the Left top corner to load assets ( scene / texture / model )
	- Scene : \*.yaml
	- Texture : \*.png
	- Model : \*.obj
	- Add Entity on 'Entities', right click the blue bar of the entity to add specific system and add components with 'SystemInfo' window

- Every shader must include common file as header
- To use different uniform buffer variables, add it to set #1


### Implemented

- Script system
- Engine Component system
- Scene Save / Load

- PBR (physically based rendering, texture & non-texture)
- Runtime Shader read
- Multiple Lights ( Point / Spot / Direction )
- Asset Loader

### Machine Tested

- OS : Windows 10 x64
- GPU Vendor : ATI Technologies Inc
- GL Renderer : AMD Ryzen 7 4800HS with Radeon Graphics

