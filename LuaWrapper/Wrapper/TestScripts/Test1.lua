-- Test1.lua --

function HelloWorld()
	print("\nHello World! From [Test1.lua]");
	Testing(10);
end

--function Update(ptr, luaObject)
	--ptr:CPrint("Hello!!", luaObject, "Print");
--end

function Update(ptr)
	print("\nCalled [Update] From [Test1.lua]");
	ptr:Print("Hello!!");
end