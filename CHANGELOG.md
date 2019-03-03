# Changelog
All notable changes to this project will be documented in this file.

## [0.10.0] - 2019 - 03 - 02
### Changed
- Cleaned some exception handlers

### Added
- An error message with a suggestion to run as Administrator.
- A status in the options menu displaying the current steam folder
- **Important** You can now select your current game version (in case you have a different one) throught the options menu bar
You can check your game version by reading the number displayed on the MHW Window.

## [0.9.3] - 2019 - 02 - 21
### Fixed
- Compatibility with game verison 166849, previous versions of the game are no longer compatible.

## [0.9.1] - 2019 - 01 - 26
### Added
- A log entry for when the armorData.json file is not found, it will output where was it trying to search for it.
- The following armors:
    * Lunastra Gamma
    * Kushala Gamma
    * Zorah Gamma
    * Xeno Gamma
    * Gala.

## [0.9.0] - 2019 - 01 - 21
### Added
- The "Change All" Button which changes all the current selected to an entire armor set.
- A "writed values" entry to the log every time the program writes to the game.
- "Change Steam Directory" button.
- The settings.json file. All the custom settings will be writted here and the application will read it at start. If file is empty, the application will use default settings.
Available settings to change:
    * "Safe Mode" : true | false
    * "Disable Unsafe Warning" : true | false
    * "Steam Dir" : path_to_folder

### Changed
- The Displayed message when writting is successful.
- The paths to files are no longer relative to the current directory, but rather the directory of the ".exe" file. Adding the capacity of making shortcuts of the application.

### Fixed
- Compatibility with game version 165889, (compatibility with game version 163956 was deprecated... for now).


## [0.8.0] - 2019-01-08
### Added
- The CHANGELOG itself.
- "Unsafe Mode" In which you can select Armor sets that *may* crash the game or cause multiplayer issues.
For example : Using the Queen Beetle while having a male character or using a Low-Rank Armor while having Armor Pigment.
- "Clear" button which sets all current Armor to "Nothing".
- Saving sets Feature. "File > Save Current Armor" and then give the set a name.
- Loading saved sets Feature. "File > Load Armor" and then select from your saved sets.
- Added "Cancel" to the searching process to stop it from running forever.
- Key Shortcuts as follows:
    * Ctrl+S to Save Armor Set
    * Ctrl+A to Load Armor Set (I Don't Really like this one)
    * Ctrl+F to Fetch Game Data
    * Ctrl+W to Write Armor to Game
- Design to Input Dialogs

### Changed
- The format of ArmorData.json.
- The option "None 255" to "Nothing"

### Fixed
- Some Armors not being displayed as options.
