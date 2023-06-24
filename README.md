# BaseRebindableController
 Only tested in unreal 5.1.1 and unreal 5.2 but should workd in any 5+ version
 
 Line 120 of BaseRebindableController.cpp needs to be changed to curMapping.bIsPlayerMappable if version is lower than 5.2
 
 UI and example InputContexts likely wont load in engine versions lower than 5.2 either

 Basic rebindable player controller for UE5 using enhanced input
 Comes with simple UI to show how it can work
 Saves and loads keybinds automatically, saves to "Keybinds" slot
 
 TO USE:
 1. Drop into plugins
 2. Create a child C++ or Blueprint class of BaseCharacter
 3. Create a child C++ or Blueprint class of BaseRebindableController
 4. Migrate the Blueprints/Input folder to your projects content folder or just make new InputMappingContexts
 5. Add your contexts to the child of BaseController 
 6. Set your gamemode to use your new character and controller 



TO ADD NEW ACTIONS:
1. Create an InputAction in the Editor

IF C++:

2. Inside the header of your child of BaseRebindableController add a UFUNCTION() named Exactly the same as the InputAction
3. Inside the cpp file define the function
4. Cast "PossessedCharacter" to your child of BaseCharacter and call whatever you need on BaseCharacter

IF BLUEPRINT:

2. Inside your child of either BaseRebindableController or BaseCharacter right click and add the new input action 

