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
	print("\n[Test1.lua] with ptr ID [" .. ptr.id .. "]")
	ptr:Print("Hello!!")

	x, y, z = ptr:GetPoint(2, 3)
	print("x: " .. x .. ", y: " .. y .. ", z: " .. z)

	x, y, z = ptr:GetPoint2(2, 3)
	print("x: " .. x .. ", y: " .. y .. ", z: " .. z)
end