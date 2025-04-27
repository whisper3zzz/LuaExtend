</p>

<p align="center">
  <img src="https://img.shields.io/badge/lang-C-yellow.svg">
     <img src="https://img.shields.io/badge/lang-Lua-blue.svg">
  <img src="https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgreen.svg">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg">
</p>

## Installation

```bash
git clone https://github.com/yourusername/LuaExtend.git
cd LuaExtend

# Build with CMake
mkdir build
cd build
cmake ..
cmake --build .

# Optional: Install the library
cmake --install .
```

## Usage
```lua
[[-- 
This code demonstrates how to load the LuaExtend library in a Lua environment:
1. First line modifies the C library search path (package.cpath) to include your DLL directory
    Replace {yourDllLibPath} with the actual path to the directory containing your DLL files
2. Second line loads the LuaExtend module using require()
    Replace {moduleName} with the specific module name you want to use from LuaExtendLib
]]

package.cpath=package.cpath..";{yourDllLibPath}"
local json = require("LuaExtendLib. {moduleName}")  

-- Example usage
package.cpath=package.cpath..";C:/LuaLibs/?.dll"
local json = require("LuaExtendLib.json")
local json_str = [[
{
    "name": "John",
    "age": 30,
    "city": "New York"
}
]]
local json_obj = json.load(json_str)
print(json_obj.name)  -- 输出: John
local new_json_str = json.dump({name = "Alice", age = 25})
print(new_json_str)
```

## Documentation

For detailed API documentation, please see the  `docs` directory.

