
# LuaWrapper
#### This project is not done. The code might change

### Table of Contents
  1. [Documentation](documentation)
     - [Start and close the LuaWrapper](init-luawrapper)
     - [Load a Lua script](load-lua-script)
     - [Create and use more Lua states](more-lua-states)
# [Documentation](#documentation)
### [Start and close the LuaWrapper](#init-luawrapper)
To use the LuaWrapper it is needed to initialize the wrapper. You can do that by calling the function "InitLuaManager".
```c++
void LuaManager::InitLuaManager(lua state name);
```
lua state name = name of the lua state. If not used then it is named "Init"

And when done to use the LuaWrapper it is needed to close it. You can do that by calling the function "CloseLuaManager"
```c++
void LuaManager::CloseLuaManager();
```

### [Load a Lua script](#load-lua-script)
To load a Lua script so it can be used in the program, the function "LoadScript" is used.
```c++
void LuaManager::LoadScript(lua state name, path of lua script);
```
- lua state name = the name of the Lua script that the script shall belong to
- path of lua script = the path to the Lua script

Thr first argument is only there to make the code smaller, more about using more Lua states in a program esists [here](more-lua-states).

### [Create and use more Lua states](#more-lua-states)
It is possible to create more lua states and have c++ functions that can only be called from a specific Lua state.
Three function exists to help with that:
```c++
void LuaManager::CreateLuaState(lua state name);
```
- lua state name = name of the new Lua state
```c++
void LuaManager::UseLuaState(lua state name);
```
- lua state name = the name of the Lua state that it shall switch to
```c++
void LuaManager::UseLuaState(lua state index);
```
- lua state index = the name of the Lua state that it shall switch to. Where the index is the order that the Lua state was added to the LuaWrapper. Lua state "Init" have the index "0".

The function that takes a index as parameter is faster than the other. So if you need to have a faster code then that is what you shall use.

An example to have two Lua states:
```c++
void foo() {
  std::cout << "Called function foo" << std::endl;
}
void bar() {
  std::cout << "Called function bar" << std::endl;
}

using namespace LFW;

void main() {
  // Init lua
  LuaManager::InitLuaManager();
  LuaManager::CreateLuaState("SecondLuaState");
  
  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");
  LuaManager::LoadScript("SecondLuaState", "LuaTestScript1.lua");
  
  // Change and use Luse state "Init"
  LuaManager::UseLuaState("Init");
  // Register a global function so it can be used in a Lua script
  LuaFunctionsWrapper::RegisterCFunction("foo", &foo);
  
  // Change and use the newly created Luse state "SecondLuaState"
  LuaManager::UseLuaState("SecondLuaState");
  LuaFunctionsWrapper::RegisterCFunction("bar", &bar);
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```
Now the function "foo()" can only be called from Lua state "Init" and "bar()" can only be called from Lua state "SecondLuaState"

### Register a C++ function

```c++
bool LuaFunctionsWrapper::RegisterCFunction(functionName, classInstance, function)
```
- functionName = the name that the function has in Lua
- classInstance = the class that the function is a member function of
- function = the function that shall be registered

return = if an error occurred returns false, eller true

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
