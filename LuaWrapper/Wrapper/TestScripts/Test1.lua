-- Test1.lua --

function HelloWorld()
	print("\nHello World! From [Test1.lua]")
	Testing(10)
	x2, y2 = GetPoint2(2)
	print("x2: " .. x2 .. ", y2: " .. y2)
	Testing3()

	ConstFunc();
	SFunc();
	SFunc2();
end

function Update(ptr)
	print("\nCalled [Update] From [Test1.lua]")
	ptr:Print("Hello!!")
	x, y, z = GetPoint(ptr.id, 3)
	print("x: " .. x .. ", y: " .. y .. ", z: " .. z)
	print("ID: " .. ptr.id)
end