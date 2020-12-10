local M = {}

M.building = nil
M.lastMonkey = nil

local function foo()
	local game = getGame()
	game:setName("Testing 123!")
	game:setSize(IVec2(1024, 600))
	game:setMaxFramesPerSecond(60)
	game:reCenterMainWindow()
	
	game:setGraphicsBackgroundColor(Vec3(0, 0, 1))
	
	Utils.logprint("Engine version: " .. Engine.Version)
	
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	local camera = entityManager:getGameCamera()
	camera:setFieldOfView(90)
	camera:getPhysicsBody():calculateShapesFromRadius(0.1)
	
	resourceManager:addShader("basic.v.glsl", "basic.f.glsl")
	resourceManager:addShader("textured.v.glsl", "textured.f.glsl")
	resourceManager:addShader("shaded.v.glsl", "shaded.f.glsl")
	
	resourceManager:addTexture("test.bmp", TextureType.BMP)
	resourceManager:addTexture("suzanne.dds", TextureType.DDS)
	resourceManager:addTexture("building.dds", TextureType.DDS)
	
	resourceManager:addObjectGeometryGroup("suzanne.obj");
	resourceManager:addObjectGeometryGroup("building.obj");
	
	resourceManager:addNamedSound("music", "texasradiofish_-_Funk_n_Jazz.ogg", SoundType.Music)
	resourceManager:addSound("soundEffect.ogg", SoundType.Chunk)
	
	resourceManager:findSound("music"):fadeIn(10, -1)
	
	inputManager:registerKeys({KeyCode.UP, KeyCode.DOWN, KeyCode.LEFT, KeyCode.RIGHT, KeyCode.LSHIFT, KeyCode.RSHIFT, KeyCode.LCTRL, KeyCode.RCTRL, KeyCode.SPACE, KeyCode.BACKSPACE, KeyCode.a, KeyCode.b, KeyCode.c, KeyCode.d, KeyCode.e, KeyCode.f, KeyCode.g, KeyCode.h, KeyCode.i, KeyCode.j, KeyCode.k, KeyCode.l, KeyCode.m, KeyCode.n, KeyCode.o, KeyCode.p, KeyCode.q, KeyCode.r, KeyCode.s, KeyCode.t, KeyCode.u, KeyCode.v, KeyCode.w, KeyCode.x, KeyCode.y, KeyCode.z})
	
	camera:setDirection(Vec4(3, 0.0, 0.0, 0.0))
	
	local geometry = resourceManager:findObjectGeometryGroup("suzanne"):getObjectGeometries()[1]
	local shader = resourceManager:findShader("shaded")
	local texture = resourceManager:findTexture("suzanne")
	M.building = ShadedObject(resourceManager:findObjectGeometryGroup("building"):getObjectGeometries()[1], shader, resourceManager:findTexture("building"), false, PhysicsBodyType.Dynamic)
	
	local light = Light(Vec3(4, 4, 4), Vec3(1, 1, 1), Vec3(1, 1, 1), 60)
	entityManager:addLight(light)
	
	local maxCoord = 0
	local firstMonkey = nil
	for i=0, 20, 1 do
		local coord = i + 0.5
		local newMonkey = ShadedObject(geometry, shader, texture, false, PhysicsBodyType.Dynamic)
		newMonkey:getPhysicsBody():setPosition(Vec3(coord, 0.0, 0.0))
		newMonkey:getPhysicsBody():setVelocity(Vec3(0, 0.0, 0.0))
		newMonkey:getPhysicsBody():setWorldFriction(2)
		entityManager:addObject(newMonkey)
		
		maxCoord = coord
		
		if(firstMonkey == nil) then
			firstMonkey = newMonkey
		end
	end
	
	M.building:getPhysicsBody():setPosition(Vec3(maxCoord + 5, 0.0, 0.0))
	M.building:getPhysicsBody():setRestitution(0.5)
	entityManager:addObject(M.building)
	M.building:getPhysicsBody():calculateShapesUsingObjectGeometry(false, Vec3(3, 3, 3))
	M.building:getPhysicsBody():setFixtedRotation(true)
	M.building:getPhysicsBody():setVelocity(Vec3(0, 0, 1))
	
	M.firstMonkey = firstMonkey
end

M.foo = foo

return M
