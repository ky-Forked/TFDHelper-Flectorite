#include "config.h"
#include <iostream>

namespace Menu
{
	extern int CurrentStyleIndex;
	void ApplySelectedStyle(int index);
}

namespace CFG
{
	CSimpleIniA ini;

	int			cfg_ShowMenuKey = VK_INSERT;

	bool		cfg_Actors_EnableESP = false;
	bool		cfg_Actors_DrawPlayerNames = false;
	bool		cfg_Actors_DrawPlayerBoxes = false;
	bool        cfg_Actors_DrawPlayerCircles = false;
	bool		cfg_Actors_DrawPlayerLines = false;
	bool		cfg_Actors_DrawEnemyNames = false;
	bool		cfg_Actors_DrawEnemyBoxes = false;
	bool        cfg_Actors_DrawEnemyCircles = false;
	bool		cfg_Actors_DrawEnemyLines = false; 
	float 		cfg_ESP_ShapeSize = 5.0f;
	float 		cfg_ESP_ShapeDistanceScale = 100.0f;

	bool		cfg_Loot_EnableItemESP = false;
	int			cfg_Loot_ItemFilterLevel = 1;
	bool		cfg_Loot_DrawItemBoxes = false;
	bool        cfg_Loot_DrawItemCircles = false;
	bool		cfg_Loot_DrawItemNames = false;
	bool		cfg_Loot_DrawVaults = false;
	bool		cfg_Loot_DrawSupplyResources = false;
	bool		cfg_Loot_DrawVoidVesselBox = false;
	bool		cfg_Loot_EnableLootVacuum = false;
	float		cfg_Loot_LootVacuumRange = 1000.0f;
	int			cfg_Loot_LootVacuumKey = 0x54;
	int			cfg_Loot_SpawnLootKey = VK_F5;
	int			cfg_Loot_SpawnVaultRewardKey = VK_F6;
	float 		cfg_Loot_ContainerDropRange = 500.0f;
	bool		cfg_Loot_MultiplyDrops = false;
	int			cfg_Loot_SpawnCount = 1;
	float		cfg_Loot_DrawHPThreshold = 50.0f;
	float		cfg_Loot_DrawMPThreshold = 50.0f;
	float       cfg_Loot_HPToRestock = 50.0f;

	bool		cfg_Aim_EnableAimbot = false;
	bool		cfg_Aim_EnableAimbotHold = true;
	bool		cfg_Aim_EnableAimbotToggle = false;
	int			cfg_Aim_AimbotHoldKey = 0x02;
	int			cfg_Aim_AimbotToggleKey = 0x59;
	bool		cfg_Aim_AimbotUseController = false;
	float		cfg_Aim_AimbotFOV = 140.0f;
	float		cfg_Aim_AimbotSpeed = 36.0f;
	bool		cfg_Aim_AimbotLineOfSight = false;
	bool		cfg_Aim_NoRecoilAndSpread = false;
	bool		cfg_Aim_NoReload = false;
	bool		cfg_Aim_RapidFire = false;
	float		cfg_Aim_FireRate = 1.0f;

	bool		cfg_Abilities_EnableModifyGrapple = false;
	float 		cfg_Abilities_GrappleRange = 10000.0f;
	bool        cfg_Abilities_AutoRestock = false;
	bool        cfg_Abilities_ResetCooldowns = false;
	int			cfg_Abilities_ResetCooldownsKey = 0x6;
	int			cfg_Abilities_Ability1Key = 0x51;
	int			cfg_Abilities_Ability2Key = 0x43;
	int			cfg_Abilities_Ability3Key = 0x56;
	int			cfg_Abilities_Ability4Key = 0x5A;

	float		cfg_Extra_TimeScale = 2.0f;
	int			cfg_Extra_TimeScaleKey = VK_F2;
	int			cfg_Extra_TimeScaleHoldKey = VK_CONTROL;
	bool		cfg_Extra_EnableUEConsole = false;

	bool		cfg_Mission_EnableMissionESP = false;
	bool		cfg_Mission_EnableMissionAutoRestart = false;
	float		cfg_Mission_MissionAutoRestartDelay = 5.0f;
	int			cfg_Mission_MissionAutoRestartKey = VK_F3;
	int			cfg_Mission_MissionTeleportKey = VK_F4;
	bool		cfg_Mission_EnableInstantInfiltration = false;
	int			cfg_Mission_InstantInfiltrationKey = VK_F7;

