
#### This project is not done. The code might change
# LuaWrapper
My own c++ wrapper for Lua. Do not expect great things from it

# Documentation
### Using the LuaWrapper
To use the LuaWrapper it is needed to initialize the wrapper. You can do that by calling the function "InitLuaManager".
```c++
LuaManager::InitLuaManager(lua state name);
```
lua state name = name of the lua state. If not used then it is named "Init"

### Register a C++ function

```c++
LuaFunctionsWrapper::RegisterCFunction(functionName, classInstance, function)
```
- functionName = the name that the function has in Lua
- classInstance = the class that the function is a member function of
- function = the function that shall be registered

```c++
void foo() {
  std::cout << "Called function foo" << std::endl;
}

// If a function in a class shall be used in Lua then it need to inherit from ILuaMember
class Bar : public ILuaMember
{
public:
  bool IsPositive(int i) {
    return (i < 0? false: true);
  }
}

using namespace LFW;

void main() {
  // Init lua
  LuaManager::InitLuaManager();

  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");

  // Register a global function so it can be used in a Lua script
  LuaFunctionsWrapper::RegisterCFunction("foo", &foo);
  
  // Register a member function so it can be used in a Lua script
  Bar bar;
  LuaFunctionsWrapper::RegisterCFunction("IsPositive", &bar, &Bar::IsPositive);
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```

In the Lua script
```Lua
-- LuaTestScript1.lua --

foo()

positiveNr = IsPositive(34)
print(positiveNr)

```
