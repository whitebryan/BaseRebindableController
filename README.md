# BaseRebindableController
 Basic rebindable player controller for UE5 using enhanced input
 Comes with simple UI to show how it can work
 Saves and loads keybinds automatically, saves to "Keybinds" slot
 
 TO USE:
 
 Drop into plugins
 
 Create a child C++ or Blueprint class of BaseCharacter
 
 Create a child C++ or Blueprint class of BaseController
 
 Migrate the Blueprints/Input folder to your projects content folder or just make new InputMappingContexts
 
 Add your contexts to the child of BaseController 
 
 Set your gamemode to use your new character and controller 
