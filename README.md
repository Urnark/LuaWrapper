
# LuaWrapper
#### This project is not done. The code might change

### Table of Contents
  1. [Documentation](#documentation)
     - [Start and close the LuaWrapper](#init-luawrapper)
     - [Load a Lua script](#load-lua-script)
     - [Create and use more Lua states](#more-lua-states)
     - [Call a Lua function](#call-lua-function)
     - [Register a C++ function as a global function in Lua](#register-global-function)
     - [Register a C++ function as a member function in Lua](#register-member-function)
     - [Show debug information](#debug-flags)
  2. [License](#license)

<a name="documentation"/></a>
# Documentation
<a name="init-luawrapper"/></a>
### Start and close the LuaWrapper
Every function in the LuaWrapper is in the namespace LFW (Lua Function Wrapper).
To use the LuaWrapper it is needed to initialize the wrapper. You can do that by calling the function "InitLuaManager".
```c++
void LuaManager::InitLuaManager(lua state name);
```
lua state name = name of the lua state. If not used then it is named "Init"

And when done to use the LuaWrapper it is needed to close it. You can do that by calling the function "CloseLuaManager"
```c++
void LuaManager::CloseLuaManager();
```
<a name="load-lua-script"/></a>
### Load a Lua script
To load a Lua script so it can be used in the program, the function "LoadScript" is used.
```c++
void LuaManager::LoadScript(lua state name, path of lua script);
```
- lua state name = the name of the Lua script that the script shall belong to
- path of lua script = the path to the Lua script

The first argument is only there to make the code smaller, more about using more Lua states in a program esists [here](#more-lua-states).

<a name="more-lua-states"/></a>
### Create and use more Lua states
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

The function that takes a index as parameter is faster than the other. So if you need to have a faster code then that is what you should use.

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
  LuaManager::LoadScript("SecondLuaState", "LuaTestScript2.lua");
  
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

<a name="call-lua-function"/></a>
### Call a Lua function
To call a Lua function use the function "CallLuaFunction".
```C++
Ret LuaManager::CallLuaFunction<Ret>(luaFunctionName, arguments);
```
- luaFunctionName = the name of the function in Lua that shall be called
- arguments = the arguments that the Lua function take as parameters
- Ret = what the Lua function returns

Example:
```C++
using namespace LFW;

void main() {
  // Init lua
  LuaManager::InitLuaManager();

  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");
  
  // Call the Lua function "foo"
  LuaManager::CallLuaFunction<void>("foo", 10);
  
  // Call the Lua function "bar"
  std::string messageFromBar = LuaManager::CallLuaFunction<std::string>("foo");
  std::cout << messageFromBar << std::endl;
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```
In the Lua script
```Lua
-- LuaTestScript1.lua --

function foo(nr)
  print("Hello from foo with number: " .. nr)
end

function bar()
  return "Hello from bar"
end
```

If the Lua function returns more than one value then this is used instead:
```C++
void LuaManager::CallLuaFunction<nrOfRet>(luaFunctionName, arguments);
```
- luaFunctionName = the name of the function in Lua that shall be called
- arguments = the arguments that the Lua function take as parameters
- nrOfRet = the number of return values that the Lua function returns to C++
And to get the returned values the function "GetAll" is used.
```C++
void LuaManager::GetAll(variables);
```
- variables = the valriables that shall contain all the returned values

It exists another way to call Lua functions that return more than one values
```C++
Ret LuaManager::CallLuaFunction<Ret, nrOfRet>(luaFunctionName, arguments);
```
- luaFunctionName = the name of the function in Lua that shall be called
- arguments = the arguments that the Lua function take as parameters
- Ret = what the Lua function returns
- nrOfRet = the number of return values that the Lua function returns to C++, including the Ret
It works the same as the first but you need to specisy the number of returned values.
The difference with this function is that you can spacify to return the last returned value from a Lua function before returning the other.

Example:
```C++
using namespace LFW;

void main() {
  // Init lua
  LuaManager::InitLuaManager();

  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");
  
  // Call the Lua function "foo"
  LuaManager::CallLuaFunction<2>("foo", 10);
  int ret1;
  std::string ret2;
  LuaManager::GetAll(ret1, ret2);
  std::cout << "Returning: " << ret1 << ", " << ret2 << std::endl;
  
  int nr =  LuaManager::CallLuaFunction<int, 2>("bar", 15);
  if (nr > 0)
  {
    std::cout << LuaManager::GetString() << std::endl;
  }
  else // Remember to remove the return values from the Lua stack if not used
    lua_pop(LFW::LuaManager::GetCurrentState(), -1);
    
  // Call the Lua function "bar"
  std::string messageFromBar = LuaManager::CallLuaFunction<std::string>("foo");
  std::cout << messageFromBar << std::endl;
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```
In the Lua script
```Lua
-- LuaTestScript1.lua --

function foo(nr)
  return nr, "Hello from foo"
end

function bar(nr)
  return "Hello from bar", nr
end
```

<a name="register-global-function"/></a>
### Register a C++ function as a global function in Lua
The function that is used to register c++ function for Lua to use as a global Lua function is "RegisterCFunction".
```c++
bool LuaFunctionsWrapper::RegisterCFunction(functionName, classInstance, function);
```
- functionName = the name that the function has in Lua
- classInstance = the class that the function is a member function of
- function = the function that shall be registered

classInstance is not needed if the function is not a member fucntion or if the function is static

return = if an error occurred returns false, else true

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

  // Register a global function so it can be used in a Lua script
  LuaFunctionsWrapper::RegisterCFunction("foo", &foo);
  
  // Register a member function so it can be used in a Lua script
  Bar bar;
  LuaFunctionsWrapper::RegisterCFunction("IsPositive", &bar, &Bar::IsPositive);

  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```
Need to register functions before calling "LoadScript". Else you can need to call c++ functions in a Lua function

In the Lua script
```Lua
-- LuaTestScript1.lua --

foo()

positiveNr = IsPositive(34)
print(positiveNr)

```

<a name="register-member-function"/></a>
### Register a C++ function as a member function in Lua
The function that is used to register c++ function for Lua to use as a member fucntion for a c++ class that is passed to Lua is "LFW_RegisterCObjectFunctions".
```c++
LFW_RegisterCObjectFunctions(classInstance, LFW_function(functionName, function)...);
```
- classInstance = the class that the function is a member function of
- LFW_function(functionName, function)... = for all the functions that the class instance shall be able to use.
```c++
LFW_function(functionName, function);
```
- functionName = the name that the function has in Lua
- function = the function that shall be registered

An exapmle of how it is used:
```C++
// If a function in a class shall be used in Lua then it need to inherit from ILuaMember
class Bar : public ILuaMember
{
public:
  void Print() {
    std::cout << "This is a Bar instance" << std::endl;
  }
  bool IsPositive(int i) {
    return (i < 0? false: true);
  }
}

void main() {
  // Init lua
  LuaManager::InitLuaManager();
  
  Bar bar;
  LFW_RegisterCObjectFunctions(bar,
    LFW_function("IsPositive", Test::IsPositive), 
    LFW_function("Print", Test::Print)
  );
  
  // Load a Lua script
  LuaManager::LoadScript("LuaTestScript1.lua");
  
  // Call the Lua function that shall be used and pass the class instance as a parameter to it
  LuaManager::CallLuaFunction<void>("UpdateBar", &bar);
  
  // Close Lua
  LuaManager::CloseLuaManager();
}
```
And in the Lua script file:
```Lua
-- LuaTestScript1.lua --

function UpdateBar(bar)
  bar:Print()
  positiveNr = bar:IsPositive(34)
  print(positiveNr)
end
```

<a name="debug-flags"/></a>
### Show debug information
You can show debug information in the console window by setting the "DEBUG_FLAGS" variable.<br />
The flags that can be used are:
- ERRORS
- FUNCTION_CALLS
- REGISTER_FUNCTIONS
- DEBUG_PRINTS
To set a flag you only need to do this in the top of your main function:
```C++
void main() {
  LuaManager::DEBUG_FLAGS = LFW::ERRORS | LFW::DEBUG_PRINTS | LFW::FUNCTION_CALLS | LFW::REGISTER_FUNCTIONS;
  // more code
}
```
##### ERRORS
Display error messages
##### FUNCTION_CALLS
Display messages for when a registered function has been called and the called functions information
##### REGISTER_FUNCTIONS
Display messages for when a function has been registered and the functions information.
##### DEBUG_PRINTS
Display all messages that is not in the other categories.

<a name="license"/></a>
### License
[MIT](LICENSE.md)<br />
[![alt text](https://www.lua.org/images/lua-language.gif)](http://www.lua.org/)<br />
[Lua License](LuaWrapper/Lua/LuaLICENSE.txt)
