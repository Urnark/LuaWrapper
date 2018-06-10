-- Test1.lua --

function HelloWorld()
	print("\nHello World! From [Test1.lua]")
	Testing(10)
	x, y, z = GetPoint(2, 3)
	print("x: " .. x .. ", y: " .. y .. ", z: " .. z)
	Testing3()
end

function Update(ptr)
	print("\nCalled [Update] From [Test1.lua]")
	ptr:Print("Hello!!")
	print("ID: " .. ptr.id)
end