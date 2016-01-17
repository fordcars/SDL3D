local test = require("test")

function gameInit()
	test.foo()
	Utils.logprint("Hello, init!")
end

function gameStep()
	Utils.logprint("Hello, step!")
end