	bool		cfg_Hotswap_EnableOverlay = false;
	int			cfg_Hotswap_PresetSelectKey = VK_F8;
	int 		cfg_ResearchQuantity = 1;
	int 		CurrentStyleIndex = 0;

	bool		cfg_Debug_ShowConsole = false;

	bool		cfg_Customize_EnableCustomizationBypass = false;
	bool		cfg_Customize_EnableAutoApplyCustomization = false;
	// 0 Head, 1 Body, 2 Back, 3 Chest, 4 Spawn, 5 Makeup

	CustomizeSlots cfg_Customize_SaveSlots[3];

	//std::vector<int> cfg_Hotswap_Presets = { -1, -1, -1, -1, -1, -1 };
	//bool		cfg_Aim_NoSpread = false;
	//bool		cfg_Aim_NoRecoil = false;
	//bool		cfg_Mission_EnableAutoInstantInfiltration = false;

	void LoadCFG()
	{
		ini.SetUnicode();
		if (ini.LoadFile("cfg.ini") < 0)
		{
			std::cout << "No cfg, making new\n";
			try
			{
				SaveCFG();
			}
			catch (...)
			{
				std::cout << "cfg fucked up\n";
			}
		}
		else
		{
			LoadValue("Actors", "EnableESP", cfg_Actors_EnableESP);
			LoadValue("Actors", "DrawPlayerNames", cfg_Actors_DrawPlayerNames);
			LoadValue("Actors", "DrawPlayerBoxes", cfg_Actors_DrawPlayerBoxes);
			LoadValue("Actors", "DrawPlayerCircles", cfg_Actors_DrawPlayerCircles);
			LoadValue("Actors", "DrawPlayerLines", cfg_Actors_DrawPlayerLines);
			LoadValue("Actors", "DrawEnemyNames", cfg_Actors_DrawEnemyNames);
			LoadValue("Actors", "DrawEnemyBoxes", cfg_Actors_DrawEnemyBoxes);
			LoadValue("Actors", "DrawEnemyCircles", cfg_Actors_DrawEnemyCircles);
			LoadValue("Actors", "DrawEnemyLines", cfg_Actors_DrawEnemyLines);
			LoadValue("Actors", "ShapeSize", cfg_ESP_ShapeSize);
			LoadValue("Actors", "ShapeDistanceScale", cfg_ESP_ShapeDistanceScale);

			LoadValue("Loot", "EnableItemESP", cfg_Loot_EnableItemESP);
			LoadValue("Loot", "ItemFilterLevel", cfg_Loot_ItemFilterLevel);
			LoadValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemBoxes);
			LoadValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemCircles);
			LoadValue("Loot", "DrawItemNames", cfg_Loot_DrawItemNames);
			//LoadValue("Loot", "DrawItemLines", cfg_Loot_DrawItemLines);
			LoadValue("Loot", "DrawVaults", cfg_Loot_DrawVaults);
			LoadValue("Loot", "DrawSupplyResources", cfg_Loot_DrawSupplyResources);
			LoadValue("Loot", "DrawVoidVesselBox", cfg_Loot_DrawVoidVesselBox);
			LoadValue("Loot", "EnableLootVacuum", cfg_Loot_EnableLootVacuum);
			LoadValue("Loot", "LootVacuumRange", cfg_Loot_LootVacuumRange);
			LoadValue("Loot", "LootVacuumKey", cfg_Loot_LootVacuumKey);
			LoadValue("Loot", "SpawnLootKey", cfg_Loot_SpawnLootKey);
			LoadValue("Loot", "SpawnVaultRewardKey", cfg_Loot_SpawnVaultRewardKey);
			LoadValue("Loot", "HPThreshold", cfg_Loot_DrawHPThreshold);
			LoadValue("Loot", "MPThreshold", cfg_Loot_DrawMPThreshold);
			LoadValue("Loot", "ContainerDropRange", cfg_Loot_ContainerDropRange);

