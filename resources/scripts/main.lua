local test = require("test")

function gameInit()
	test.foo()
	Utils.logprint("Hello, init from lua!")
end

function gameStep()
Utils.logprint("Hello, STEPPP from lua!")
	local game = getGame()
	local resourceManager = game:getResourceManager()
	local inputManager = game:getInputManager()
	local entityManager = game:getEntityManager()
	
	local camera = entityManager:getGameCamera()
	
	local shader = resourceManager:findShader("basic");
	local objects = entityManager:getObjects()
	for i,v in ipairs(objects) do
		local physicsBody = v:getPhysicsBody()
		physicsBody:renderDebugShape(shader, camera, 0.0)
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
end