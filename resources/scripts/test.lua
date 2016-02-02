local M = {}

M.building = nil

local function foo()
	local game = getGame()
	game:setName("Testing 123!")
	game:setSize(1024, 600)
	game:setMaxFramesPerSecond(60)
	game:reCenterMainWindow()
	
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	local camera = entityManager:getGameCamera()
	camera:setFieldOfView(90)
	
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
	
	Utils.logprint("Hello, world from lua!!!!")
	
	camera:setDirection(Vec4(3, 0.0, 0.0, 1.0))
	
	local geometry = resourceManager:findObjectGeometryGroup("suzanne"):getObjectGeometries()[1]
	local shader = resourceManager:findShader("shaded")
	local texture = resourceManager:findTexture("suzanne")
	local monkey = ShadedObject(geometry, shader, texture, false, PhysicsBodyType.Dynamic)
	local monkey2 = ShadedObject(geometry, shader, texture, false, PhysicsBodyType.Dynamic)
	M.building = ShadedObject(resourceManager:findObjectGeometryGroup("building"):getObjectGeometries()[1], shader, resourceManager:findTexture("building"), false, PhysicsBodyType.Dynamic)
	
	local velocity = Vec3(0.8, 0.0, 0.0)
	monkey:getPhysicsBody():setVelocity(velocity)
	--entityManager:addObject(monkey)
	monkey:getPhysicsBody():setRestitution(0.5)
	
	monkey2:getPhysicsBody():setPosition(Vec3(1, 0.0, 0.0))
	monkey2:getPhysicsBody():setRestitution(0.9)
	monkey2:getPhysicsBody():setVelocity(Vec3(0, 0.0, 0.0))
	--entityManager:addObject(monkey2)
	
	local light = Light(Vec3(4, 4, 4), Vec3(1, 1, 1), Vec3(1, 1, 1), 60)
	entityManager:addLight(light)
	
	local maxCoord = 0
	for i=0, 20, 1 do
		local coord = i + 0.5
		local newMonkey = ShadedObject(geometry, shader, texture, false, PhysicsBodyType.Dynamic)
		newMonkey:getPhysicsBody():setPosition(Vec3(coord, 0.0, 0.0))
		entityManager:addObject(newMonkey)
		
		maxCoord = coord
	end
	
	M.building:getPhysicsBody():setPosition(Vec3(maxCoord + 5, 0.0, 0.0))
	M.building:getPhysicsBody():setRestitution(0.5)
	entityManager:addObject(M.building)
	M.building:getPhysicsBody():calculateShapesUsingObjectGeometry(false, Vec3(3, 3, 3))
	M.building:getPhysicsBody():setFixtedRotation(true)
end

M.foo = foo

return M