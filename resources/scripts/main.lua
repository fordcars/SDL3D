local test = require("test")

local game = getGame()

local resourceManager = game:getResourceManager()
local inputManager = game:getInputManager()
local entityManager = game:getEntityManager()

function gameInit()
	test.foo()
	Utils.logprint("Hello, init from lua!")
end

function gameStep()
	local game = getGame()
	
	local camera = entityManager:getGameCamera()
	
	local shader = resourceManager:findShader("basic");
	local objects = entityManager:getObjects()
	
	doControls();
	
	for i,v in ipairs(objects) do
		local physicsBody = v:getPhysicsBody()
		physicsBody:renderDebugShape(shader, camera)
	end
	
	--[[Utils.logprint("")
	Utils.logprint("Position:")
	Utils.logprint("x: " .. physicsBody:getPosition().x)
	Utils.logprint("y: " .. physicsBody:getPosition().y)
	Utils.logprint("z: " .. physicsBody:getPosition().z)
	Utils.logprint("Velocity:")
	Utils.logprint("x: " .. physicsBody:getVelocity().x)
	Utils.logprint("y: " .. physicsBody:getVelocity().y)
	Utils.logprint("z: " .. physicsBody:getVelocity().z)]]--
	
	--test.building:getPhysicsBody():setVelocity(Vec3(-3.0, 0.0, 0.0))
	
	local buildingPosition = test.building:getPhysicsBody():getPosition()
	test.building:getPhysicsBody():setPosition(Vec3(buildingPosition.x, 0, 0))
	
	local rotation = test.firstMonkey:getPhysicsBody():getRotation()
		Utils.logprint("Last monkey angle: " .. rotation.y)
	test.firstMonkey:getPhysicsBody():setRotation(Vec3(rotation.x + 0.1, rotation.y, rotation.z))
end

function doControls()
	local speed = 0.01
	
	local camera = entityManager:getGameCamera()
	local cameraPhysicsBody = camera:getPhysicsBody()
	local cameraDirection = Vec3.fromVec4(camera:getDirection()) -- Explicit conversion to vec3
	
	cameraPhysicsBody:renderDebugShapeWithCoord(resourceManager:findShader("basic"), camera, 0.0)

	if(inputManager:isKeyPressed(KeyCode.LSHIFT)) then
		speed = speed * 50
	end
	
	if(inputManager:isKeyPressed(KeyCode.UP)) then
		local velocity = Vec3.scalarMul(Vec3(cameraDirection.x, 0, cameraDirection.z), speed)
		camera:getPhysicsBody():setVelocity(velocity)
	end
end