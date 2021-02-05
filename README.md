# xxImGui
A simulate Next-Generation Graphic API for ImGui

### Library Dependencies
| Library   | Note                                             |
| --------- | ------------------------------------------------ |
| imgui     | https://github.com/ocornut/imgui                 |
| freetype2 | https://gitlab.freedesktop.org/freetype/freetype |
| stb       | https://github.com/nothings/stb                  |
| xxGraphic | https://github.com/metarutaiga/xxGraphic         |
| xxMiniCRT | https://github.com/metarutaiga/xxMiniCRT         |

### Supported Graphic API
| API             | Android | iOS | macOS | Windows | Windows on ARM |
| --------------- | ------- | --- | ----- | ------- | -------------- |
| Direct3D 6.0    |         |     |       | ⭕       |                |
| Direct3D 7.0    |         |     |       | ⭕       |                |
| Direct3D 8.0    |         |     | 🍷     | ⭕       |                |
| Direct3D 9.0    |         |     | 🍷     | ⭕       | 🚑              |
| Direct3D 9Ex    |         |     | 🍷     | ⭕       | 🚑              |
| Direct3D 9On12  |         |     |       | ⭕       | 🚑              |
| Direct3D 10.0   |         |     |       | ⭕       | ⭕              |
| Direct3D 10.1   |         |     |       | ⭕       | ⭕              |
| Direct3D 11.0   |         |     |       | ⭕       | ⭕              |
| Direct3D 11On12 |         |     |       | ⭕       | ⭕              |
| Direct3D 12.0   |         |     |       | ⭕       | ⭕              |
| OpenGL ES 2.0   | ⭕       | ⭕   | ⭕     | ⭕       |                |
| OpenGL ES 3.0   | ⭕       | ⭕   | ⭕     | ⭕       |                |
| OpenGL ES 3.1   | ⭕       |     |       | ⭕       |                |
| OpenGL ES 3.2   | ⭕       |     |       | ⭕       |                |
| Mantle          |         |     |       | 🚑       |                |
| Metal           |         | ⭕   | ⭕     |         |                |
| Metal 2         |         | ⭕   | ⭕     |         |                |
| Vulkan          | ⭕       | ⭕   | ⭕     | ⭕       |                |

### Build and Run xxImGui for Windows on macOS
* Install wine and llvm
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew tap gcenx/wine
brew install gcenx-wine-staging
brew install llvm
```
* Edit build/xxImGui.xcodeproj/xcshareddata/xcschemes/xxImGui (Windows).xcscheme
```
   <LaunchAction
      buildConfiguration = "Release"
      ...
      allowLocationSimulation = "YES">
      <PathRunnable
         runnableDebuggingMode = "0"
         FilePath = "/usr/local/bin/wine64">
      </PathRunnable>
      <CommandLineArguments>
         <CommandLineArgument
            argument = "xxImGui.exe"
            isEnabled = "YES">
         </CommandLineArgument>
      </CommandLineArguments>
   </LaunchAction>
```
