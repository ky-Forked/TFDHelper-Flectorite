#include "cheat.h"
#include "magic_enum/magic_enum.hpp"
#include <random>


//#define IS_DEBUG_VERSION




namespace Cheat
{
#define IsKeyPressed(key) ([](int k){ SHORT s = GetAsyncKeyState(k); return (s & 0x8001) == 0x8001; })(key)
#define IsKeyHeld(key) (GetAsyncKeyState(key) & 0x8000)

	__int64 (*oPostRender)(void* ViewportClient, void* DebugCanvas) = nullptr;

	bool UpdateCache = false;
	float BoundsScale = 1.0f;

	CONTROLLER_STATE g_Controllers[4];

	SDK::FVector2D ScreenMiddle = { 0, 0 };

	CSimpleIniA ini;

	TFD::UWorld* GWorld = nullptr;
	TFD::AM1PlayerControllerInGame* LocalPlayerController = nullptr;
	TFD::AM1Player* LocalPlayerCharacter = nullptr;
	TFD::UCanvas* Canvas = nullptr;
	TFD::UM1MissionControlComponent* Missions = nullptr;
	TFD::AActor* Aimbot_Target = nullptr;
	int Aimbot_BoneIndex = 0;
	TFD::UM1WeaponSlotControlComponent* WeaponSlot = nullptr;


	TFD::ANPC_MultiSupply_C* ResupplyNPC = nullptr;

	SDK::FLinearColor ColorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
	SDK::FLinearColor ColorRed = { 1.0f, 0, 0, 1.0f };
	SDK::FLinearColor ColorDarkRed = { 0.5f, 0.0f, 0.0f, 0.8f };
	SDK::FLinearColor ColorGreen = { 0, 1.0f, 0, 1.0f };
	SDK::FLinearColor ColorMana = { 0.184f, 0.702f, 1.0f, 1.0f };
	SDK::FLinearColor ColorCommon = { 0, 0.922f, 0.29f, 1.0f };
	SDK::FLinearColor ColorUncommon = { 0.2f, 0.29f, 1.0f, 1.0f };
	SDK::FLinearColor ColorRare = { 0.761f, 0.063f, 0.961, 1.0f };
	SDK::FLinearColor ColorUltimate = { 1.0f, 0.247f, 0.247f, 1.0f };
	SDK::FLinearColor ColorGold = { 1.0f, 0.882f, 0, 1.0f };

	std::unordered_map<int, std::vector<int>> IDBoneMap = { };
	std::unordered_map<int, std::string> IDNameMap = { };
	std::unordered_map<int, std::string> PresetsMap = { };
	int CurrentPresetIndex = 0;
	std::vector<int> HotSwapPreset = { -1, -1, -1, -1, -1, -1 };
	int HotSwapIndex = 0;

	TFD::FVector RotatorToVector(const TFD::FRotator& R)
	{
		float CP = std::cos(R.Pitch * 3.14159265f / 180.f);
		float SP = std::sin(R.Pitch * 3.14159265f / 180.f);
		float CY = std::cos(R.Yaw * 3.14159265f / 180.f);
		float SY = std::sin(R.Yaw * 3.14159265f / 180.f);

		return TFD::FVector{ CP * CY, CP * SY, SP };
	}

	bool TrySpawnGenericLoot = false;
	bool TrySpawnVaultLoot = false;
	bool RestartDecoding = false;
	bool TryAddAllItems = false;
	bool TryResetAbilities = false;

	__int64 PostRender(void* self, void* canvas)
	{
		//std::cout << "PostRender called" << std::endl;
		//return oPostRender(self, canvas);

		static bool Init = false;
		if (!Init)
		{
			//std::cout << "PostRender called once." << std::endl;
			Canvas = static_cast<TFD::UCanvas*>(canvas);
			LoadCacheData();
			//ToggleMemFeature(0);
			//ToggleMemFeature(1);
			//ToggleMemFeature(2);
			Init = true;
		}

		if (!IsValidUWorld())
		{
			Render::R_End();
			return oPostRender(self, canvas);
		}

		if (!IsCheatReady())
		{
			Render::R_End();
			return oPostRender(self, canvas);
		}

		static bool TryEquipCustomization = true;
		if ((int)(static_cast<TFD::UM1GameInstance*>(GWorld->OwningGameInstance)->ConnectionState) != 10)
		{
			Render::R_End();
			WeaponSlot = nullptr;
			//ResupplyNPC = nullptr;
			if (CFG::cfg_Customize_EnableAutoApplyCustomization && TryEquipCustomization == false)
				TryEquipCustomization = true;
			return oPostRender(self, canvas);
		}

		if (CFG::cfg_Customize_EnableAutoApplyCustomization && TryEquipCustomization)
		{

			if (TryEquipSavedCustomization())
				TryEquipCustomization = false;
		}

		if (TryAddAllItems)
		{
			TryAddAllItems = false;
			AddAllCustomizationItems();
		}

		ScreenMiddle.X = (Canvas->SizeX / 2.0f) / (Canvas->SizeX / Canvas->ClipX);
		ScreenMiddle.Y = (Canvas->SizeY / 2.0f) / (Canvas->SizeY / Canvas->ClipY);
		static bool PresetFirstCheck = true;
		if (PresetFirstCheck && PresetsMap.empty())
		{
			PresetFirstCheck = false;
			//PresetRefresh();
		}
		UpdateControllerState();

		Menu::HandleKeybinds();

		if (UpdateCache)
		{
			SaveCacheData();
			UpdateCache = false;
		}

		if (CFG::cfg_Actors_EnableESP)
			PlayerEnemyESP();

		if (CFG::cfg_Mission_EnableMissionESP)
			MissionESPTeleport();

		if (CFG::cfg_Mission_EnableMissionAutoRestart)
			AutoRestartMission();

		if (CFG::cfg_Loot_EnableItemESP || CFG::cfg_Loot_EnableLootVacuum || TrySpawnGenericLoot || TrySpawnVaultLoot || RestartDecoding)
			Loot();

		if (CFG::cfg_Aim_EnableAimbot)
			Aimbot();

		if (CFG::cfg_Abilities_EnableModifyGrapple)
			ModifyGrapple();

		if (CFG::cfg_Aim_NoRecoilAndSpread || CFG::cfg_Aim_NoReload || CFG::cfg_Aim_RapidFire)
			WeaponModifications();

		if (PresetsMap.empty())
			PresetRefresh();

		if (CFG::cfg_Hotswap_EnableOverlay)
			PresetOverlay();

		if (CFG::cfg_Mission_EnableInstantInfiltration)
			InstantInfiltration();

		if (CFG::cfg_Abilities_EnableAutomaticResupply || CFG::cfg_Abilities_EnableAutomaticResupplyTimed)
			AutoResupply();

		Render::R_End();
		return oPostRender(self, canvas);
	}