			LoadValue("Aimbot", "EnableAimbot", cfg_Aim_EnableAimbot);
			LoadValue("Aimbot", "EnableAimbotHold", cfg_Aim_EnableAimbotHold);
			LoadValue("Aimbot", "EnableAimbotToggle", cfg_Aim_EnableAimbotToggle);
			LoadValue("Aimbot", "AimbotHoldKey", cfg_Aim_AimbotHoldKey);
			LoadValue("Aimbot", "AimbotToggleKey", cfg_Aim_AimbotToggleKey);
			LoadValue("Aimbot", "AimbotUseController", cfg_Aim_AimbotUseController);
			LoadValue("Aimbot", "AimbotDistance", cfg_Aim_AimbotFOV);
			LoadValue("Aimbot", "AimbotSpeed", cfg_Aim_AimbotSpeed);
			LoadValue("Aimbot", "AimbotLineOfSight", cfg_Aim_AimbotLineOfSight);
			//LoadValue("Aimbot", "NoSpread", cfg_Aim_NoSpread);
			//LoadValue("Aimbot", "NoRecoil", cfg_Aim_NoRecoil);
			LoadValue("Aimbot", "NoReload", cfg_Aim_NoReload);
			LoadValue("Aimbot", "NoRecoilAndSpread", cfg_Aim_NoRecoilAndSpread);
			LoadValue("Aimbot", "RapidFire", cfg_Aim_RapidFire);

			LoadValue("Abilities", "EnableModifyGrapple", cfg_Abilities_EnableModifyGrapple);
			LoadValue("Abilities", "GrappleRange", cfg_Abilities_GrappleRange);
			LoadValue("Abilities", "AutoRestock", cfg_Abilities_AutoRestock);
			LoadValue("Abilities", "ResetCooldowns", cfg_Abilities_ResetCooldowns);
			LoadValue("Abilities", "ResetCooldownsKey", cfg_Abilities_ResetCooldownsKey);
			LoadValue("Abilities", "Ability1Key", cfg_Abilities_Ability1Key);
			LoadValue("Abilities", "Ability2Key", cfg_Abilities_Ability2Key);
			LoadValue("Abilities", "Ability3Key", cfg_Abilities_Ability3Key);
			LoadValue("Abilities", "Ability4Key", cfg_Abilities_Ability4Key);

			LoadValue("Extra", "TimeScale", cfg_Extra_TimeScale);
			LoadValue("Extra", "TimeScaleKey", cfg_Extra_TimeScaleKey);
			LoadValue("Extra", "TimeScaleHoldKey", cfg_Extra_TimeScaleHoldKey);

			LoadValue("Missions", "EnableMissionESP", cfg_Mission_EnableMissionESP);
			LoadValue("Missions", "EnableMissionAutoRestart", cfg_Mission_EnableMissionAutoRestart);
			LoadValue("Missions", "MissionAutoRestartDelay", cfg_Mission_MissionAutoRestartDelay);
			LoadValue("Missions", "MissionAutoRestartKey", cfg_Mission_MissionAutoRestartKey);
			LoadValue("Missions", "MissionTeleportKey", cfg_Mission_MissionTeleportKey);

			LoadValue("Hotswap", "EnableOverlay", cfg_Hotswap_EnableOverlay);
			LoadValue("Hotswap", "PresetSelectKey", cfg_Hotswap_PresetSelectKey);
			LoadValue("Style", "Index", Menu::CurrentStyleIndex);
			Menu::ApplySelectedStyle(Menu::CurrentStyleIndex);
			//LoadValue("Hotswap", "Preset_1", cfg_Hotswap_Presets[0]);
			//LoadValue("Hotswap", "Preset_2", cfg_Hotswap_Presets[1]);
			//LoadValue("Hotswap", "Preset_3", cfg_Hotswap_Presets[2]);
			//LoadValue("Hotswap", "Preset_4", cfg_Hotswap_Presets[3]);
			//LoadValue("Hotswap", "Preset_5", cfg_Hotswap_Presets[4]);
			//LoadValue("Hotswap", "Preset_6", cfg_Hotswap_Presets[5]);

			LoadValue("Customize", "EnableCustomizationBypass", cfg_Customize_EnableCustomizationBypass);
			LoadValue("Customize", "EnableAutoApplyCustomization", cfg_Customize_EnableAutoApplyCustomization);

