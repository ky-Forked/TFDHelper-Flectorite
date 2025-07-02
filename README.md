# Flectorite - TFDHelpers DescendantInternal Rename.
<h2>TFD Helper - Flectorite</h2>

This is just a cleaned up setting, the I changed the name of this version more to be seperate from the still working DescendantInternal Athrun maintains.
The biggest difference is this DLL will prevent Black Cipher from launching with the game when started, meaning prior to using this all you need to do is bypass EAC.
To bypass EAC, open Steam and right click 'The First Descendant', hover over 'Manage' then click 'Browse local files'. Open the folder 'EasyAntiCheat' then open 'Settings.json', there will be 3 IDs in this file. My recommendation is randomly replace a couple of letters with different letters and a couple numbers with different numbers, essentially only CHANGE a letter/number, dont add or subtract from the total count. Safe the 'Settings.json' file and next time the game is launched EAC will not connect to the games servers/run properly.

Use Extreme Injector 3.7.2/3.7.3 First, I dont care what Injector you have or use normally, if Extreme Injector Auto Injecting doesnt work its a different issue. If you can use Extreme then use whatever backwoods injector youd like as long as you do it right.
Open Extreme Injector and open The First Descendant, in Extreme Injector select The First Descendant. Now close The First Descendant. Add the DLL to Extreme Injector and set Extreme Injector to Manual Map + Auto-Inject. This DLL NEEDS to be injected as the game is Opening, attempting to Inject AFTER it started opening will either cause a crash or have nothing happening. Once the DLL is added to Extreme and the settings are correct, re-open The First Descendant. Extreme should close as the game is opening now, it shouldnt be open or anything as the game is loading the title screen. You may get a pop-up when you try to Login In from the title screen saying you were disconnected from the server or couldn't connect but just ignore it and try again.

Frame Generation, NVidia or AMDs, can/will cause problems for this so make sure both are turned off as well before posting about crashing.
Changing resoltions of the game after injection will result in crashing.
To my knowledge this also may not work with 4K or Ultra Wide, but im not fully sure. It may just be messy ESPs.
The config file will be 'cfg.ini' located at "...The First Descendant\M1\Binaries\Win64" after the first time injecting. If you crash once try deleting those.

Menu key is INSERT.
The menu will not function until you're fully in-game.
[COLOR="Lime"]ImGui is replacing ZeroGui in this version.[/COLOR]

[SIZE="2"] [COLOR="Red"]If you crash when using this, Try the other version from the other thread, Try to let me know if only this one isnt working or if its both. If you did use the other version at some point prior to trying this one, delete the cfg file and try again. Old config files and the two being swapped between versions can cause some weird visual things and who knows what else.[/COLOR] [/SIZE]

--ESP for Players, Enemies, Items, Resources, Coded Vaults--
Boxes
Circles
Names
Lines
--Aimbot--
[LIST]
Hold or Toggle Aimbot (Changing Binds to Mouse key currently isnt working, will be fixed in next release)
Adjustable Targetting Distance
Adjustable Aim Smoothing
No Spread or Recoil
Instant/Auto Reload (Bullet count hits 3 and will refil to full)
Rapid Fire (Only works properly with semi-auto weapons, Ghost bullets are common otherwise or too high)
Targets Head and Weakness bones, and *most* invulnerability spheres
Controller support using Hold mode with Left Trigger
[/LIST]
[SIZE="4"]Extras[/SIZE]
[LIST]
[*][COLOR="MediumTurquoise"]Spawn Vault Rewards : During Vault MiniGames can be used to get rewards like it was completed on every key press during the Minigame.[/COLOR]
[*][COLOR="MediumTurquoise"]Spawn Generic/World Loot : Will spawn the loot Field Interactables drop, like Vulgus Resource Crates or Void Vessel Containers.[/COLOR]
[*][COLOR="MediumTurquoise"]Custom Research Amounts : Allows Researching more then the standard amount allowed in game on items, Bookmark whatever item and start the research from the Menu after setting Quantity. You still need the resources that would be expected, this is mainly for things like Catalysts or Activators, things that only allow 1 or a few to be researched normally. [/COLOR]
[*]Loot Vacuum Toggle Key
[*][COLOR="MediumTurquoise"]Loot Filters + Settings to limit grabbing health and mana with Loot Vacuum.[/COLOR]
[*]Adjustable Loot Vacuum Range
[*][COLOR="MediumTurquoise"]Character Swapping was entirely replaced bu Preset Swapping. Preset Swapping will let you essentially refill Health, Mana, and Ammo.[/COLOR]
[*]Adjustable Timescale with a Toggle Key
[*][COLOR="MediumTurquoise"]Not using a Grapple Module or using Mid-Air will let you enable Grapple Mod to set its max range.
[/LIST]
[SIZE="4"]Missions[/SIZE]
[LIST]
[*][COLOR="MediumTurquoise"]Mission ESP[/COLOR]
[*][COLOR="MediumTurquoise"]Mission Teleport[/COLOR]
[*][COLOR="MediumTurquoise"]Auto Mission Restart[/COLOR]
[*][COLOR="MediumTurquoise"]Instant Infiltration[/COLOR]
[/LIST]

