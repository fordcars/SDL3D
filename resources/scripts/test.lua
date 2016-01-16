local M = {}

local function foo()
	local game = getGame()
	game:setName("Testing 123!")
	game:setSize(1024, 600)
	game:setMaxFramesPerSecond(60)
	game:reCenterMainWindow()
	
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	local camera = entityManager:getGameCamera():setFieldOfView(90)
	
	resourceManager:addShader("basic.v.glsl", "basic.f.glsl")
	resourceManager:addShader("textured.v.glsl", "textured.f.glsl")
	resourceManager:addShader("shaded.v.glsl", "shaded.f.glsl")
	
	resourceManager:addTexture("test.bmp", TextureType.BMP)
	resourceManager:addTexture("suzanne.dds", TextureType.DDS)
	resourceManager:addTexture("building.dds", TextureType.DDS)
	resourceManager:addTexture("minecraft.dds", TextureType.DDS)
	
	resourceManager:addObjectGeometryGroup("suzanne.obj");
	resourceManager:addObjectGeometryGroup("building.obj");
	--resourceManager:addObjectGeometryGroup("minecraft.obj");
	
	resourceManager:addNamedSound("music", "texasradiofish_-_Funk_n_Jazz.ogg", SoundType.Music)
	resourceManager:addSound("soundEffect.ogg", SoundType.Chunk)
	
	resourceManager:findSound("music"):fadeIn(10000, -1)
	--resourceManager:findSound("music"):setVolume(resourceManager:findSound("music"):getVolume()-120)
	
	inputManager:registerKeys({KeyCode.UP, KeyCode.DOWN, KeyCode.LEFT, KeyCode.RIGHT,
		KeyCode.w, KeyCode.a, KeyCode.s, KeyCode.d, KeyCode.SPACE, KeyCode.LSHIFT, KeyCode.LCTRL, KeyCode.m})
	
	Utils.logprint("Hello, world!!!!")
end

M.foo = foo

return M