	bool IsValidUWorld()
	{
		if (GWorld == nullptr)
		{
#ifdef IS_DEBUG_VERSION
			std::cout << "[Cheat] World is null.\n";
#endif
			GWorld = *reinterpret_cast<TFD::UWorld**>(TFD::BASE + SDK::Offsets::GWorld);
			if (GWorld && GWorld->IsA(TFD::UWorld::StaticClass()))
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] Found World\n";
#endif
				return true;
			}
			else
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] World is null and GWorld not pointing to valid UWorld.\n";
#endif
				GWorld = nullptr;
				return false;
			}
		}
		else
		{
			TFD::UWorld* CheckWorld = *reinterpret_cast<TFD::UWorld**>(TFD::BASE + SDK::Offsets::GWorld);
			if (!CheckWorld)
			{
#ifdef IS_DEBUG_VERSION
				std::cout << "[Cheat] World was not null but GWorld not currently pointing to valid UWorld.\n";
#endif
				GWorld = nullptr;
				return false;
			}
			if (CheckWorld->IsA(TFD::UWorld::StaticClass()))
			{
				if (GWorld != CheckWorld)
				{
#ifdef IS_DEBUG_VERSION
					std::cout << "[Cheat] World is not null and GWorld is pointing to different UWorld.\n";
#endif
					GWorld = CheckWorld;
				}
				return true;
			}
		}

		return false;
	}

	bool IsCheatReady()
	{
		std::string Name = GWorld->Name.ToString();
		if (Name != "" && Name != "None")
		{
			if (Name != "Lobby_P" && Name != "Level_Transition")
			{
				if (GWorld->OwningGameInstance && GWorld->OwningGameInstance->IsA(TFD::UM1GameInstance::StaticClass()))
				{
					if (GWorld->OwningGameInstance->LocalPlayers && GWorld->OwningGameInstance->LocalPlayers[0] && GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController)
					{
						if (GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController->IsA(TFD::AM1PlayerControllerInGame::StaticClass()))
						{
							TFD::AM1PlayerControllerInGame* PC = static_cast<TFD::AM1PlayerControllerInGame*>(GWorld->OwningGameInstance->LocalPlayers[0]->PlayerController);
							if (PC->Character && PC->Character->IsA(TFD::AM1Player::StaticClass()) && PC->ActorManager_Subsystem && PC->ActorManager_Subsystem->IsA(TFD::UM1ActorManagerSubsystem::StaticClass()))
							{
								LocalPlayerController = PC;
								LocalPlayerCharacter = static_cast<TFD::AM1Player*>(PC->Character);
								if (PC->PlayerState && PC->PlayerState->IsA(TFD::AM1PlayerState::StaticClass()))
									Missions = static_cast<TFD::AM1PlayerState*>(PC->PlayerState)->MissionControlComponent;
								else
									Missions = nullptr;
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}



	bool WorldToScreen(const SDK::FVector& worldLoc, SDK::FVector2D* screenPos)
	{
		bool inView = TFD::UGameplayStatics::ProjectWorldToScreen(LocalPlayerController, worldLoc, screenPos, false);
		if (inView && screenPos->X > 0 && screenPos->X < Canvas->SizeX && screenPos->Y > 0 && screenPos->Y < Canvas->SizeY)
			return true;
		return false;
	}

	void PlayerEnemyESP()
	{
		TFD::UM1ActorManagerSubsystem* Actors = LocalPlayerController->ActorManager_Subsystem;
		if (Actors->Characters.IsValid() && Actors->Characters.Num() > 0)
		{
			int StartNumber = Actors->Characters.Num();
			for (int i = 0; i < Actors->Characters.Num(); i++)
			{
				if (!GWorld || !LocalPlayerController || !LocalPlayerController->ActorManager_Subsystem || !Actors || !Actors->Characters)
					return;
				if (Actors->Characters.Num() != StartNumber)
					return;
				if (!Actors->Characters.IsValidIndex(i))
					continue;
				TFD::AM1Character* p = Actors->Characters[i];
				if (p)
				{
					if (p == LocalPlayerCharacter)
						continue;
					if (p->IsDead())
						continue;
					if (p->IsA(TFD::AM1Player::StaticClass()))
					{
						TFD::AM1Player* player = static_cast<TFD::AM1Player*>(p);
						SDK::FVector2D ScreenPos = { -1, -1 };
						if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
						{
							if (CFG::cfg_Actors_DrawPlayerNames)
							{
								if (player && player->PlayerName)
								{
									std::string playerNameStr = player->PlayerName.ToString();
									if (!playerNameStr.empty())
									{
										// Debug logging
										//std::cout << "before R_DrawText" << std::endl;
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, player->PlayerName, &ColorGreen);
										//std::cout << "after R_DrawText" << std::endl;
									}
								}
							}
							if (CFG::cfg_Actors_DrawPlayerBoxes)
							{
								if (player->Mesh && player->Mesh->SkeletalMesh)
								{
									float ODistance = player->GetDistanceTo(LocalPlayerController->Pawn) / CFG::cfg_ESP_ShapeDistanceScale;
									if (ODistance > 0)
									{
										float BoxSize = CFG::cfg_ESP_ShapeSize / ODistance;
										BoxSize = std::clamp(BoxSize, 8.0f, 80.0f);

										Render::R_DrawRect(
											ScreenPos.X - BoxSize / 1.5f,
											ScreenPos.Y - BoxSize,
											ScreenPos.X + BoxSize / 1.5f,
											ScreenPos.Y + BoxSize,
											&ColorGreen);

									}
								}
							}
							if (CFG::cfg_Actors_DrawPlayerCircles)
							{
								if (player->Mesh && player->Mesh->SkeletalMesh)
								{
									float ODistance = player->GetDistanceTo(LocalPlayerController->Pawn) / CFG::cfg_ESP_ShapeDistanceScale;
									if (ODistance > 0)
									{
										float Radius = CFG::cfg_ESP_ShapeSize / ODistance;
										Radius = std::clamp(Radius, 4.0f, 60.0f);
										Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, Radius, &ColorGreen);
									}
								}
							}
							if (CFG::cfg_Actors_DrawPlayerLines)
							{
								if (player->Mesh && player->Mesh->SkeletalMesh)
								{
									SDK::FVector2D LocalScreenPos;
									if (WorldToScreen(LocalPlayerCharacter->K2_GetActorLocation(), &LocalScreenPos))
									{
										Render::R_DrawLine(LocalScreenPos.X, LocalScreenPos.Y, ScreenPos.X, ScreenPos.Y, &ColorGreen);
									}

								}
							}
						}

					}
					else if (p->IsA(TFD::AM1Monster::StaticClass()) || p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass()))
					{
						//TFD::AM1Monster* monster = static_cast<TFD::AM1Monster*>(p);
						SDK::FVector2D ScreenPos = { -1, -1 };
						if (WorldToScreen(p->K2_GetActorLocation(), &ScreenPos))
						{
							/*SDK::FLinearColor color = ColorDarkRed;
							if (LocalPlayerController->LineOfSightTo(p, SDK::FVector{ 0, 0, 0 }, false))
								color = ColorRed;*/

							if (CFG::cfg_Actors_DrawEnemyNames)
							{
								if (!IDNameMap.contains(p->CharacterId.ID))
								{
									static TFD::FString CharacterName;
									CharacterName = *TFD::native_GetCharacterName(p, &CharacterName);
									std::string fmtName = CharacterName.ToString();
									fmtName = fmtName.substr(fmtName.find_last_of("_") + 1);
									int id = p->CharacterId.ID;
									IDNameMap.insert({ id, fmtName });
									UpdateCache = true;
								}
								else
								{
									Render::R_DrawText(ScreenPos.X, ScreenPos.Y, IDNameMap[(int)p->CharacterId.ID], &ColorRed);
								}
							}
							if (CFG::cfg_Actors_DrawEnemyBoxes)
							{
								if (p->Mesh && p->Mesh->SkeletalMesh)
								{
									float ODistance = p->GetDistanceTo(LocalPlayerController->Pawn) / CFG::cfg_ESP_ShapeDistanceScale;
									if (ODistance > 0)
									{
										float BoxSize = CFG::cfg_ESP_ShapeSize / ODistance;
										BoxSize = std::clamp(BoxSize, 8.0f, 80.0f);

										Render::R_DrawRect(
											ScreenPos.X - BoxSize / 1.5f,
											ScreenPos.Y - BoxSize,
											ScreenPos.X + BoxSize / 1.5f,
											ScreenPos.Y + BoxSize,
											&ColorRed);
									}
								}
							}
							if (CFG::cfg_Actors_DrawEnemyCircles)
							{
								if (p->Mesh && p->Mesh->SkeletalMesh)
								{
									float ODistance = p->GetDistanceTo(LocalPlayerController->Pawn) / CFG::cfg_ESP_ShapeDistanceScale;
									if (ODistance > 0)
									{
										float Radius = CFG::cfg_ESP_ShapeSize / ODistance;
										Radius = std::clamp(Radius, 4.0f, 60.0f);
										Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, Radius, &ColorRed);
									}
								}
							}
							if (CFG::cfg_Actors_DrawEnemyLines)
							{
								if (p->Mesh && p->Mesh->SkeletalMesh)
								{
									SDK::FVector2D LocalScreenPos;
									if (WorldToScreen(LocalPlayerCharacter->K2_GetActorLocation(), &LocalScreenPos))
									{
										Render::R_DrawLine(LocalScreenPos.X, LocalScreenPos.Y, ScreenPos.X, ScreenPos.Y, &ColorRed);
									}

								}
							}
						}
					}
				}
			}
		}
	}

	void MultiplyDrops(TFD::AActor* Actor, int Amount, bool isVault)
	{
		if (!Actor || !LocalPlayerController || Amount <= 0)
			return;

		TFD::FVector ActorPos = Actor->K2_GetActorLocation();
		TFD::FVector PlayerPos = LocalPlayerCharacter->K2_GetActorLocation();
		float Distance = ActorPos.GetDistanceTo(PlayerPos);

		if (Distance > CFG::cfg_Loot_ContainerDropRange)
			return;

		for (int i = 0; i < Amount; ++i)
		{
			if (isVault)
			{
				auto* Vault = static_cast<TFD::AM1MiniGameActor*>(Actor);
				Vault->ServerDropItems(LocalPlayerController);
			}
			else
			{
				auto* Generic = static_cast<TFD::AM1FieldInteractableActor*>(Actor);
				LocalPlayerController->ServerRequestFieldObjectDropItems(Generic);
			}
		}
	}


	void Loot()
	{
		if (GWorld->Levels.IsValid())
		{
			static TFD::UM1LocalGameInstanceSubsystem* GISystem = nullptr;
			for (int i = 0; i < GWorld->Levels.Num(); ++i)
			{
				if (GWorld->Levels.IsValidIndex(i))
				{
					TFD::ULevel* Level = GWorld->Levels[i];
					if (!Level) continue;

					for (int j = 0; j < Level->Actors.Num(); ++j)
					{
						TFD::AActor* Actor = (TFD::AActor*)Level->Actors[j];
						if (!Actor)
							continue;

						if (Actor->IsA(TFD::AStaticMeshActor::StaticClass()))
							continue;

						/*SDK::FVector2D ScreenPosb = { -1, -1 };
						SDK::FVector WorldPositionb = Actor->K2_GetActorLocation();
						if (WorldToScreen(WorldPositionb, &ScreenPosb))
						{
							Render::R_DrawText(ScreenPosb.X, ScreenPosb.Y, Actor->Class->GetName(), &ColorWhite, true);
						}*/

						/*if (Actor->IsA(TFD::AM1Npc::StaticClass()))
						{
							if (Actor->Class->GetName().contains("MultiSupply"))
							{
								SDK::FVector2D ScreenPosb = { -1, -1 };
								SDK::FVector WorldPositionb = Actor->K2_GetActorLocation();
								if (WorldToScreen(WorldPositionb, &ScreenPosb))
								{
									Render::R_DrawText(ScreenPosb.X, ScreenPosb.Y, Actor->Class->GetName(), &ColorGold, true);
								}
								ResupplyNPC = static_cast<TFD::ANPC_MultiSupply_C*>(Actor);
							}
							continue;
						}*/

						if (Actor->IsA(TFD::AM1MiniGameActor::StaticClass()))
						{
							GISystem = TFD::UM1LocalGameInstanceSubsystem::GetSystem(GWorld);
							if (GISystem && GISystem->MiniGamePlayer && GISystem->MiniGamePlayer->CurrentMiniGame)
							{
								TFD::AM1MiniGameActor* Vault = static_cast<TFD::AM1MiniGameActor*>(Actor);
								if (RestartDecoding)
								{
									Vault->ClientStartMiniGame(Vault->MiniGameTid, LocalPlayerCharacter->CharacterId, Vault->FieldDifficultyTid, TFD::EM1MiniGameDifficulty::Normal);
								}
								if (TrySpawnVaultLoot)
								{
									int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
									MultiplyDrops(Vault, Amount, true);
								}
							}
							continue;
						}
						if (CFG::cfg_Loot_DrawVaults)
						{
							if (Actor->IsA(TFD::AM1FieldInteractableActorMiniGame::StaticClass()))
							{
								SDK::FVector2D ScreenPosa = { -1, -1 };
								SDK::FVector WorldPositiona = Actor->K2_GetActorLocation();
								if (WorldToScreen(WorldPositiona, &ScreenPosa))
								{
									static std::string VaultText = "Coded Vault";
									Render::R_DrawText(ScreenPosa.X, ScreenPosa.Y, VaultText, &ColorGold, true);
								}
								continue;
							}
						}
						if (CFG::cfg_Loot_DrawSupplyResources)
						{
							if (Actor->IsA(TFD::AM1FieldInteractableActor_Hit::StaticClass()))
							{
								if (Actor->GetFullName().contains("VulgusBox"))
								{
									SDK::FVector2D ScreenPos = { -1, -1 };
									SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string RBText = "Resource Box";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, RBText, &ColorUncommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
							}
							else if (Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()))
							{
								if (Actor->GetFullName().contains("MilitarySupplies"))
								{
									SDK::FVector2D ScreenPos = { -1, -1 };
									SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string SPText = "Supplies";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, SPText, &ColorCommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
							}
						}

						if (CFG::cfg_Loot_DrawVoidVesselBox)
						{
							if (Actor->IsA(TFD::AM1FieldInteractableActor_Interaction::StaticClass()))
							{
								std::string aName = Actor->GetFullName();
								SDK::FVector2D ScreenPos = { -1, -1 };
								SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
								if (aName.contains("CompanionVault_TypeB_Item_C"))
								{
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string SPText = "Experimental Box";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, SPText, &ColorCommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
								if (aName.contains("CompanionVault_TypeA_Broken_C"))
								{
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string SPText = "Broken Box";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, SPText, &ColorCommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
								if (aName.contains("CompanionVault_TypeA_C"))
								{
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string SPText = "Special Box";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, SPText, &ColorCommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
								if (aName.contains("CompanionVault_TypeB_Data_C"))
								{
									if (WorldToScreen(WorldPosition, &ScreenPos))
									{
										static std::string SPText = "Genetic Box";
										Render::R_DrawText(ScreenPos.X, ScreenPos.Y, SPText, &ColorCommon, true);
									}

									TFD::FVector PlayerPosition = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(PlayerPosition);
									if (Distance <= CFG::cfg_Loot_ContainerDropRange && CFG::cfg_Loot_MultiplyDrops)
									{
										if (WorldToScreen(WorldPosition, &ScreenPos))
										{
											float ODistance = Distance;
											if (ODistance > 0)
											{
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 3.0f, &ColorWhite);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 5.0f, &ColorGreen);
												Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, 7.0f, &ColorMana);
											}
										}
									}

									if (TrySpawnGenericLoot)
									{
										int Amount = CFG::cfg_Loot_MultiplyDrops ? CFG::cfg_Loot_SpawnCount : 1;
										MultiplyDrops(Actor, Amount, false);
									}
									continue;
								}
							}
						}

						if (!Actor->IsA(TFD::AM1DroppedItem::StaticClass()))
							continue;

						TFD::AM1DroppedItem* Item = static_cast<TFD::AM1DroppedItem*>(Actor);
						if (Item->IsObtained() || Item->bBeingPickedLocally || Item->bTriedSetToObtained || Item->bObtainRequestedOnClient)
							continue;

						SDK::FVector2D ScreenPos = { -1, -1 };
						SDK::FVector WorldPosition = Actor->K2_GetActorLocation();
						bool OnScreen = WorldToScreen(WorldPosition, &ScreenPos);

						//if (OnScreen)
						//{
						//	Render::R_DrawText(ScreenPos.X, ScreenPos.Y, Actor->Class->GetFullName(), &ColorWhite, true);
						//}

						if (Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Equipment
							|| Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Consumable
							|| Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Rune)
						{
							bool isFiltered = true;
							SDK::FLinearColor color = ColorWhite;
							std::string Text = "Unknown";

							if (CFG::cfg_Loot_ItemFilterLevel == 1)
							{
								if (Item->IsA(TFD::ABP_EquipTier01DroppedItem_C::StaticClass()))
								{
									color = ColorUncommon;
									Text = "T1 Item";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_ResourceTier01DroppedItem_C::StaticClass()))
								{
									color = ColorUncommon;
									Text = "T1 Resource";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_RuneTier01DroppedItem_C::StaticClass()))
								{
									color = ColorUncommon;
									Text = "T1 Rune";
									isFiltered = false;
								}
							}
							if (CFG::cfg_Loot_ItemFilterLevel <= 2)
							{
								if (Item->IsA(TFD::ABP_EquipTier02DroppedItem_C::StaticClass()))
								{
									color = ColorRare;
									Text = "T2 Item";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_ResourceTier02DroppedItem_C::StaticClass()))
								{
									color = ColorRare;
									Text = "T2 Resource";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_RuneTier02DroppedItem_C::StaticClass()))
								{
									color = ColorRare;
									Text = "T2 Rune";
									isFiltered = false;
								}
							}
							if (CFG::cfg_Loot_ItemFilterLevel <= 3)
							{
								if (Item->IsA(TFD::ABP_EquipTier03DroppedItem_C::StaticClass()))
								{
									color = ColorGold;
									Text = "T3 Item";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_ResourceTier03DroppedItem_C::StaticClass()))
								{
									color = ColorGold;
									Text = "T3 Resource";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_RuneTier03DroppedItem_C::StaticClass()))
								{
									color = ColorGold;
									Text = "T3 Rune";
									isFiltered = false;
								}
								else if (Item->IsA(TFD::ABP_RuneTier04DroppedItem_C::StaticClass()))
								{
									color = ColorUltimate;
									Text = "T4 Rune";
									isFiltered = false;
								}
							}


							if (!isFiltered)
							{
								if (OnScreen && CFG::cfg_Loot_DrawItemNames)
								{
									Render::R_DrawText(ScreenPos.X, ScreenPos.Y, Text, &color);
								}

								if (OnScreen && CFG::cfg_Loot_DrawItemBoxes)
								{
									float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
									if (ODistance > 0)
									{
										float Width = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
										float Height = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
										Render::R_DrawRect(ScreenPos.X - Width, ScreenPos.Y - Height, ScreenPos.X + Width, ScreenPos.Y + Height, &color);
									}
								}

								if (OnScreen && CFG::cfg_Loot_DrawItemCircles)
								{
									float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
									if (ODistance > 0)
									{
										float Radius = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
										Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, Radius, &color);
									}
								}

								if (CFG::cfg_Loot_EnableLootVacuum)
								{
									SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
									float Distance = WorldPosition.GetDistanceTo(player);
									if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
										Item->K2_SetActorLocation(player, false, nullptr, true);
								}

							}

						}
						else if (Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::InstantUse)
						{
							float hp = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentHp).Value / 10000.0f;
							float maxhp = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxHp).Value / 10000.0f;
							float mana = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentMp).Value / 10000.0f;
							float maxmana = (float)Cheat::LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxMp).Value / 10000.0f;
							bool shouldVacuum = false;
							SDK::FLinearColor color = ColorUltimate;
							std::string Text = "Unknown InstantUse";

							if (Item->IsA(TFD::ABP_HealthOrbDroppedItem_C::StaticClass()))
							{
								color = ColorRed;
								Text = "Health";
								if (hp < (maxhp * (CFG::cfg_Loot_DrawHPThreshold / 100.0f)))
									shouldVacuum = true;
							}
							else if (Item->IsA(TFD::ABP_ManaOrbDroppedItem_C::StaticClass()))
							{
								color = ColorMana;
								Text = "Mana";
								if (mana < (maxmana * (CFG::cfg_Loot_DrawMPThreshold / 100.0f)))
									shouldVacuum = true;
							}
							else if (Item->IsA(TFD::ABP_AmmoEnhancedDroppedItem_C::StaticClass()))
							{
								Text = "Enhanced Ammo";
								if (GetSpareRounds(TFD::EM1RoundsType::EnhancedRounds, 120))
									shouldVacuum = true;
							}
							else if (Item->IsA(TFD::ABP_AmmoGeneralDroppedItem_C::StaticClass()))
							{
								Text = "General Ammo";
								if (GetSpareRounds(TFD::EM1RoundsType::GeneralRounds, 200))
									shouldVacuum = true;
							}
							else if (Item->IsA(TFD::ABP_AmmoHighpowerDroppedItem_C::StaticClass()))
							{
								Text = "Highpower Ammo";
								if (GetSpareRounds(TFD::EM1RoundsType::HighpowerRounds, 2))
									shouldVacuum = true;
							}
							else if (Item->IsA(TFD::ABP_AmmoImpactDroppedItem_C::StaticClass()))
							{
								Text = "Impact Ammo";
								if (GetSpareRounds(TFD::EM1RoundsType::ImpactRounds, 25))
									shouldVacuum = true;
							}
							else
							{
								Text = Item->Class->GetFullName();
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemNames)
							{
								Render::R_DrawText(ScreenPos.X, ScreenPos.Y, Text, &color);
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemBoxes)
							{
								float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
								if (ODistance > 0)
								{
									float Width = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									float Height = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									Render::R_DrawRect(ScreenPos.X - Width, ScreenPos.Y - Height, ScreenPos.X + Width, ScreenPos.Y + Height, &color);
								}
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemCircles)
							{
								float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
								if (ODistance > 0)
								{
									float Radius = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, Radius, &color);
								}
							}

							if (shouldVacuum && CFG::cfg_Loot_EnableLootVacuum)
							{
								SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
								float Distance = WorldPosition.GetDistanceTo(player);
								if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
									Item->K2_SetActorLocation(player, false, nullptr, true);
							}

						}
						else if (Item->DropItemInfo.ItemBox.Type == TFD::EM1ItemType::Currency)
						{
							SDK::FLinearColor color = ColorDarkRed;
							std::string Text = "Unknown Currency";

							if (Item->IsA(TFD::ABP_GoldDroppedItem_C::StaticClass()))
							{
								color = ColorGold;
								Text = "Gold";
							}
							else if (Item->IsA(TFD::ABP_KuiperShardDroppedItem_C::StaticClass()))
							{
								color = ColorMana;
								Text = "Kuiper Shard";
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemNames)
							{
								Render::R_DrawText(ScreenPos.X, ScreenPos.Y, Text, &color);
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemBoxes)
							{
								float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
								if (ODistance > 0)
								{
									float Width = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									float Height = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									Render::R_DrawRect(ScreenPos.X - Width, ScreenPos.Y - Height, ScreenPos.X + Width, ScreenPos.Y + Height, &color);
								}
							}

							if (OnScreen && CFG::cfg_Loot_DrawItemCircles)
							{
								float ODistance = Item->GetDistanceTo(LocalPlayerController->Pawn) / 100.0f;
								if (ODistance > 0)
								{
									float Radius = std::clamp(20.0f / ODistance, 3.0f, 20.0f);
									Render::R_DrawCircle(ScreenPos.X, ScreenPos.Y, Radius, &color);
								}
							}

							if (CFG::cfg_Loot_EnableLootVacuum)
							{
								SDK::FVector player = LocalPlayerCharacter->K2_GetActorLocation();
								float Distance = WorldPosition.GetDistanceTo(player);
								if (Distance > 150 && Distance < CFG::cfg_Loot_LootVacuumRange)
									Item->K2_SetActorLocation(player, false, nullptr, true);
							}
						}
					}
				}
			}
		}
	}

	void Aimbot()
	{
		static int currentTargetID;

		if (CFG::cfg_Aim_EnableAimbotHold)
		{
			if (!CFG::cfg_Aim_AimbotUseController && !IsKeyHeld(CFG::cfg_Aim_AimbotHoldKey))
			{
				currentTargetID = 0;
				Aimbot_BoneIndex = -1;
				return;
			}
			if (CFG::cfg_Aim_AimbotUseController)
			{
				bool isAiming = false;
				for (DWORD i = 0; i < 4; i++)
				{
					if (g_Controllers[i].bConnected)
					{
						if (g_Controllers[i].state.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
						{
							isAiming = true;
						}
					}
				}
				if (!isAiming)
				{
					currentTargetID = 0;
					Aimbot_BoneIndex = -1;
					return;
				}
			}
		}
		else if (CFG::cfg_Aim_EnableAimbotToggle && !CFG::cfg_Aim_EnableAimbot)
		{
			currentTargetID = 0;
			Aimbot_BoneIndex = -1;
			return;
		}

		if (currentTargetID && Aimbot_Target && Aimbot_BoneIndex != -1)
		{
			if (Aimbot_Target->IsA(TFD::AM1Character::StaticClass()))
			{
				TFD::AM1Character* AimTarget = static_cast<TFD::AM1Character*>(Aimbot_Target);
				if (!AimTarget->IsDead() && LocalPlayerController->LineOfSightTo(AimTarget, SDK::FVector{ 0, 0, 0 }, false))
				{
					if (AimTarget->Mesh && AimTarget->Mesh->BoneArray.IsValid() && AimTarget->Mesh->BoneArray.IsValidIndex(Aimbot_BoneIndex))
					{
						SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(AimTarget->Mesh->K2_GetComponentToWorld());
						SDK::FTransform bone = AimTarget->Mesh->BoneArray[Aimbot_BoneIndex];
						SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
						SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
						SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
						SDK::FRotator Angles = TFD::UKismetMathLibrary::FindLookAtRotation(LocalPlayerController->PlayerCameraManager->GetCameraLocation(), WorldPosition.Translation);
						LocalPlayerController->SetControlRotation(
							TFD::UKismetMathLibrary::RInterpTo(LocalPlayerController->PlayerCameraManager->GetCameraRotation(), Angles, ((TFD::UGameplayStatics*)TFD::UGameplayStatics::StaticClass())->GetWorldDeltaSeconds(GWorld), CFG::cfg_Aim_AimbotSpeed));
						return;
					}

				}
			}
			else if (Aimbot_Target->IsA(TFD::AM1AbilityActor::StaticClass()))
			{

				SDK::FRotator Angles = TFD::UKismetMathLibrary::FindLookAtRotation(LocalPlayerController->PlayerCameraManager->GetCameraLocation(), Aimbot_Target->K2_GetActorLocation());
				LocalPlayerController->SetControlRotation(
					TFD::UKismetMathLibrary::RInterpTo(LocalPlayerController->PlayerCameraManager->GetCameraRotation(), Angles, ((TFD::UGameplayStatics*)TFD::UGameplayStatics::StaticClass())->GetWorldDeltaSeconds(GWorld), CFG::cfg_Aim_AimbotSpeed));
				return;
			}
			currentTargetID = 0;
			Aimbot_BoneIndex = -1;
			Aimbot_Target = nullptr;
		}

		Aimbot_Target = GetClosestEnemy(currentTargetID);
	}

	TFD::AActor* GetClosestEnemy(int& ID)
	{

		TFD::AActor* ret = nullptr;
		float closestDistance = INFINITY;
		Aimbot_BoneIndex = -1;
		if (LocalPlayerController->ActorManager_Subsystem->Characters.IsValid() && LocalPlayerController->ActorManager_Subsystem->Characters.Num() > 0)
		{
			int StartNumber = LocalPlayerController->ActorManager_Subsystem->Characters.Num();
			for (int i = 0; i < LocalPlayerController->ActorManager_Subsystem->Characters.Num(); i++)
			{
				if (LocalPlayerController->ActorManager_Subsystem->Characters.Num() != StartNumber)
					return nullptr;
				if (!LocalPlayerController->ActorManager_Subsystem->Characters.IsValidIndex(i))
					continue;
				TFD::AM1Character* p = LocalPlayerController->ActorManager_Subsystem->Characters[i];
				if (p)
				{
					if (p->IsDead())
						continue;
					if (p->IsA(TFD::AM1Monster::StaticClass()) || p->CharacterAttribute->IsA(TFD::UM1MonsterAttribute::StaticClass()))
					{
						if (!LocalPlayerController->LineOfSightTo(p, SDK::FVector{ 0, 0, 0 }, false))
						{
							if (p->Children.Num() > 0)
							{
								bool any = false;
								for (int c = 0; c < p->Children.Num(); c++)
								{
									if (p->Children[c]->IsA(TFD::AM1AbilityActor::StaticClass()))
										if (p->Children[c]->Class->GetFullName().contains("Immunity") || p->Children[c]->Class->GetFullName().contains("JudgementEye"))
											if (LocalPlayerController->LineOfSightTo(p->Children[c], SDK::FVector{ 0, 0, 0 }, false))
												any = true;
								}
								if (!any)
									continue;
							}
							else
								continue;
						}
						if (!IDBoneMap.contains(p->CharacterId.ID))
						{
							if (p->Mesh && p->Mesh->BoneArray.IsValid() && p->Mesh->BoneArray.Num() > 0)
							{
								std::vector<int> bones = { };
								for (int j = 0; j < p->Mesh->BoneArray.Num(); j++)
								{
									if (p->Mesh->BoneArray.IsValidIndex(j))
									{
										if (p->Mesh->GetBoneName(j).ToString().contains("Weakness") || p->Mesh->GetBoneName(j).ToString().contains("-Head") || p->Mesh->GetBoneName(j).ToString().contains("_head"))
										{
											bones.push_back(j);
										}
									}
								}
								if (bones.size() > 0)
								{
									IDBoneMap.insert({ p->CharacterId.ID, bones });
									UpdateCache = true;
								}
							}
						}
						else
						{
							if (p->Mesh && p->Mesh->BoneArray.IsValid() && p->Mesh->BoneArray.Num() > 0)
							{
								SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(p->Mesh->K2_GetComponentToWorld());
								std::vector<int> indexes = IDBoneMap[p->CharacterId.ID];
								for (int j = 0; j < indexes.size(); j++)
								{
									if (p->Mesh->BoneArray.IsValidIndex(indexes[j]))
									{
										SDK::FTransform bone = p->Mesh->BoneArray[indexes[j]];
										SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
										SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
										SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
										SDK::FVector2D BoneScreenPos = { -1, -1 };
										if (WorldToScreen(WorldPosition.Translation, &BoneScreenPos))
										{
											float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, BoneScreenPos);
											if (distance > CFG::cfg_Aim_AimbotFOV)
												continue;
											if (distance < closestDistance)
											{
												closestDistance = distance;
												ID = p->Index;
												Aimbot_BoneIndex = indexes[j];
												ret = p;
											}
										}
									}
								}
							}

							if (p->Children.Num() > 0)
							{
								for (int a = 0; a < p->Children.Num(); a++)
								{
									if (p->Children[a]->IsA(TFD::AM1AbilityActor::StaticClass()))
									{
										if (!(p->Children[a]->Class->GetFullName().contains("Immun")) && !(p->Children[a]->Class->GetFullName().contains("JudgementEye")))
											continue;
										SDK::FVector2D ScreenPos = { -1, -1 };
										if (WorldToScreen(p->Children[a]->K2_GetActorLocation(), &ScreenPos))
										{
											float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
											if (distance > CFG::cfg_Aim_AimbotFOV)
												continue;
											if (distance < closestDistance)
											{
												closestDistance = distance;
												ID = p->Children[a]->Index;
												Aimbot_BoneIndex = 999;
												ret = p->Children[a];
											}
										}
									}
								}
							}

						}
					}
				}
			}
		}
		return ret;
	}

	void SaveCacheData()
	{
		std::ofstream outFile("cache.dat", std::ios::binary);

		if (!outFile) {
			return;
		}
		outFile.clear();
		// Write the size of the map first
		size_t mapSize = IDBoneMap.size();
		outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

		// Write each key-vector pair
		for (const auto& pair : IDBoneMap) {
			// Write the key
			outFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

			// Write the size of the vector
			size_t vectorSize = pair.second.size();
			outFile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(vectorSize));

			// Write the vector elements
			outFile.write(reinterpret_cast<const char*>(pair.second.data()), vectorSize * sizeof(int));
		}

		size_t size = IDNameMap.size();
		outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

		// Write each key-value pair
		for (const auto& pair : IDNameMap) {
			// Write the key
			outFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

			// Write the length of the string value
			size_t strLength = pair.second.size();
			outFile.write(reinterpret_cast<const char*>(&strLength), sizeof(strLength));

			// Write the string value
			outFile.write(pair.second.data(), strLength);
		}

		outFile.close();

	}
	void LoadCacheData()
	{
		std::unordered_map<int, std::vector<int>> bonemap;
		std::unordered_map<int, std::string> map;
		std::ifstream inFile("cache.dat", std::ios::binary);

		if (!inFile) {
			return;
		}

		size_t mapSize = 0;
		inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

		for (size_t i = 0; i < mapSize; ++i) {
			int key;
			size_t vectorSize;
			std::vector<int> value;

			// Read the key
			inFile.read(reinterpret_cast<char*>(&key), sizeof(key));

			// Read the size of the vector
			inFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));

			// Resize the vector to hold the elements
			value.resize(vectorSize);

			// Read the vector elements
			inFile.read(reinterpret_cast<char*>(value.data()), vectorSize * sizeof(int));

			// Insert into the map
			bonemap[key] = value;
		}

		// Read the size of the map
		size_t size = 0;
		inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

		for (size_t i = 0; i < size; ++i) {
			int key;
			size_t strLength;
			std::string value;

			// Read the key
			inFile.read(reinterpret_cast<char*>(&key), sizeof(key));

			// Read the length of the string value
			inFile.read(reinterpret_cast<char*>(&strLength), sizeof(strLength));

			// Read the string value
			value.resize(strLength);
			inFile.read(&value[0], strLength);

			// Insert into the map
			map[key] = value;
		}
		inFile.close();

		IDBoneMap = bonemap;
		IDNameMap = map;
	}

	HRESULT UpdateControllerState()
	{
		DWORD dwResult;
		for (DWORD i = 0; i < 4; i++)
		{
			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &g_Controllers[i].state);

			if (dwResult == ERROR_SUCCESS)
				g_Controllers[i].bConnected = true;
			else
				g_Controllers[i].bConnected = false;
		}

		return S_OK;
	}

	void MissionESPTeleport()
	{
		if (Missions != nullptr && Missions->AvailableMissions)
		{
			if (Missions->AvailableMissions.IsValid())
			{
				TFD::AActor* TPTarget = nullptr;
				float closestDistance = INFINITY;
				for (int i = 0; i < Missions->AvailableMissions.Num(); i++)
				{
					if (Missions->AvailableMissions.IsValidIndex(i))
					{
						TFD::AM1MissionActor* Actor = Missions->AvailableMissions[i];
						if (Actor)
						{
							SDK::FVector2D ScreenPos = { -1, -1 };
							if (WorldToScreen(Actor->K2_GetActorLocation(), &ScreenPos))
							{
								if (CFG::cfg_Mission_EnableMissionESP)
								{
									if (Actor->MissionData)
									{
										std::string MissionName = Actor->MissionData->MissionDataRowName.ToString();
										if (!MissionName.empty())
										{
											//char bufferlol[128];
											//sprintf_s(bufferlol, "Mission %s", Actor->MissionData->MissionDataRowName.ToString().c_str());
											Render::R_DrawText(ScreenPos.X, ScreenPos.Y, MissionName, &ColorGold, true);
										}
									}
								}

								float distance = TFD::UKismetMathLibrary::Distance2D(ScreenMiddle, ScreenPos);
								if (distance > 300)
									continue;
								if (distance < closestDistance)
								{
									closestDistance = distance;
									TPTarget = Actor;
								}
							}
						}
					}
				}
				if (IsKeyPressed(CFG::cfg_Mission_MissionTeleportKey) && TPTarget != nullptr)
				{
					LocalPlayerCharacter->TeleportHandler->ServerMoveToTeleportToLocation(TPTarget->K2_GetActorLocation(), TPTarget->K2_GetActorRotation());
				}
			}
		}
	}

	void AutoRestartMission()
	{
		if (Missions == nullptr)
			return;

		static std::chrono::steady_clock::time_point LastMissionEndTime;
		static bool bMissionCompleted = false;
		Render::R_DrawText(ScreenMiddle.X, 50, "Mission Auto-Restart Enabled", &ColorRed);

		if (bMissionCompleted)
			Render::R_DrawText(ScreenMiddle.X, 64, "Mission Completed", &ColorRed);
		else
			Render::R_DrawText(ScreenMiddle.X, 64, "No Mission Completed", &ColorRed);


		//if (!Missions->ActivatedMissions.IsValid() || !Missions->LastActivatedMissions.IsValid())
		//	return;

		//Render::R_DrawText(ScreenMiddle.X, 78, "Pointers valid", &ColorRed);

		// Check if the player has completed a mission and is not in a new one
		if (Missions->LastActivatedMissions.IsValid())
		{
			if (!bMissionCompleted && !Missions->ActivatedMissions.IsValid())
			{
				// Store the time when the mission ended
				LastMissionEndTime = std::chrono::steady_clock::now();
				bMissionCompleted = true;
			}
		}

		// If we are in the waiting period after mission completion
		if (bMissionCompleted)
		{

			if (Missions->ActivatedMissions.IsValid())
			{
				bMissionCompleted = false;
				return;
			}
			// Get the current time
			auto Now = std::chrono::steady_clock::now();

			// Calculate the elapsed time in seconds
			double ElapsedSeconds = std::chrono::duration<double>(Now - LastMissionEndTime).count();

			char bufferlol[32];
			sprintf_s(bufferlol, "Elapsed Time %f", ElapsedSeconds);
			Render::R_DrawText(ScreenMiddle.X, 92, bufferlol, &ColorRed);

			// Check if 5 seconds have passed
			if (ElapsedSeconds >= CFG::cfg_Mission_MissionAutoRestartDelay)
			{
				// Reset the flag
				if (Missions->MissionResult->MissionSubType == TFD::EM1MissionSubType::DestructionVulgusPost)
				{
					for (TFD::AM1MissionActor* LAMA : Missions->LastActivatedMissions)
					{
						if (LAMA->MissionData->MissionSubType == Missions->MissionResult->MissionSubType)
						{
							Missions->ServerStartMission(LAMA, true);
							bMissionCompleted = false;
						}
					}
				}
				else
				{
					Missions->ServerRestartLastPlayedMission();
					bMissionCompleted = false;
				}
			}
		}
	}

	void WeaponModifications()
	{
		if (!LocalPlayerCharacter)
			return;

		auto* WeaponControl = LocalPlayerCharacter->WeaponSlotControl;
		if (!WeaponControl)
			return;

		auto* WeaponBase = WeaponControl->ActivatedWeaponSlot.WeaponSlot.Weapon;
		if (!WeaponBase || !WeaponBase->IsA(TFD::AM1RangedWeapon::StaticClass()))
			return;

		TFD::AM1RangedWeapon* Weapon = static_cast<TFD::AM1RangedWeapon*>(WeaponBase);

		if (CFG::cfg_Aim_RapidFire && Weapon->FireLoopComponent)
		{
			auto& Params = Weapon->FireLoopComponent->CurrFireParams;
			if (!Params.IsSet())
				return;

			float& FireInterval = Params.GetValueRef().FireInterval;
			float& ElapsedTime = Weapon->FireLoopComponent->ElapsedTimeAfterFire;

			FireInterval -= FireInterval * (CFG::cfg_Aim_FireRate / 100.0f);
			ElapsedTime -= ElapsedTime * (CFG::cfg_Aim_FireRate / 100.0f);
		}

		if (CFG::cfg_Aim_NoReload && Weapon->RoundsComponent)
		{
			if (Weapon->RoundsComponent->CurrentRounds < 3)
			{
				Weapon->RoundsComponent->ClientFillCurrentRoundByServer();
			}
		}

		if (CFG::cfg_Aim_NoRecoilAndSpread && Weapon->SprayPatternComponent)
		{
			auto* Spray = Weapon->SprayPatternComponent;

			if (Spray->CrosshairSizeBase != 0.0f) Spray->CrosshairSizeBase = 0.0f;
			if (Spray->bApplySpreadSize != false) Spray->bApplySpreadSize = false;
			if (Spray->CurrentSpreadSize != 0.0f) Spray->CurrentSpreadSize = 0.0f;
			if (Spray->CurrAccuracySizeInterp != 0.000001f) Spray->CurrAccuracySizeInterp = 0.000001f;
			if (Spray->CurrBaseSizeInterp != 0.000001f) Spray->CurrBaseSizeInterp = 0.000001f;
			if (Spray->RecoverRecoilStartDelayTime != 0.000001f) Spray->RecoverRecoilStartDelayTime = 0.000001f;


			if (Spray->RecoilData->RecoilApplyInterpSpeed != 0.000001f) Spray->RecoilData->RecoilApplyInterpSpeed = 0.000001f;
			if (Spray->RecoilData->RecoilRecoverInterpSpeed != 0.000001f) Spray->RecoilData->RecoilRecoverInterpSpeed = 0.000001f;
			if (Spray->RecoilData->RecoilRecoverStartDelay != 0.000001f) Spray->RecoilData->RecoilRecoverStartDelay = 0.000001f;
			if (Spray->RecoilData->RecoilResetTimeAfterFire != 0.000001f) Spray->RecoilData->RecoilResetTimeAfterFire = 0.000001f;
			if (Spray->ZoomRecoilData->RecoilApplyInterpSpeed != 0.000001f) Spray->ZoomRecoilData->RecoilApplyInterpSpeed = 0.000001f;
			if (Spray->ZoomRecoilData->RecoilRecoverInterpSpeed != 0.000001f) Spray->ZoomRecoilData->RecoilRecoverInterpSpeed = 0.000001f;
			if (Spray->ZoomRecoilData->RecoilRecoverStartDelay != 0.000001f) Spray->ZoomRecoilData->RecoilRecoverStartDelay = 0.000001f;
			if (Spray->ZoomRecoilData->RecoilResetTimeAfterFire != 0.000001f) Spray->ZoomRecoilData->RecoilResetTimeAfterFire = 0.000001f;

			LocalPlayerController->PlayerCameraManager->StopAllCameraShakes(true);
		}
	}
	/*if (Spray->RecoilData)
	{
		if (Spray->RecoilData->RecoilApplyInterpSpeed > 0.000001f)
			Spray->RecoilData->RecoilApplyInterpSpeed = 0.000001f;
		if (Spray->RecoilData->RecoilRecoverInterpSpeed > 0.000001f)
			Spray->RecoilData->RecoilRecoverInterpSpeed = 0.000001f;
	}

	if (Spray->ZoomRecoilData)
	{
		if (Spray->ZoomRecoilData->RecoilApplyInterpSpeed > 0.000001f)
			Spray->ZoomRecoilData->RecoilApplyInterpSpeed = 0.000001f;
		if (Spray->ZoomRecoilData->RecoilRecoverInterpSpeed > 0.000001f)
			Spray->ZoomRecoilData->RecoilRecoverInterpSpeed = 0.000001f;
	}*/

	void ModifyGrapple()
	{
		if (!Cheat::GWorld || !LocalPlayerController || !LocalPlayerCharacter)
			return;

		auto* Caster = LocalPlayerCharacter->WireCaster;
		auto* WeaponControl = LocalPlayerCharacter->WeaponSlotControl;

		if (!Caster || !WeaponControl)
			return;

		auto* AbilityComponent = WeaponControl->Ability_Component;

		if (!AbilityComponent)
			return;

		for (auto* Ability : AbilityComponent->RegisteredAbilities)
		{
			if (!Ability || !Ability->IsA(TFD::UM1WireSkillAbility::StaticClass()))
				continue;
			auto* WireAbility = static_cast<TFD::UM1WireSkillAbility*>(Ability);

			if (WireAbility->FireMaxDistance != CFG::cfg_Abilities_GrappleRange)
			{
				Caster->bValidLandingSpot = true;
				WireAbility->FireMinDistance = 0.0f;
				WireAbility->FireMaxDistance = CFG::cfg_Abilities_GrappleRange;
				WireAbility->bAllowedAirDestination = true;
			}
		}
	}

	void CreateConsole()
	{
		TFD::UEngine* Engine = TFD::UEngine::GetEngine();
		if (!Engine->GameViewport->ViewportConsole)
		{
			TFD::UInputSettings::GetDefaultObj()->ConsoleKeys[0].KeyName = TFD::UKismetStringLibrary::Conv_StringToName(L"F1");
			Engine->GameViewport->ViewportConsole = static_cast<TFD::UConsole*>(TFD::UGameplayStatics::SpawnObject(Engine->ConsoleClass, Engine->GameViewport));
		}
	}
	void CreateCheatManager()
	{
		LocalPlayerController->CheatManager = static_cast<TFD::UCheatManager*>(TFD::UGameplayStatics::SpawnObject(LocalPlayerController->CheatClass, LocalPlayerController));
	}
	void ToggleCheatUI()
	{
		LocalPlayerController->EnableCheats();
		//static_cast<TFD::UM1Cheat*>(LocalPlayerController->CheatManager)->ToggleUI();
	}

	void PresetOverlay()
	{

		if (!PresetsMap.empty())
		{
			int YPos = 20;
			for (int i = 0; i < PresetsMap.size(); i++)
			{
				std::string buffer;

				if (PresetsMap.contains(i))
					buffer = std::format("Preset {}: {}", i + 1, PresetsMap[i]);
				else
					buffer = std::format("Preset {}: None", i + 1);

				if (i == CurrentPresetIndex)
					Render::R_DrawText(270, YPos, buffer, &ColorGold, true);
				else
					Render::R_DrawText(270, YPos, buffer, &ColorWhite);

				YPos += 14;
			}
		}
	}

	void PresetRefresh()
	{
		if (!LocalPlayerController || !LocalPlayerController->PrivateOnlineServiceComponent)
			return;

		TFD::UM1Account* Account = static_cast<TFD::UM1Account*>(TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent));
		if (!Account)
			return;
		if (Account->Preset)
		{
			for (const auto& Pair : Account->Preset->PresetSlotByIndex)
			{

				TFD::FM1PresetSlot Value = Pair.Value();
				std::string PresetName = Value.PresetName.ToString();
				if (PresetName.empty())
				{
					PresetName = std::format("Preset {}", Value.PresetIndex);
				}
				PresetsMap.insert({ Value.PresetIndex, PresetName });
			}
		}
	}

	void PresetActivate()
	{

		if (!LocalPlayerController || !LocalPlayerController->PrivateOnlineServiceComponent)
			return;

		if (!PresetsMap.empty() && CurrentPresetIndex < PresetsMap.size())
		{
			for (TFD::UM1PrivateOnlineSubService* Subserv : LocalPlayerController->PrivateOnlineServiceComponent->SubServices)
			{
				if (!Subserv)
					continue;
				if (Subserv->IsA(TFD::UM1PrivateOnlineServicePreset::StaticClass()) && Subserv->bIsReady == true)
				{
					static_cast<TFD::UM1PrivateOnlineServicePreset*>(Subserv)->ServerRequestApplyPreset(CurrentPresetIndex);
					break;
				}
			}
		}
	}

	void InstantInfiltration()
	{
		if (!Cheat::GWorld || !LocalPlayerController)
			return;

		auto* PlayerState = static_cast<TFD::AM1PlayerState*>(LocalPlayerController->PlayerState);
		if (!PlayerState || !PlayerState->MissionControlComponent)
			return;
		TFD::UM1MissionControlComponent* MCC = PlayerState->MissionControlComponent;
		if (!MCC || MCC->ActivatedMissions.Num() == 0 || MCC->SubServices.Num() == 0)
			return;
		for (TFD::AM1MissionActor* MissionActor : MCC->ActivatedMissions)
		{
			if (!MissionActor)
				continue;
			//int CurrentTask = MissionActor->ProgressInfo.ActivatedTaskIndex;
			if (MissionActor->TaskLinks.IsValidIndex(0))
			{
				TFD::FM1MissionTaskLink TaskLink = MissionActor->TaskLinks[0];
				TFD::AM1MissionTaskActor* TaskActor = TaskLink.InstancedTaskActor;
				if (TaskActor)
				{
					if (!TaskActor->IsA(TFD::AM1MissionTaskActorDestructionVulgusPost::StaticClass()))
						continue;
					auto* VPost = static_cast<TFD::AM1MissionTaskActorDestructionVulgusPost*>(TaskActor);
					if (!VPost)
						continue;
					for (TFD::UM1MissionTaskService* MCCSub : MCC->SubServices)
					{
						if (!MCCSub || !MCCSub->bJoined)
							continue;

						auto* MCCInt = static_cast<TFD::UM1MissionTaskServiceInteraction*>(MCCSub);
						if (!MCCInt)
							continue;
						if (!VPost->MissionTargets.IsValid())
							continue;

						for (TFD::AM1MissionTargetInteraction* Target : VPost->MissionTargets)
						{
							if (!Target ||
								Target->CurrentState == TFD::EM1MissionTargetState::Destructed ||
								Target->CurrentState == TFD::EM1MissionTargetState::Deactivated)
								continue;
							//std::cout << "target is not destructed or deactivated\n";
							auto now = std::chrono::steady_clock::now();
							if (std::chrono::duration_cast<std::chrono::milliseconds>(now - AutoInstantInfilStartTime).count() > 200)
							{
								AutoInstantInfilStartTime = now;
								MCCInt->ServerRequestMissionTargetBeginInteraction(Target, LocalPlayerController);
								return;
							}
						}
					}

				}
			}
		}
	}


	void ResearchBookmarkedItems()
	{
		if (!Cheat::LocalPlayerController || !Cheat::LocalPlayerController->PrivateOnlineServiceComponent)
			return;

		TFD::UM1LocalGameInstanceSubsystem* LS = TFD::UM1LocalGameInstanceSubsystem::GetSystem(Cheat::GWorld);
		if (!LS || !LS->ResearchSystem)
			return;

		TFD::UM1ResearchSystem* RS = LS->ResearchSystem;

		if (Cheat::LocalPlayerController->PrivateOnlineServiceComponent->IsA(TFD::UM1PrivateOnlineServiceComponent::StaticClass()))
		{
			for (TFD::UM1PrivateOnlineSubService* Subserv : Cheat::LocalPlayerController->PrivateOnlineServiceComponent->SubServices)
			{
				if (!Subserv)
					continue;

				if (Subserv->IsA(TFD::UM1PrivateOnlineServiceResearch::StaticClass()) && Subserv->bIsReady)
				{
					for (const auto& RDTID : RS->BookmarkResearchTids)
					{
						static_cast<TFD::UM1PrivateOnlineServiceResearch*>(Subserv)->ServerRequestStartResearch(RDTID, CFG::cfg_ResearchQuantity);
					}
				}
			}
		}
	}

	void AutoResupply()
	{
		if (!LocalPlayerController
			|| !LocalPlayerCharacter
			|| !LocalPlayerCharacter->StatComponent
			|| !LocalPlayerController->MultiSupplierObtainComponent)
			return;

		bool ShouldResupply = false;
		static std::chrono::steady_clock::time_point LastResupplyTime = std::chrono::steady_clock::now();

		if (CFG::cfg_Abilities_EnableAutomaticResupplyTimed)
		{
			auto now = std::chrono::steady_clock::now();
			float elapsed = std::chrono::duration<float>(now - LastResupplyTime).count();

			if (elapsed >= CFG::cfg_Abilities_AutomaticResupplyTime)
			{
				LastResupplyTime = now;
				ShouldResupply = true;
			}
			else
				return;
		}

		if (!ShouldResupply && TryResetAbilities)
		{
			ShouldResupply = true;
			TryResetAbilities = false;
		}

		if (!ShouldResupply)
		{
			float CurrentHP = (float)LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_CurrentHp).Value / 10000.0f;
			float MaxHP = (float)LocalPlayerCharacter->StatComponent->GetStatValue(TFD::EM1StatType::Stat_MaxHp).Value / 10000.0f;
			if (CurrentHP <= (MaxHP * (CFG::cfg_Abilities_AutomaticResupplyHealth / 100.0f)))
				ShouldResupply = true;
		}


		if (!ShouldResupply)
		{
			if (LocalPlayerCharacter->WeaponSlotControl
				&& LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon)
				//&& LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon->RoundsComponent)
			{
				auto* WeaponBase = LocalPlayerCharacter->WeaponSlotControl->ActivatedWeaponSlot.WeaponSlot.Weapon;
				if (!WeaponBase || !WeaponBase->IsA(TFD::AM1RangedWeapon::StaticClass()))
					return;

				TFD::AM1RangedWeapon* Weapon = static_cast<TFD::AM1RangedWeapon*>(WeaponBase);
				if (Weapon->RoundsComponent->CurrentRounds < CFG::cfg_Abilities_AutomaticResupplyAmmo)
					ShouldResupply = true;
			}
		}

		if (ShouldResupply)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<int> dis(100000, 999999);
			int ObjectUniqueID = dis(gen);

			//std::cout << "resupply......\n";
			//if (ResupplyNPC)
			//	LocalPlayerController->MultiSupplierObtainComponent->ServerRequestProcessInteraction({ 363100004 }, ObjectUniqueID, ResupplyNPC);

		}
	}

	bool GetSpareRounds(TFD::EM1RoundsType RoundsType, int RoundsPerLoot)
	{
		if (!LocalPlayerCharacter || !LocalPlayerCharacter->RoundsComponent)
			return false;

		int CurrentSpareRounds = 0;
		int MaxRoundCapacity = 0;

		CurrentSpareRounds = LocalPlayerCharacter->RoundsComponent->CurrentSpareRounds[(int)RoundsType];

		for (const auto& Capacities : LocalPlayerCharacter->RoundsComponent->CachedMaxCapacities)
		{
			if (Capacities.Key() == RoundsType)
			{
				MaxRoundCapacity = Capacities.Value().CachedCapacity;
				break;
			}
		}

		if (CurrentSpareRounds <= (MaxRoundCapacity - RoundsPerLoot))
			return true;
		else
			return false;
	}

	// Customization functions
	void AddAllCustomizationItems()
	{
		static TFD::UM1Account* Account;
		static TFD::TArray<TFD::FM1CustomizingInfoWrapper> Items;
		if (Items.NumElements == 0)
		{
			// Get ALL customization tIDs
			TFD::TM1DataTable* CustomizationTable = TFD::native_GetCustomizationTable(false);
			if (!CustomizationTable)
				return;

			SDK::TArray<TFD::FM1CustomizingItemData*> Data;
			static SDK::FString Context(L"UM1AccountInventory");
			TFD::native_GetCustomizationData(CustomizationTable->TableObject, &Context, &Data);

			// If you want to dump them all to a file, uncomment this block
			/*std::ofstream outFile("customization_data.txt");
			if (outFile.is_open())
			{
				for (int i = 0; i < Data.Num(); i++)
				{
					TFD::FM1CustomizingItemData* itm = Data[i];
					std::string text = std::format("[{}] - tID: {} - Name: {}", (int)itm->Category, itm->TemplateId.ID, itm->StringId.ToString().c_str());
					outFile << text << "\n";
				}
				outFile.close();
			}*/

			int Count = 0;
			for (int i = 0; i < Data.Num(); i++)
			{
				Count++;
			}
			if (Count > 0)
			{
				Items.Data = (TFD::FM1CustomizingInfoWrapper*)TFD::native_FMemMalloc(20 * Count, 0);
				Items.NumElements = 0;
				Items.MaxElements = Count;
				if (!LocalPlayerController->PrivateOnlineServiceComponent)
					return;
				Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
				for (int i = 0; i < Data.Num(); i++)
				{
					TFD::FM1CustomizingItemData* itm = Data[i];
					TFD::FM1CustomizingInfoWrapper ItemWrapper = {};
					ItemWrapper.bNewItem = true;
					ItemWrapper.CustomizingItemInfo = {};
					ItemWrapper.CustomizingItemInfo.StackCount = 1;
					ItemWrapper.CustomizingItemInfo.Tid = itm->TemplateId;
					ItemWrapper.CustomizingItemInfo.EvolutionComplete = false;
					ItemWrapper.CustomizingItemInfo.EvolutionIdx = TFD::native_GetSkinEvolutionIdx(Account->Inventory, itm->TemplateId);
					Items.Add(ItemWrapper);
				}
			}
		}
		if (Items.NumElements > 0)
		{
			if (!LocalPlayerController->PrivateOnlineServiceComponent)
				return;
			Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
			TFD::native_AddOrUpdateCustomizingItems(Account->Inventory, &Items, true);
		}
	}
	bool TryEquipState = false;
	bool TryEquipSavedCustomization()
	{

		if (!Cheat::LocalPlayerCharacter)
			return false;
		if (!Cheat::LocalPlayerCharacter->CustomizeComponent)
			return false;
		if (!Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.IsValid())
			return false;
		if (Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray.Num() != 6)
			return false;

		TFD::UM1Account* Account = (TFD::UM1Account*)TFD::native_GetUM1Account(LocalPlayerController->PrivateOnlineServiceComponent);
		if (Account)
		{
			int SaveSlot = -1;
			for (int i = 0; i < 3; i++)
				if (CFG::cfg_Customize_SaveSlots[i].CharacterID == LocalPlayerCharacter->CharacterId.ID)
				{
					SaveSlot = i;
					break;
				}

			if (SaveSlot != -1)
			{
				//std::cout << SaveSlot << std::endl;
				// 282000003 - 282100000 = Head		// Slot 0
				// 282100001 - 282200000 = Body		// Slot 1
				// 282400001 - 282500000 = Back		// Slot 2
				// 282500001 - 282600000 = Front	// Slot 3
				// 282600002 - 282700000 = Makeup	// Slot 5
				// 283000001 - 283000090 = Character Paint
				// 283100001 - 283100060 = Hair Paint
				// 284100001 - 284100029 = Spawn	// Slot 4
				// 282009001 - 282109000 = Default Head
				// 282109001 - 282109110 = Default Body

				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Body != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Body }, true);
				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Head != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Head }, true);
				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Chest != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Chest }, true);
				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup }, true);
				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Back != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Back }, true);
				if (CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn != 0)
					TFD::native_AccountEquipCustomizeCharacterSkin(Account->Inventory, LocalPlayerCharacter->CharacterId, TFD::FM1TemplateId{ CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn }, true);



				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[0].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Head;
				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[1].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Body;
				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[2].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Back;
				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[3].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Chest;
				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[4].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Spawn;
				Cheat::LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[5].SkinTid.ID = CFG::cfg_Customize_SaveSlots[SaveSlot].Makeup;


				//TFD::native_SetCharacterCustomizingAsInventory(LocalPlayerCharacter->Mesh, &LocalPlayerCharacter->CustomizeComponent->CustomizingSkinComps, LocalPlayerCharacter->CharacterId, Account->Inventory);
				TFD::native_OnRep_CustomizeCharacterSkinData(Cheat::LocalPlayerCharacter->CustomizeComponent);
				return true;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	void __fastcall hkReceiveCustomizingCharacterSkin(TFD::UM1PrivateOnlineServiceCustomize* This, TFD::FM1TemplateId InTargetCharacterTid, TFD::FM1TemplateId InSkinTid, bool bEquip, TFD::EM1CustomizeReason InReason)
	{
		//std::cout << "Equip: " << InTargetCharacterTid.ID << " - " << InSkinTid.ID << " - " << bEquip << " - " << (int)InReason << std::endl;
		//std::cout << std::hex << static_cast<void*>(LocalPlayerCharacter->CustomizeComponent) << std::dec << std::endl;
		//InReason = EM1CustomizeReason::Success;
		// 282000003 - 282100000 = Head		// Slot 0
		// 282100001 - 282200000 = Body		// Slot 1
		// 282400001 - 282500000 = Back		// Slot 2
		// 282500001 - 282600000 = Front	// Slot 3
		// 282600002 - 282700000 = Makeup	// Slot 5
		// 283000001 - 283000090 = Character Paint
		// 283100001 - 283100060 = Hair Paint
		// 284100001 - 284100029 = Spawn	// Slot 4
		// 282009001 - 282109000 = Default Head
		// 282109001 - 282109110 = Default Body

		//if (CFG::cfg_Customize_EnableCustomizationBypass)
		//{
		//	if ((InSkinTid.ID >= 282000003 && InSkinTid.ID < 282100000) || (InSkinTid.ID >= 282009001 && InSkinTid.ID < 282109000)) // Head
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[0].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[0].SkinTid.ID = 0;
		//	}
		//	else if ((InSkinTid.ID >= 282100001 && InSkinTid.ID < 282200000) || (InSkinTid.ID >= 282109001 && InSkinTid.ID < 282109110)) // Body
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[1].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[1].SkinTid.ID = 0;
		//	}
		//	else if (InSkinTid.ID >= 282400001 && InSkinTid.ID < 282500000) // Back
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[2].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[2].SkinTid.ID = 0;
		//	}
		//	else if (InSkinTid.ID >= 282500001 && InSkinTid.ID < 282600000) // Front
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[3].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[3].SkinTid.ID = 0;
		//	}
		//	else if (InSkinTid.ID >= 284100001 && InSkinTid.ID < 284100030) // Spawn
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[4].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[4].SkinTid.ID = 0;
		//	}
		//	else if (InSkinTid.ID >= 282600002 && InSkinTid.ID < 282700000) // Makeup
		//	{
		//		if (bEquip)
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[5].SkinTid.ID = InSkinTid.ID;
		//		else
		//			LocalPlayerCharacter->CustomizeComponent->CustomizeCharacterSkinData.CustomizeSkinInfoArray[5].SkinTid.ID = 0;
		//	}
		//	TFD::native_ReceiveCustomizingCharacterSkin(This, InTargetCharacterTid, InSkinTid, bEquip, TFD::EM1CustomizeReason::Success);
		//	TFD::native_OnRep_CustomizeCharacterSkinData(Cheat::LocalPlayerCharacter->CustomizeComponent);
		//}
		//else
		TFD::native_ReceiveCustomizingCharacterSkin(This, InTargetCharacterTid, InSkinTid, bEquip, InReason);
	}


	void __fastcall hkSpeedHackDetecting(void* This, float InDeltaTime)
	{
#ifdef IS_DEBUG_VERSION
		std::cout << "UM1SpeedHackDetectorSubSystem::SpeedHackDetecting was triggered.\n";
#endif
	}

	void __fastcall hkBP_FireInternal_Implementation(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, float WeaponRange, TFD::FM1ScaledInteger PenetrationStat)
	{
		//std::cout << std::endl;
		//std::cout << std::endl;
		//std::cout << std::endl;
		//std::cout << "FireLoc Before: (" << FireLoc->X << ", " << FireLoc->Y << ", " << FireLoc->Z << ")\n";
		//if (Aimbot_Target)
		//{
		//	SDK::FVector FirePos = This->Weapon_Owner->WeaponMesh->GetSocketTransform(This->Weapon_Owner->VFXComponent->FireBone, false).Translation;
		//	std::cout << "FirePos Pos: (" << FirePos.X << ", " << FirePos.Y << ", " << FirePos.Z << ")\n";
			//FireLoc->X = FirePos.X;
			//FireLoc->Y = FirePos.Y;
			//FireLoc->Z = FirePos.Z;
		//}

		TFD::native_BP_FireInternal_Implementation(This, FireTime, FireLoc, WeaponRange, PenetrationStat);
	}
	void __fastcall hkHandleInstantHitResult(TFD::UM1WeaponInstantHitComponent* This, TFD::FM1WeaponInstantHitParams* Params, TFD::FM1WeaponInstantHitTraceResult_Multi_Penetration* Result)
	{
		//if (Aimbot_Target)
		//{
		//	TFD::AM1Character* Targ = static_cast<TFD::AM1Character*>(Aimbot_Target);
		//	if (Targ->Mesh && Targ->Mesh->GetNumBones() > 0 && Targ->Mesh->BoneArray.IsValidIndex(Aimbot_BoneIndex))
		//	{
		//		SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(Targ->Mesh->K2_GetComponentToWorld());
		//		SDK::FTransform bone = Targ->Mesh->BoneArray[Aimbot_BoneIndex];
		//		SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
		//		SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
		//		SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
		//		SDK::FVector Target = WorldPosition.Translation;
		//		for (int i = 0; i < Result->Traces.Num(); i++)
		//		{
		//			Result->Traces[i].FireEnd = Target;
		//			Result->Traces[i].bBlocked = false;
		//			for (int j = 0; j < Result->Traces[i].HitResults.Num(); j++)
		//			{
		//				Result->Traces[i].HitResults[j].ImpactPoint = Target;
		//				Result->Traces[i].HitResults[j].ImpactNormal = (Target - Params->FireLoc).Normalize();
		//				Result->Traces[i].HitResults[j].bBlockingHit = false;
		//				Result->Traces[i].HitResults[j].HitObjectHandle.Actor.ObjectIndex = Targ->Index;
		//				Result->Traces[i].HitResults[j].HitObjectHandle.Actor.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Index)->SerialNumber;
		//				Result->Traces[i].HitResults[j].Component.ObjectIndex = Targ->Mesh->Index;
		//				Result->Traces[i].HitResults[j].Component.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Mesh->Index)->SerialNumber;
		//				Result->Traces[i].HitResults[j].BoneName = Targ->Mesh->GetBoneName(Aimbot_BoneIndex);
		//			}
		//		}
		//	}
		//}
		TFD::native_HandleInstantHitResult(This, Params, Result);
	}

	bool __fastcall hkIsValidHit(TFD::UM1WeaponInstantHitComponent* This, float FireTime, SDK::FVector* FireLoc, SDK::FVector* FireDir, TFD::FHitResult* HitResult)
	{
		bool IsValid = TFD::native_IsValidHit(This, FireTime, FireLoc, FireDir, HitResult);
		std::cout << "IsValidHit Return: (" << IsValid << ")\n";
		std::cout << "IsValidHit FireLoc: (" << FireLoc->X << ", " << FireLoc->Y << ", " << FireLoc->Z << ")\n";
		std::cout << "IsValidHit FireDir: (" << FireDir->X << ", " << FireDir->Y << ", " << FireDir->Z << ")\n";
		std::cout << "IsValidHit HitResult ImpactPoint: (" << HitResult->ImpactPoint.X << ", " << HitResult->ImpactPoint.Y << ", " << HitResult->ImpactPoint.Z << ")\n";
		return IsValid;
	}


	float DistX = 0.0f;
	float DistY = 0.0f;
	float DistZ = 0.0f;

	bool __fastcall hkLineTraceMulti(SDK::TArray<TFD::FHitResult>* OutHits, TFD::AActor* InSrcActor, SDK::FVector* StartPos, SDK::FVector* EndPos, TFD::EM1RelationsCheckType InRelationsCheckType, void* InQueryTag, bool InbCheckValidTarget, bool bTraceComplex, bool bReturnBlockedTargetIfNoTarget)
	{
		if (Aimbot_Target)
		{
			TFD::AM1Character* Targ = static_cast<TFD::AM1Character*>(Aimbot_Target);
			if (Targ->Mesh && Targ->Mesh->GetNumBones() > 0 && Targ->Mesh->BoneArray.IsValidIndex(Aimbot_BoneIndex))
			{
				SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(Targ->Mesh->K2_GetComponentToWorld());
				SDK::FTransform bone = Targ->Mesh->BoneArray[Aimbot_BoneIndex];
				SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
				SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
				SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
				SDK::FVector Target = WorldPosition.Translation;
				SDK::FVector Start = *StartPos + SDK::FVector(DistX, DistY, DistZ);
				//std::cout << "hkLineTraceMulti StartPos Before: (" << StartPos->X << ", " << StartPos->Y << ", " << StartPos->Z << ")\n";
				//std::cout << "hkLineTraceMulti EndPos Before: (" << EndPos->X << ", " << EndPos->Y << ", " << EndPos->Z << ")\n\n";
				*StartPos = Start;
				*EndPos = Target;
				//std::cout << "hkLineTraceMulti StartPos After: (" << StartPos->X << ", " << StartPos->Y << ", " << StartPos->Z << ")\n";
				//std::cout << "hkLineTraceMulti EndPos After: (" << EndPos->X << ", " << EndPos->Y << ", " << EndPos->Z << ")\n\n";

				InbCheckValidTarget = false;
				bTraceComplex = false;

				//bool Ret = TFD::native_LineTraceMulti(OutHits, InSrcActor, StartPos, EndPos, InRelationsCheckType, InQueryTag, InbCheckValidTarget, bTraceComplex, bReturnBlockedTargetIfNoTarget);
				//if (Ret)
				//{
					//if (!OutHits)
					//{
					//	std::cout << "hkLineTraceMulti OutHits null?\n";
					//	return Ret;
					//}
					//if (!OutHits->IsValid())
					//{
					//	std::cout << "hkLineTraceMulti OutHits " << OutHits->NumElements << " - " << OutHits->MaxElements << "\n";
					//	if (OutHits->Data)
					//		std::cout << "hkLineTraceMulti OutHits Data not null?\n";
					//	return Ret;
					//}
					//for (int i = 0; i < OutHits->Num(); i++)
					//{
					//	TFD::FHitResult Res = (*OutHits)[i];
					//	//std::cout << "hkLineTraceMulti OutHits Result " << i << " Actor Index: (" << Res.HitObjectHandle.Actor.ObjectIndex << ")\n";
					//	std::cout << "hkLineTraceMulti OutHits Result " << i << " Actor bBlockingHit: (" << Res.bBlockingHit << ")\n";
					//	std::cout << "hkLineTraceMulti OutHits Result " << i << " Distance: (" << Res.Distance << ")\n";
					//	std::cout << "hkLineTraceMulti OutHits Result " << i << " Normal: (" << Res.Normal.X << ", " << Res.Normal.Y << ", " << Res.Normal.Z << ")\n";
					//	std::cout << "hkLineTraceMulti OutHits Result " << i << " ImpactNormal: (" << Res.ImpactNormal.X << ", " << Res.ImpactNormal.Y << ", " << Res.ImpactNormal.Z << ")\n";
					//}
				//}

				//return Ret;
			}
		}
		return TFD::native_LineTraceMulti(OutHits, InSrcActor, StartPos, EndPos, InRelationsCheckType, InQueryTag, InbCheckValidTarget, bTraceComplex, bReturnBlockedTargetIfNoTarget);
		//bool Ret = TFD::native_LineTraceMulti(OutHits, InSrcActor, StartPos, EndPos, InRelationsCheckType, InQueryTag, InbCheckValidTarget, bTraceComplex, bReturnBlockedTargetIfNoTarget);
		//std::cout << "hkLineTraceMulti Return: (" << Ret << ")\n";
		//if (Ret)
		//{
		//	if (!OutHits)
		//	{
		//		std::cout << "hkLineTraceMulti OutHits null?\n";
		//		return false;
		//	}
		//	if (!OutHits->IsValid())
		//	{
		//		std::cout << "hkLineTraceMulti OutHits " << OutHits->NumElements << " - " << OutHits->MaxElements << "\n";
		//		if(OutHits->Data)
		//			std::cout << "hkLineTraceMulti OutHits Data not null?\n";
		//		return false;
		//	}
		//	if (Aimbot_Target)
		//	{
		//		TFD::AM1Character* Targ = static_cast<TFD::AM1Character*>(Aimbot_Target);
		//		if (Targ->Mesh && Targ->Mesh->GetNumBones() > 0 && Targ->Mesh->BoneArray.IsValidIndex(Aimbot_BoneIndex))
		//		{
		//			//std::cout << "hkLineTraceMulti trying to force valid hit\n";
		//			SDK::FMatrix ComponentMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(Targ->Mesh->K2_GetComponentToWorld());
		//			SDK::FTransform bone = Targ->Mesh->BoneArray[Aimbot_BoneIndex];
		//			SDK::FMatrix BoneMatrix = TFD::UKismetMathLibrary::Conv_TransformToMatrix(bone);
		//			SDK::FMatrix WorldMatrix = TFD::UKismetMathLibrary::Multiply_MatrixMatrix(BoneMatrix, ComponentMatrix);
		//			SDK::FTransform WorldPosition = TFD::UKismetMathLibrary::Conv_MatrixToTransform(WorldMatrix);
		//			SDK::FVector Target = WorldPosition.Translation;
		//			for (int i = 0; i < OutHits->Num(); i++)
		//			{
		//				(*OutHits)[i].ImpactPoint = Target;
		//				(*OutHits)[i].ImpactNormal = (Target - *StartPos).Normalize();
		//				(*OutHits)[i].bBlockingHit = false;
		//				(*OutHits)[i].HitObjectHandle.Actor.ObjectIndex = Targ->Index;
		//				(*OutHits)[i].HitObjectHandle.Actor.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Index)->SerialNumber;
		//				(*OutHits)[i].Component.ObjectIndex = Targ->Mesh->Index;
		//				(*OutHits)[i].Component.ObjectSerialNumber = SDK::UObject::GObjects->GetItemByIndex(Targ->Mesh->Index)->SerialNumber;
		//				(*OutHits)[i].BoneName = Targ->Mesh->GetBoneName(Aimbot_BoneIndex);
		//			}
		//			return true;
		//		}
		//	}
		//}
		/*if (Ret)
		{
			std::cout << std::endl;
			std::cout << "hkLineTraceMulti Return: (" << Ret << ")\n";
			if (OutHits && OutHits->IsValid())
			{
				std::cout << "hkLineTraceMulti OutHits: (" << OutHits->Num() << ")\n";
				for (int i = 0; i < OutHits->Num(); i++)
				{
					TFD::FHitResult Res = (*OutHits)[i];
					std::cout << "hkLineTraceMulti OutHits Result " << i << " Actor Index: (" << Res.HitObjectHandle.Actor.ObjectIndex << ")\n";
				}
			}
			std::cout << "hkLineTraceMulti StartPos: (" << StartPos->X << ", " << StartPos->Y << ", " << StartPos->Z << ")\n";
			std::cout << "hkLineTraceMulti EndPos: (" << EndPos->X << ", " << EndPos->Y << ", " << EndPos->Z << ")\n";
			std::cout << "hkLineTraceMulti InRelationsCheckType: (" << (int)InRelationsCheckType << ")\n";
			std::cout << "hkLineTraceMulti InbCheckValidTarget: (" << InbCheckValidTarget << ")\n";
			std::cout << "hkLineTraceMulti bTraceComplex: (" << bTraceComplex << ")\n";
			std::cout << "hkLineTraceMulti bReturnBlockedTargetIfNoTarget: (" << bReturnBlockedTargetIfNoTarget << ")\n";
			std::cout << std::endl;
		}*/
	}

	bool __fastcall hkTestBeamHits(void* This, void* Start, void* Dir, void* Results, float Size)
	{
		bool Pass = TFD::nativeTestBeamHits(This, Start, Dir, Results, Size);
		std::cout << "Test beam: " << (int)Pass << "\n";
		return Pass;
	}

	void hkServerRequestProcessInteraction(void* This, TFD::FM1TemplateId* ID, int ActorUniqueID, TFD::AActor* InNpc)
	{
		std::cout << "interact: " << ID->ID << " - " << ActorUniqueID << " - " << std::hex << static_cast<void*>(InNpc) << std::dec << "\n";

		return TFD::ServerRequestProcessInteraction(This, ID, ActorUniqueID, InNpc);
	}

