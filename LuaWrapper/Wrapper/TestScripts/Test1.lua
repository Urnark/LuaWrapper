-- Test1.lua --

function HelloWorld()
	print("\nHello World! From [Test1.lua]")
	Testing(10)
	Testing3()

	ConstFunc();
	SFunc();
	SFunc2();
	
	local test = require("Wrapper.TestScripts.LibTest")
	print("From LibTest.lua: " .. test.testingAdd(1, 5))
end

function Update(ptr)
	print("\nCalled [Update] From [Test1.lua]")
	ptr:Print("Hello!!")
	x2, y2 = ptr:GetPoint2(2)
	print("x2: " .. x2 .. ", y2: " .. y2)
	x, y, z = ptr:GetPoint(2, 3)
	print("x: " .. x .. ", y: " .. y .. ", z: " .. z)
	print("ID: " .. ptr.id)
end