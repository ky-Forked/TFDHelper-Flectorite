# Flectorite

Flectorite is a modification for *The First Descendant*, building upon the original work of DescendantInternal. This version is designed to be a cleaner and more distinct tool.

A key feature of Flectorite is its ability to prevent Black Cipher from launching with the game. This requires bypassing EAC (EasyAntiCheat) beforehand.

---

## 🛑 Important Notices

* **Game Settings:** For optimal performance and to avoid crashes, please disable Frame Generation (both NVIDIA and AMD). It is also recommended to set your desired screen resolution *before* injecting the DLL, as changing it mid-game will cause a crash. This tool may not function correctly with 4K or ultra-wide resolutions.
* **Configuration File:** The configuration file, `cfg.ini`, will be created in `...The First Descendant\M1\Binaries\Win64` after the first successful injection. If you experience crashes, try deleting this file.
* **Disclaimer:** Using this tool, especially a public version, carries an inherent risk of a game ban. While it has been tested to minimize this risk, use it at your own discretion.

---

## 🚀 Getting Started

### 1. Bypass EAC

1.  Open Steam, right-click on **The First Descendant**, and select **Manage** > **Browse local files**.
2.  Navigate to the `EasyAntiCheat` folder and open `Settings.json`.
3.  You will find three IDs in this file. Randomly change a few letters and numbers within these IDs. **Do not alter the total length of the IDs.**
4.  Save the `Settings.json` file. This will prevent EAC from properly connecting.

## 2. Building from Source

To build the `Flectorite.dll` from the source code, please follow these steps:

1.  Open the solution in Visual Studio. You can do this by clicking on **Code** and then **Open with Visual Studio**.
2.  Set the build configuration to **Release** and the platform to **x64**.
3.  Build the solution by navigating to **Build > Build Solution**.
4.  Once the build is complete, you can find the **Flectorite.dll** file in the output folder.

### 3. Injection Process

**Note:** It is highly recommended to use **Extreme Injector v3.7.2 or v3.7.3** for the initial setup.

1.  Launch Extreme Injector and select "The First Descendant" process, then close the game.
2.  In Extreme Injector, add the Flectorite DLL.
3.  Configure the injection settings to **Manual Map** and **Auto-Inject**.
4.  Relaunch *The First Descendant*. The injector should automatically inject the DLL as the game starts and then close itself. You may need to pre-select M1-Win64-Shipping.exe from the processes by browsing your game files to TheFirstDescendant>M1>Binaries>Win64>M1-Win64-Shipping.exe

You may see a "disconnected from server" error on the title screen; this is normal. Simply try to log in again. The menu, accessed with the **INSERT** key, will be available once you are fully in-game.

---

## ✨ Features

### ESP
* **Players, Enemies, Items, Resources, Coded Vaults**
    * Boxes
    * Circles
    * Names
    * Lines

### Aimbot
* **Toggle or Hold Aimbot**
* **Adjustable Targeting:** Control distance and aim smoothing.
* **Weapon Enhancements:** Includes No Spread, No Recoil, Instant/Auto Reload, and Rapid Fire (most effective with semi-automatic weapons, doesn't really work for full-auto guns).
* **Targeting:** Aims for the head, weak points, and most invulnerability spheres.
* **Controller Support:** Available with "Hold" mode using the Left Trigger.

### Extras
* **Vault Rewards:** Spawn rewards during vault minigames.
* **Loot Generation:** Spawn loot from world interactables.
* **Custom Research:** Research larger quantities of items than normally allowed (requires the necessary resources).
* **Loot Vacuum:** Toggleable with adjustable range and filters for health and mana.
* **Preset Swapping:** Refill your health, mana, and ammo.
* **Adjustable Timescale:** Control the game speed with a toggle key. (Will cause server desync)
* **Enhanced Grapple:** Modify the grapple's maximum range (when no grapple module or the "Mid-Air" module is equipped).

### Missions
* **Mission ESP**
* **Mission Teleport**
* **Auto Mission Restart**
* **Instant Infiltration**

---

## 🔧 For Developers (Using the Source)

The included source code should allow you to build the solution without any initial errors.

* **SDK Updates:** To update the SDK, use a tool like Dumper-7 to get a fresh dump from the game and compare class and struct sizes and offsets. Core engine classes rarely change, so focus on the M1 classes.
* **Adding Functionality:** If you wish to add new features that rely on classes or structs not already in the project, you will need to modify the relevant areas. The project has been stripped down to the essentials.
* **Troubleshooting Crashes:** If the game crashes on load after an update, the "No Spread" signature may need to be updated. Refer to the image for the specific area to look for.

## 📜 Credits

* **Original Concept:** [DescendantInternal](https://www.unknowncheats.me/forum/other-fps-games/658547-descendantinternal-descendant.html)
* **TFD Discussion & Resources:** [TFD Reversal Thread](https://www.unknowncheats.me/forum/other-fps-games/602336-descendant-reversal-structs-offsets.html)
* **eizzliB:** For creating the original DescendantInternal and this cleaner version.
* **athrun0001:** For Maintaining and updating DescendantInternal.
* **Tivmo:** Maintaning and updating Flectorite.
* **Sinnisterly:** Helping with Discord and Github (Creator of this ReadMe and TFD Helper Bot)


If you have questions or need help, first read through all the instructions given and maybe check the other forum to see if someone had the same issue at some point. Ill try to respond and help but I am very inconsistent with when I am able to sit down and troubleshoot for hours.
If things aren't working for you, double check things on your side before saying something is broke with the DLL, this version in testing before I uploaded it here had issues but we held back on releasing it until everyone that had issues getting it to work no longer did. Otherwise if an update didnt come out recently, the DLL is most likely working fine for most other people.


---

## 📦 Downloads & Changelog

* **v1.0.30 (Flectorite) - July 10th, 2025**
    * Auto-Restock + Ability Cooldown Reset Added

* **v1.0.29 (Flectorite) - July 8th, 2025**
    * Auto-Restart added for Infiltration Outposts.
    * Style Saving/Loading correctly done in configs.

* **v1.0.28 - July 3rd, 2025**
    * Style Selection added to Extra tab.
      
* **v1.0.27 (Flectorite) - July 2nd, 2025**
    * Initial release under the Flectorite name.

* **v1.0.25 - June 29th, 2025**
    * **CRASH FIX:** Cleaned up the grapple mod implementation.
    * [Source + Precompiled DLL](https://www.unknowncheats.me/forum/downloads.php?do=file&id=50266)

* **v1.0.24 - June 28th, 2025**
    * Fixed Black Cipher bypass with a new, more future-proof signature.
    * [Source + Precompiled DLL](https://www.unknowncheats.me/forum/downloads.php?do=file&id=50257)

* **v1.0.23 - June 24th, 2025**
    * Fixed ESP shapes.
    * Enabled mouse key binding for hotkeys.
    * Re-implemented range check and multiply options for loot drops.
    * Added a visual indicator for targeted containers.
    * Grapple mod cleanup and faster casting.
    * [Source + Precompiled DLL](https://www.unknowncheats.me/forum/downloads.php?do=file&id=50208)

* **v1.0.21 - June 21st, 2025**
    * Initial public release.
    * [Source + Precompiled DLL](https://www.unknowncheats.me/forum/downloads.php?do=file&id=50174)