			LoadValue("Customize", "SaveSlot0_CharacterID", cfg_Customize_SaveSlots[0].CharacterID);
			if (cfg_Customize_SaveSlots[0].CharacterID != -1)
			{
				LoadValue("Customize", "SaveSlot0_Name", cfg_Customize_SaveSlots[0].CharacterName);
				LoadValue("Customize", "SaveSlot0_Head", cfg_Customize_SaveSlots[0].Head);
				LoadValue("Customize", "SaveSlot0_Body", cfg_Customize_SaveSlots[0].Body);
				LoadValue("Customize", "SaveSlot0_Back", cfg_Customize_SaveSlots[0].Back);
				LoadValue("Customize", "SaveSlot0_Chest", cfg_Customize_SaveSlots[0].Chest);
				LoadValue("Customize", "SaveSlot0_Spawn", cfg_Customize_SaveSlots[0].Spawn);
				LoadValue("Customize", "SaveSlot0_Makeup", cfg_Customize_SaveSlots[0].Makeup);
			}
			LoadValue("Customize", "SaveSlot1_CharacterID", cfg_Customize_SaveSlots[1].CharacterID);
			if (cfg_Customize_SaveSlots[1].CharacterID != -1)
			{
				LoadValue("Customize", "SaveSlot1_Name", cfg_Customize_SaveSlots[1].CharacterName);
				LoadValue("Customize", "SaveSlot1_Head", cfg_Customize_SaveSlots[1].Head);
				LoadValue("Customize", "SaveSlot1_Body", cfg_Customize_SaveSlots[1].Body);
				LoadValue("Customize", "SaveSlot1_Back", cfg_Customize_SaveSlots[1].Back);
				LoadValue("Customize", "SaveSlot1_Chest", cfg_Customize_SaveSlots[1].Chest);
				LoadValue("Customize", "SaveSlot1_Spawn", cfg_Customize_SaveSlots[1].Spawn);
				LoadValue("Customize", "SaveSlot1_Makeup", cfg_Customize_SaveSlots[1].Makeup);
			}
			LoadValue("Customize", "SaveSlot2_CharacterID", cfg_Customize_SaveSlots[2].CharacterID);
			if (cfg_Customize_SaveSlots[2].CharacterID != -1)
			{
				LoadValue("Customize", "SaveSlot2_Name", cfg_Customize_SaveSlots[2].CharacterName);
				LoadValue("Customize", "SaveSlot2_Head", cfg_Customize_SaveSlots[2].Head);
				LoadValue("Customize", "SaveSlot2_Body", cfg_Customize_SaveSlots[2].Body);
				LoadValue("Customize", "SaveSlot2_Back", cfg_Customize_SaveSlots[2].Back);
				LoadValue("Customize", "SaveSlot2_Chest", cfg_Customize_SaveSlots[2].Chest);
				LoadValue("Customize", "SaveSlot2_Spawn", cfg_Customize_SaveSlots[2].Spawn);
				LoadValue("Customize", "SaveSlot2_Makeup", cfg_Customize_SaveSlots[2].Makeup);
			}
		}
	}

	void SaveCFG()
	{

		SaveValue("Actors", "EnableESP", cfg_Actors_EnableESP);
		SaveValue("Actors", "DrawPlayerNames", cfg_Actors_DrawPlayerNames);
		SaveValue("Actors", "DrawPlayerBoxes", cfg_Actors_DrawPlayerBoxes);
		SaveValue("Actors", "DrawPlayerCircles", cfg_Actors_DrawPlayerCircles);
		SaveValue("Actors", "DrawPlayerLines", cfg_Actors_DrawPlayerLines);
		SaveValue("Actors", "DrawEnemyNames", cfg_Actors_DrawEnemyNames);
		SaveValue("Actors", "DrawEnemyBoxes", cfg_Actors_DrawEnemyBoxes);
		SaveValue("Actors", "DrawEnemyCircles", cfg_Actors_DrawEnemyCircles);
		SaveValue("Actors", "DrawEnemyLines", cfg_Actors_DrawEnemyLines);
		SaveValue("Actors", "ShapeSize", cfg_ESP_ShapeSize);
		SaveValue("Actors", "ShapeDistanceScale", cfg_ESP_ShapeDistanceScale);

		SaveValue("Loot", "EnableItemESP", cfg_Loot_EnableItemESP);
		SaveValue("Loot", "ItemFilterLevel", cfg_Loot_ItemFilterLevel);
		SaveValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemBoxes);
		SaveValue("Loot", "DrawItemBoxes", cfg_Loot_DrawItemCircles);
		SaveValue("Loot", "DrawItemNames", cfg_Loot_DrawItemNames);
		//SaveValue("Loot", "DrawItemLines", cfg_Loot_DrawItemLines);
		SaveValue("Loot", "DrawVaults", cfg_Loot_DrawVaults);
		SaveValue("Loot", "DrawSupplyResources", cfg_Loot_DrawSupplyResources);
		SaveValue("Loot", "DrawVoidVesselBox", cfg_Loot_DrawVoidVesselBox);
		SaveValue("Loot", "EnableLootVacuum", cfg_Loot_EnableLootVacuum);
		SaveValue("Loot", "LootVacuumRange", cfg_Loot_LootVacuumRange);
		SaveValue("Loot", "LootVacuumKey", cfg_Loot_LootVacuumKey);
		SaveValue("Loot", "SpawnLootKey", cfg_Loot_SpawnLootKey);
		SaveValue("Loot", "SpawnVaultRewardKey", cfg_Loot_SpawnVaultRewardKey);
		SaveValue("Loot", "HPThreshold", cfg_Loot_DrawHPThreshold);
		SaveValue("Loot", "MPThreshold", cfg_Loot_DrawMPThreshold);
		SaveValue("Loot", "ContainerDropRange", cfg_Loot_ContainerDropRange);
		


		SaveValue("Aimbot", "EnableAimbot", cfg_Aim_EnableAimbot);
		SaveValue("Aimbot", "EnableAimbotHold", cfg_Aim_EnableAimbotHold);
		SaveValue("Aimbot", "EnableAimbotToggle", cfg_Aim_EnableAimbotToggle);
		SaveValue("Aimbot", "AimbotHoldKey", cfg_Aim_AimbotHoldKey);
		SaveValue("Aimbot", "AimbotToggleKey", cfg_Aim_AimbotToggleKey);
		SaveValue("Aimbot", "AimbotUseController", cfg_Aim_AimbotUseController);
		SaveValue("Aimbot", "AimbotDistance", cfg_Aim_AimbotFOV);
		SaveValue("Aimbot", "AimbotSpeed", cfg_Aim_AimbotSpeed);
		SaveValue("Aimbot", "AimbotLineOfSight", cfg_Aim_AimbotLineOfSight);
		//SaveValue("Aimbot", "NoSpread", cfg_Aim_NoSpread);
		//SaveValue("Aimbot", "NoRecoil", cfg_Aim_NoRecoil);
		SaveValue("Aimbot", "NoReload", cfg_Aim_NoReload);
		SaveValue("Aimbot", "NoRecoilAndSpread", cfg_Aim_NoRecoilAndSpread);
		SaveValue("Aimbot", "RapidFire", cfg_Aim_RapidFire);

		SaveValue("Abilities", "EnableModifyGrapple", cfg_Abilities_EnableModifyGrapple);
		SaveValue("Abilities", "GrappleRange", cfg_Abilities_GrappleRange);
		SaveValue("Abilities", "AutoRestock", cfg_Abilities_AutoRestock);
		SaveValue("Abilities", "ResetCooldowns", cfg_Abilities_ResetCooldowns);
		SaveValue("Abilities", "ResetCooldownsKey", cfg_Abilities_ResetCooldownsKey);
		SaveValue("Abilities", "Ability1Key", cfg_Abilities_Ability1Key);
		SaveValue("Abilities", "Ability2Key", cfg_Abilities_Ability2Key);
		SaveValue("Abilities", "Ability3Key", cfg_Abilities_Ability3Key);
		SaveValue("Abilities", "Ability4Key", cfg_Abilities_Ability4Key);

		SaveValue("Extra", "TimeScale", cfg_Extra_TimeScale);
		SaveValue("Extra", "TimeScaleKey", cfg_Extra_TimeScaleKey);
		SaveValue("Extra", "TimeScaleHoldKey", cfg_Extra_TimeScaleHoldKey);

		SaveValue("Missions", "EnableMissionESP", cfg_Mission_EnableMissionESP);
		SaveValue("Missions", "EnableMissionAutoRestart", cfg_Mission_EnableMissionAutoRestart);
		SaveValue("Missions", "MissionAutoRestartDelay", cfg_Mission_MissionAutoRestartDelay);
		SaveValue("Missions", "MissionAutoRestartKey", cfg_Mission_MissionAutoRestartKey);
		SaveValue("Missions", "MissionTeleportKey", cfg_Mission_MissionTeleportKey);

		SaveValue("Hotswap", "EnableOverlay", cfg_Hotswap_EnableOverlay);
		SaveValue("Hotswap", "PresetSelectKey", cfg_Hotswap_PresetSelectKey);
		SaveValue("Style", "Index", Menu::CurrentStyleIndex);
		//SaveValue("Hotswap", "Preset_1", cfg_Hotswap_Presets[0]);
		//SaveValue("Hotswap", "Preset_2", cfg_Hotswap_Presets[1]);
		//SaveValue("Hotswap", "Preset_3", cfg_Hotswap_Presets[2]);
		//SaveValue("Hotswap", "Preset_4", cfg_Hotswap_Presets[3]);
		//SaveValue("Hotswap", "Preset_5", cfg_Hotswap_Presets[4]);
		//SaveValue("Hotswap", "Preset_6", cfg_Hotswap_Presets[5]);

		SaveValue("Customize", "EnableCustomizationBypass", cfg_Customize_EnableCustomizationBypass);
		SaveValue("Customize", "EnableAutoApplyCustomization", cfg_Customize_EnableAutoApplyCustomization);

		SaveValue("Customize", "SaveSlot0_CharacterID", cfg_Customize_SaveSlots[0].CharacterID);
		SaveValue("Customize", "SaveSlot0_Name", cfg_Customize_SaveSlots[0].CharacterName);
		SaveValue("Customize", "SaveSlot0_Head", cfg_Customize_SaveSlots[0].Head);
		SaveValue("Customize", "SaveSlot0_Body", cfg_Customize_SaveSlots[0].Body);
		SaveValue("Customize", "SaveSlot0_Back", cfg_Customize_SaveSlots[0].Back);
		SaveValue("Customize", "SaveSlot0_Chest", cfg_Customize_SaveSlots[0].Chest);
		SaveValue("Customize", "SaveSlot0_Spawn", cfg_Customize_SaveSlots[0].Spawn);
		SaveValue("Customize", "SaveSlot0_Makeup", cfg_Customize_SaveSlots[0].Makeup);
		SaveValue("Customize", "SaveSlot1_CharacterID", cfg_Customize_SaveSlots[1].CharacterID);
		SaveValue("Customize", "SaveSlot1_Name", cfg_Customize_SaveSlots[1].CharacterName);
		SaveValue("Customize", "SaveSlot1_Head", cfg_Customize_SaveSlots[1].Head);
		SaveValue("Customize", "SaveSlot1_Body", cfg_Customize_SaveSlots[1].Body);
		SaveValue("Customize", "SaveSlot1_Back", cfg_Customize_SaveSlots[1].Back);
		SaveValue("Customize", "SaveSlot1_Chest", cfg_Customize_SaveSlots[1].Chest);
		SaveValue("Customize", "SaveSlot1_Spawn", cfg_Customize_SaveSlots[1].Spawn);
		SaveValue("Customize", "SaveSlot1_Makeup", cfg_Customize_SaveSlots[1].Makeup);
		SaveValue("Customize", "SaveSlot2_CharacterID", cfg_Customize_SaveSlots[2].CharacterID);
		SaveValue("Customize", "SaveSlot2_Name", cfg_Customize_SaveSlots[2].CharacterName);
		SaveValue("Customize", "SaveSlot2_Head", cfg_Customize_SaveSlots[2].Head);
		SaveValue("Customize", "SaveSlot2_Body", cfg_Customize_SaveSlots[2].Body);
		SaveValue("Customize", "SaveSlot2_Back", cfg_Customize_SaveSlots[2].Back);
		SaveValue("Customize", "SaveSlot2_Chest", cfg_Customize_SaveSlots[2].Chest);
		SaveValue("Customize", "SaveSlot2_Spawn", cfg_Customize_SaveSlots[2].Spawn);
		SaveValue("Customize", "SaveSlot2_Makeup", cfg_Customize_SaveSlots[2].Makeup);

		ini.SaveFile("cfg.ini");
	}


	template <typename T>
	void SaveValue(const char* Section, const char* Name, T &Value)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			ini.SetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			ini.SetBoolValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			ini.SetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			ini.SetValue(Section, Name, std::string(Value).c_str());
		}
	}

	template <typename T>
	void LoadValue(const char* Section, const char* Name, T& Value)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			Value = (int)ini.GetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			Value = ini.GetBoolValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			Value = (float)ini.GetDoubleValue(Section, Name, Value);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			Value = std::string(ini.GetValue(Section, Name, "None"));
		}
	}
}