#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
	void PrintStackTrace()
	{
		void* stack[64];
		USHORT frames = CaptureStackBackTrace(0, 64, stack, nullptr);

		HANDLE process = GetCurrentProcess();

		for (USHORT i = 0; i < frames; i++)
		{
			DWORD64 addr = (DWORD64)stack[i];

			HMODULE hModule = NULL;
			if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)addr, &hModule))
			{
				char moduleName[MAX_PATH];
				if (GetModuleFileNameA(hModule, moduleName, MAX_PATH))
				{
					// Get offset from module base
					DWORD64 base = (DWORD64)hModule;
					DWORD64 offset = addr - base;

					// Extract just the file name (optional)
					const char* fileName = strrchr(moduleName, '\\');
					if (!fileName) fileName = moduleName;
					else fileName++;

					std::cout << i << ": " << fileName
						<< "!0x" << std::hex << offset << std::dec << "\n";
				}
				else
				{
					std::cout << i << ": [Unknown Module]!0x" << std::hex << addr << std::dec << "\n";
				}
			}
			else
			{
				std::cout << i << ": [No Module]!0x" << std::hex << addr << std::dec << "\n";
			}
		}
	}

	bool hkRequestProcessInteractionCheckA(void* This, int ID)
	{
		PrintStackTrace();
		if (ResupplyNPC != nullptr)
			std::cout << "got npc\n";
		std::cout << "hkRequestProcessInteractionCheckA\n";
		return true;
	}
	bool hkRequestProcessInteractionCheckB(void* This, TFD::AActor* NPC)
	{
		PrintStackTrace();
		if (ResupplyNPC != nullptr)
			std::cout << "got npc\n";
		std::cout << "hkRequestProcessInteractionCheckB\n";
		return true;
	}
}
