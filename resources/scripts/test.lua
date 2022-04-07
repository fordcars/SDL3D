local M = {}

M.building = nil
M.lastMonkey = nil

local function foo()
	local game = getGame()
	game:setName("Testing 123!")
	game:setSize(IVec2(1024, 600))
	game:setMaxFramesPerSecond(60)
	game:reCenterMainWindow()
	
	game:setGraphicsBackgroundColor(Vec3(0, 0, 0))
	
	Utils.logprint("Engine version: " .. Engine.Version)
	
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	local camera = entityManager:getGameCamera()
	camera:setFieldOfView(90)
	camera:getPhysicsBody():calculateShapesFromRadius(0.1)
	
	resourceManager:addShader("basic.v.glsl", "basic.f.glsl")
	resourceManager:addShader("textured.v.glsl", "textured.f.glsl")
    resourceManager:addShader("deferredLight.v.glsl", "deferredLight.f.glsl")
    resourceManager:addShader("deferredShaded.v.glsl", "deferredShaded.f.glsl")
	
	resourceManager:addTexture("test.bmp", TextureType.BMP)
	resourceManager:addTexture("suzanne.dds", TextureType.DDS)
	resourceManager:addTexture("building.dds", TextureType.DDS)
	
	resourceManager:addObjectGeometryGroup("suzanne.obj");
	resourceManager:addObjectGeometryGroup("building.obj");
	
	resourceManager:addNamedSound("music", "texasradiofish_-_Funk_n_Jazz.ogg", SoundType.Music)
	resourceManager:addSound("soundEffect.ogg", SoundType.Chunk)
	
	resourceManager:findSound("music"):fadeIn(10, -1)
	
	inputManager:registerKeys({KeyCode.UP, KeyCode.DOWN, KeyCode.LEFT, KeyCode.RIGHT, 
	KeyCode.LSHIFT, KeyCode.RSHIFT, KeyCode.LCTRL, KeyCode.RCTRL, KeyCode.SPACE, 
	KeyCode.BACKSPACE, KeyCode.TAB, KeyCode.RETURN, KeyCode.ESCAPE, KeyCode.EXCLAIM, 
	KeyCode.QUOTEDBL, KeyCode.HASH, KeyCode.DOLLAR, KeyCode.AMPERSAND, 
	KeyCode.QUOTE, KeyCode.LEFTPAREN, KeyCode.RIGHTPAREN, KeyCode.ASTERISK, 
	KeyCode.PLUS, KeyCode.COMMA, KeyCode.MINUS, KeyCode.PERIOD, KeyCode.FSLASH, 
	KeyCode.BSLASH, 
	KeyCode.a, KeyCode.b, KeyCode.c, KeyCode.d, KeyCode.e, KeyCode.f, KeyCode.g, 
	KeyCode.h, KeyCode.i, KeyCode.j, KeyCode.k, KeyCode.l, KeyCode.m, KeyCode.n, 
	KeyCode.o, KeyCode.p, KeyCode.q, KeyCode.r, KeyCode.s, KeyCode.t, KeyCode.u, 
	KeyCode.v, KeyCode.w, KeyCode.x, KeyCode.y, KeyCode.z, KeyCode.N0, KeyCode.N1, 
	KeyCode.N2, KeyCode.N3, KeyCode.N4, KeyCode.N5, KeyCode.N6, KeyCode.N7, KeyCode.N8, 
	KeyCode.N9, KeyCode.KP0, KeyCode.KP1, KeyCode.KP2, KeyCode.KP3, KeyCode.KP4, 
	KeyCode.KP5, KeyCode.KP6, KeyCode.KP7, KeyCode.KP8, KeyCode.KP9, KeyCode.F1, 
	KeyCode.F2, KeyCode.F3, KeyCode.F4, KeyCode.F5, KeyCode.F6, KeyCode.F7, 
	KeyCode.F8, KeyCode.F9, KeyCode.F10, KeyCode.F11, KeyCode.F12})
	
	camera:setDirection(Vec4(3, 0.0, 0.0, 0.0))
	
	local geometry = resourceManager:findObjectGeometryGroup("suzanne"):getObjectGeometries()[1]
	local shader = resourceManager:findShader("deferredShaded")
	local texture = resourceManager:findTexture("suzanne")
	M.building = ShadedObject(resourceManager:findObjectGeometryGroup("building"):getObjectGeometries()[1], shader, resourceManager:findTexture("building"), false, PhysicsBodyType.Dynamic)
	
	local lightShader = resourceManager:findShader("deferredLight")
    
	math.randomseed(os.time())
	local stride = 1
    
	--[[
	for i = 0, 20, 1 do
		for j = 0, 20, 1 do
			local color = Vec3(math.random(), math.random(), math.random())
			local light = Light(lightShader, Vec3(i * stride, 4, j * stride), color, color, 60)
			entityManager:addLight(light)
		end
	end]]--
	
	local color = Vec3(1, 1, 0.5)
	local light = Light(lightShader, Vec3(4, 4, 4), color, color, 60)
	entityManager:addLight(light)
	
	color = Vec3(1, 0.5, 1)
	light = Light(lightShader, Vec3(4.5, 4, 4), color, color, 60)
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
