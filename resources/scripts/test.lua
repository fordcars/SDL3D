local M = {}

local function foo()
	local game = getGame()
	local entityManager = game:getEntityManager()
	
	local objects = entityManager:getObjects()
	objects[1]:TEST()
end

M.foo = foo

return M