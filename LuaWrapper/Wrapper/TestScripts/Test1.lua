-- Test1.lua --

function HelloWorld()
	print("\nHello World! From [Test1.lua]");
	Testing(10);
end

function Update(ptr, luaObject)
	ptr:CPrint("Hello!!", luaObject, "Print");
end