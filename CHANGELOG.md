# Changelog
All notable changes to this project will be documented in this file.

## [0.16.9] - 2020 - 04 - 16
### Added
- Compatibility with version 410013
- Furious Rajang and Raging Brachydios

## [0.16.8] - 2020 - 03 - 29
### Added
- Compatibility with version 408899

## [0.16.7] - 2020 - 03 - 19
### Fixed
- Orion armors

## [0.16.6] - 2020 - 03 - 14
### Fixed
- Leon, Claire and Rajang armors were missing
### Changed
- Armor data spreadsheet is now a csv file
### Removed
- All the low rank armors


## [0.16.5] - 2020 - 03 - 12
### Added
- Compatibility with version 406510
- Stygian Zinogre and Safi'jiva

## [0.16.4] - 2020 - 02 - 25
### Fixed
- steamID is now of type u_int instead of int

## [0.16.3] - 2020 - 02 - 20
### Added
- Compatibility with version 404549

## [0.16.2] - 2020 - 02 - 08
### Added
- Chinese Iceborne Armor Names

## [0.16.1] - 2020 - 02 - 06
### Fixed
- An error with the version number check
- Added a command to copy openSSL at release Build
- Added Leon and Claire Armors

## [0.16.0] - 2020 - 02 - 06
### Added
- Compatibility with game version 402862
- Rajang A+ and B+ armors
- Option to re-turn on Steam path automatic search
- Added an auto-updater feature and a Search for Updates option
- An option to select the level of the log
- Now the program can be opened before MHW and does not need to be closed if the game is restarted
- An checkable option to write without backup save file
- A Log viewer option under the Debug Menu

### Changed
- Restructured the entire file tree
- Manually Input ID moved to the Debug Menu
- ArmorData.json is now embeded in the executable
- Changed own implementation of filesystem to use Qt's QDir and QFile
- Changed nlohmann implementation of json to use Qt's QJsonObject
- Settings is now a .ini file and almost every setting has changed its name, but currently there are no hidden settings that cannot be changed using the graphical interface

### Removed
- Removed safe mode and Low Rank Armors

## [0.15.1] - 2020 - 01 - 29
### Added
- Support for game version 401727
### Fixed
- The Searching Data part not working sometimes

## [0.15.0] - 2020 - 01 - 19
### Added
- Iceborne Compatibility
- Support for game version 400974
- Lots of Iceborne Armors (Hopefully All of them)

### Changed
- The maximum value for armor ID is now 512

## [0.14.4] - 2019 - 12 - 07
### Added
- Support for game version 168030
- Defender alpha armor

## [0.14.2] - 2019 - 07 - 18
### Added
- Support for game version 167796
- Korean armor names

### Changed
- The load saved set hotkey from ctrl+a to ctrl+d.
- The behaivor of the Load button. The load button on the toolbar will remain the same, while 
the one under the File menu and also the ctrl+d hotkey one will now pop a window prompting to select one of your saved sets, just like it did before the saved sets toolbar.

### Fixed
- When failing to search for game data, the application will no longer crash. Instead it will display a message.
- The change all button was only displaying one-piece sets, instead of full-sets.

## [0.14.1] - 2019 - 06 - 25
### Added
- Simplified Chinese Armor Names
- Female and Male Butterfly Armors

## [0.14.0] - 2019 - 06 - 21
### Added
- Traditional Chinese Armor Names

### Fixed
- Some armor names

## [0.13.0] - 2019 - 06 - 08
### Added 
- Toolbar with editable combobox for saved sets
- Option at menu to manually input an ID for an armor, use with caution
- Nergigante Y

## [0.12.0] - 2019 - 05 - 19
### Added
- Bayak Layered, Ciri A, Geralt A

### Fixed
- The MOD not working on character slots 2 and 3
- The backups were corrupted

### Changed
- Migrated from experimental::filesystem to own filesystem implementation

## [0.10.2] - 2019 - 05 - 11
### Added
- Origin Layered and Kulve Y armors.
- A global exception handler to print crash reasons in the log.
- Support for game version 167541
- The entry for Queen Beetle Armor (Note that Butterfly and Queen Beetle are the same armor). After some experiments we determined what happens if you equip King or Queen Beetle Armors if your character is Male or Female.
The armor displayed in game is as follows:

If your character is Male:
* if yoy equip King Beetle -> The Game Displays King Beetle.
* if yoy equip Queen Beetle -> The Game Displays Butterfly.

If your character is Female:
* if yoy equip King Beetle -> The Game Displays Butterfly.
* if yoy equip Queen Beetle -> The Game Displays King Beetle.

Selecting Butterfly always displays Butterfly despite the gender.

### Changed
- The MHW_Armor_Data.xlsx file to a different format.
- MainWindow.cpp was splitted into different files because it was getting too big.
There should be no noticable changes due to this action
- From std::filesystem to experimental::filesystem. So now it can be compiled using Mingw7.3 bundled with QT5.

### Removed
- The repeated entries for King Beetle armor

## [0.10.1] - 2019 - 04 - 03
### Added
- Support for Game Version 167353

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
