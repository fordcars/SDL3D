local test = require("test")

local game = getGame()

local resourceManager = game:getResourceManager()
local inputManager = game:getInputManager()
local entityManager = game:getEntityManager()

local musicButtonPressedLastFrame = false

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
	
	local buildingPosition = test.building:getPhysicsBody():getPosition()
	test.building:getPhysicsBody():setPosition(Vec3(buildingPosition.x, 0, 0))
	
	local rotation = test.firstMonkey:getPhysicsBody():getRotation()
	test.firstMonkey:getPhysicsBody():setRotation(Vec3(rotation.x + 0.1, rotation.y, rotation.z))
end

function doControls()
	local speed = 0.05
	local angleIncrementation = 0.02
	
	local camera = entityManager:getGameCamera()
	local cameraPhysicsBody = camera:getPhysicsBody()
	
	cameraPhysicsBody:renderDebugShapeWithCoord(resourceManager:findShader("basic"), camera, 0.0)

	-- Movement
	if(inputManager:isKeyPressed(KeyCode.LSHIFT)) then
		speed = speed * 50
	end
	
	if(inputManager:isKeyPressed(KeyCode.UP)) then
		local cameraDirection = camera:getDirection()
		
		local velocity = Vec3.scalarMul(Vec3(cameraDirection.x, 0, cameraDirection.z), speed)
		cameraPhysicsBody:setVelocity(velocity)
	elseif(inputManager:isKeyPressed(KeyCode.DOWN)) then
		local cameraDirection = camera:getDirection()
	
		local velocity = Vec3.scalarMul(Vec3(-cameraDirection.x, 0, -cameraDirection.z), speed)
		cameraPhysicsBody:setVelocity(velocity)
	end
	
	if(inputManager:isKeyPressed(KeyCode.LEFT)) then
		local cameraDirection = camera:getDirection()
		local angle = 1.5708 -- 90 degrees in radians
		
		local otherX = cameraDirection.x * math.cos(-angle) - cameraDirection.z * math.sin(-angle)
		local otherY = cameraDirection.x * math.sin(-angle) + cameraDirection.z * math.cos(-angle)
		
		local currentVelocity = cameraPhysicsBody:getVelocity()
		
		--cameraPhysicsBody:setVelocity(currentVelocity.x +
	elseif(inputManager:isKeyPressed(KeyCode.RIGHT)) then
		local cameraDirection = camera:getDirection()
	
		local velocity = Vec3.scalarMul(Vec3(-cameraDirection.x, 0, -cameraDirection.z), speed)
		cameraPhysicsBody:setVelocity(velocity)
	end
	
	-- View controls
	if(inputManager:isKeyPressed(KeyCode.w)) then
		local cameraDirection = camera:getDirection() -- Here we make sure we have the latest direction
		
		-- Base of the triangle
		local base = math.sqrt((cameraDirection.x * cameraDirection.x) + (cameraDirection.z * cameraDirection.z))
	
		-- http://stackoverflow.com/questions/22818531/how-to-rotate-2d-vector
		local newBase = base * math.cos(angleIncrementation) - cameraDirection.y * math.sin(angleIncrementation)
		local newY = base * math.sin(angleIncrementation) + cameraDirection.y * math.cos(angleIncrementation)
		
		local ratio = newBase / base
		
		camera:setDirection(Vec4(cameraDirection.x * ratio, newY, cameraDirection.z * ratio, 0)) -- 0 for vector
	elseif(inputManager:isKeyPressed(KeyCode.s)) then
		local cameraDirection = camera:getDirection()
		
		-- Base of the triangle
		local base = math.sqrt((cameraDirection.x * cameraDirection.x) + (cameraDirection.z * cameraDirection.z))
	
		local newBase = base * math.cos(-angleIncrementation) - cameraDirection.y * math.sin(-angleIncrementation)
		local newY = base * math.sin(-angleIncrementation) + cameraDirection.y * math.cos(-angleIncrementation)
		
		local ratio = newBase / base
		
		camera:setDirection(Vec4(cameraDirection.x * ratio, newY, cameraDirection.z * ratio, 0)) -- 0 for vector
	end
	
	if(inputManager:isKeyPressed(KeyCode.a)) then
		local cameraDirection = camera:getDirection()
		
		local newX = cameraDirection.x * math.cos(-angleIncrementation) - cameraDirection.z * math.sin(-angleIncrementation)
		local newZ = cameraDirection.x * math.sin(-angleIncrementation) + cameraDirection.z * math.cos(-angleIncrementation)
		
		camera:setDirection(Vec4(newX, cameraDirection.y, newZ, 0)) -- 0 for vector
	elseif(inputManager:isKeyPressed(KeyCode.d)) then
		local cameraDirection = Vec3.fromVec4(camera:getDirection())
		
		local newX = cameraDirection.x * math.cos(angleIncrementation) - cameraDirection.z * math.sin(angleIncrementation)
		local newZ = cameraDirection.x * math.sin(angleIncrementation) + cameraDirection.z * math.cos(angleIncrementation)
		
		camera:setDirection(Vec4(newX, cameraDirection.y, newZ, 0)) -- 0 for vector
	end
	
	-- Up/down controls
	if(inputManager:isKeyPressed(KeyCode.SPACE)) then
		cameraPhysicsBody:setPosition( Vec3.add(cameraPhysicsBody:getPosition(), Vec3(0, speed/3, 0)) )
	elseif(inputManager:isKeyPressed(KeyCode.LCTRL)) then
		cameraPhysicsBody:setPosition( Vec3.add(cameraPhysicsBody:getPosition(), Vec3(0, -speed/3, 0)) )
	end
	
	-- Music controls
	if(inputManager:isKeyPressed(KeyCode.m)) then
		if(musicButtonPressedLastFrame == false) then
			musicButtonPressedLastFrame = true
			
			resourceManager:findSound("soundEffect"):play()
			
			local music = resourceManager:findSound("music")
			if(music:isPaused()) then
				music:resume()
			else
				music:pause()
			end
		end
	else
		musicButtonPressedLastFrame = false
	end
end