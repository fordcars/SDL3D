local M = {}

local function foo()
	local game = getGame()
	game:setName("Testing 123!")
	game:setSize(1024, 600)
	game:reCenterMainWindow()
	
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	resourceManager:addShader("basic.v.glsl", "basic.f.glsl")
	resourceManager:addShader("textured.v.glsl", "textured.f.glsl")
	resourceManager:addShader("shaded.v.glsl", "shaded.f.glsl")
	
	resourceManager:addTexture("test.bmp", TextureType.BMP_TEXTURE)
	resourceManager:addTexture("suzanne.dds", TextureType.DDS_TEXTURE)
	resourceManager:addTexture("building.dds", TextureType.DDS_TEXTURE)
	resourceManager:addTexture("minecraft.dds", TextureType.DDS_TEXTURE)
	
	resourceManager:addObjectGeometryGroup("suzanne.obj");
	resourceManager:addObjectGeometryGroup("building.obj");
	
	inputManager:registerKeys({KeyCode.SDLK_UP, KeyCode.SDLK_DOWN, KeyCode.SDLK_LEFT, KeyCode.SDLK_RIGHT,
		KeyCode.SDLK_w, KeyCode.SDLK_a, KeyCode.SDLK_s, KeyCode.SDLK_d, KeyCode.SDLK_SPACE, KeyCode.SDLK_LSHIFT, KeyCode.SDLK_LCTRL})
	
	
end

M.foo = foo

return M