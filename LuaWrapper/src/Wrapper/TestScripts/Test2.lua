-- Test2.lua --

function HelloWorld()
	print("\nHello World! From [Test2.lua]");
	Testing(10);
	print("Testing2 Ret: " .. (Testing2("return boolean") and "true" or "false"))
	return "hello"
end