Most game updates/hotfixes shouldn't require a cheat update unless they change data offsets as they have every now and then. Blizzie has yet to be banned while making this cheat even while BlackCipher was running for a good while and using it almost since game launch. Cheating, especially using a public cheat, means you are at risk of being banned at any time.

[size=4]Credits[/size]
Original Forum Thread at : [URL="https://www.unknowncheats.me/forum/other-fps-games/658547-descendantinternal-descendant.html"]DescendantInternal for The First Descendant[/URL]
Everyone in the TFD discussion thread for code examples and their help with my questions: [URL="https://www.unknowncheats.me/forum/other-fps-games/602336-descendant-reversal-structs-offsets.html"]The First Descendant Reversal, Structs and Offsets[/URL]
[COLOR="Magenta"]eizzliB[/COLOR] - Creating the original DescendantInternal and this new version while cleaning up the messy code we kept adding to the older version.
[COLOR="DarkOrchid"]athrun0001[/COLOR] - Keeping DescendantInternal up-to-date and working while also adding to it and finding new things like spawning Vault Drops.

If you have questions or need help, first read through all the instructions given and maybe check the other forum to see if someone had the same issue at some point. Ill try to respond and help but I am very inconsistent with when I am able to sit down and troubleshoot for hours.

If things aren't working for you, double check things on your side before saying something is broke with the DLL, this version in testing before I uploaded it here had issues but we held back on releasing it until everyone that had issues getting it to work no longer did. Otherwise if an update didnt come out recently, the DLL is most likely working fine for most other people.

[SIZE="4"]Download[/SIZE]
[SIZE="3"]v1.0.21[/SIZE]
Source + Precompile DLL - [URL="https://www.unknowncheats.me/forum/downloads.php?do=file&id=50174"]https://www.unknowncheats.me/forum/downloads.php?do=file&id=50174[/URL]
(Compiled and Released June 21st 2025)

[SIZE="3"]v1.0.23[/SIZE]
Source + Precompile DLL - [URL="https://www.unknowncheats.me/forum/downloads.php?do=file&id=50208"]https://www.unknowncheats.me/forum/downloads.php?do=file&id=50208[/URL]
(Compiled and Released June 24th 2025)
[SIZE="2"][COLOR="Red"]Changes[/COLOR][/SIZE]
[spoiler]ESP Shapes are fixed to a point I visually am fine with. 
Mouse Keys can be recorded by ImGui Menu for changing Hotkeys to Mouse keys.
The loot drop function now has a range check and multiply option again. 
The range check wont apply unless Multiply is enabled since I thought it really didnt matter unless you wanted to multiply the drops and when within range Containers will receive a small multicolored circle drawn on it to show which is being targeted. (This is useful for farming specific Void Vessel Containers, to our knowledge no one has been banned for doing this over the last almost 2 months of heavy abuse.)
The grapple mod was also cleaned up for now and im done messing with it, it still only works with No Grapple module equipped or the Mid-Air one but when enabled you can choose the range those will fire to, I also made the grapple cast faster but sadly couldn't make it pull faster.[/spoiler]

[SIZE="3"]v1.0.24[/SIZE]
Source + Precompile DLL : [URL="https://www.unknowncheats.me/forum/downloads.php?do=file&id=50257"]https://www.unknowncheats.me/forum/downloads.php?do=file&id=50257[/URL]
(Compiled and Released June 28th 2025)
Fixed Black Cipher Bypass (New Sig eizzilB Provided should be a bit more future update proof.)
[SIZE="1"]Sorry for the delay uploading this, my PC has not been happy with me and tried to lobotomize itself so I had to first.[/SIZE]

[SIZE="3"]v1.0.25[/SIZE]
Source + Precompile DLL - [URL="https://www.unknowncheats.me/forum/downloads.php?do=file&id=50266"]https://www.unknowncheats.me/forum/downloads.php?do=file&id=50266[/URL]
(Compiled and Released June 29th 2025)
[SIZE="2"][COLOR="LimeGreen"]CRASH FIX[/COLOR][/SIZE]
[SIZE="1"]The Grapple mod was cleaned up, I removed most stuff I was using in testing and left it with just what made it work.[/SIZE]

[SIZE="3"]v1.0.27/Flectorite[/SIZE]
(Compiled and Released July 2nd 2025, Github link set up so new branch to forget past sins.)

[SIZE="4"]Using The Source[/SIZE]
[spoiler]
The entire source is included so you should be able to just open the solution and build immediately without errors.
To update the SDK, you will need to get a fresh dump from the game using Dumper-7 or your own tool(s) and just compare the classes and structs for size and offset changes. The core Engine classes have rarely changed so usually you only need to focus on the M1 classes.
The static asserts will show as errors but will be valid when compiled so don't worry about them unless you've edited the corresponding classes.

To add your own functionality that uses classes/structs/functions that don't already exist in the project then you will need to modify the corresponding areas as I've stripped everything down to the bare minimum for the cheat to work as intended.

If for some reason the game is crashing on load after a game update, there's a chance the No Spread signature will need to be updated. The area I have selected in the image below is what my signature looks for, and then overwrites. Everything else around it has been the same so it shouldn't be hard to find.
[img]https://i.imgur.com/84pW6Tq.jpeg[/img]
[/spoiler]
