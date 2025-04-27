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

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` file for more information.
