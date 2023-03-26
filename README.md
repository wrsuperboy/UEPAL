# UEPAL
A UE-based cross-platform reimplementation of the classic Chinese RPG game "PAL".

## Getting started
To start working with this project, you'll need to follow a few simple steps:
1. Ensure that you have Unreal Engine and Visual Studio installed.
2. Ensure that you have PAL game files.
3. Configure in `Config/DefaultGame.ini`, change `GameResourcePath` under `/Script/PAL.PALCommon` into your game path.
4. Open Unreal Project Browser.
5. Click "Browse..." and select `PAL.uproject` to import the project.
6. In the opened UE editor, click on Menu > Tools > Refresh Visual Studio Project.
7. Open VS project by clicking on Menu > Tools > Open Visual Studio.
8. In Visual Studio, in Solution Explorer, right click on project `PAL` and select "Set as Startup Project".
9. Select `Development Editor` as active solution configuration. Try debug the game in Menu > Debug > Start Debugging.

Please note that:
- This project is built on Unreal Engine 5.1, and may not be compatible with earlier or later versions of the engine.
- This project is tested on PAL DOS and 95(98) versions. It may support other versions such as Dream. Please find legal copy of the game files. 
- If you want to publish an Android distribution, please configure the game path in `Config/Android/AndroidGame.ini`.

## Contribution
While we welcome contributors to this project, please note that we **do not permit forking** of this project. If you would like to contribute, please submit pull request in accordance with our contribution guidelines (coming soon).
