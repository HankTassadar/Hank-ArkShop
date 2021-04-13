#include"QQAndApp.h"

#include <DBHelper.h>
#include <Points.h>

#include "ArkShop.h"
#include "ShopLog.h"


namespace ArkShop::QQAndApp {

	void KillPlayerSelf(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
		
		try {
			
		if (ArkApi::GetApiUtils().IsPlayerDead(player_controller))
			return;

		if (ArkApi::GetApiUtils().IsRidingDino(player_controller)) {
			ArkApi::GetApiUtils().SendNotification(player_controller, FLinearColor(0, 255, 0), 5, 5, nullptr
				, ArkApi::Tools::Utf8Decode(GetText("CannotKillCauseDino").ToString()).c_str());
			return;
		}
		auto* cheat_manager = static_cast<UShooterCheatManager*>(player_controller->CheatManagerField());
		
		cheat_manager->KillPlayer(ArkApi::GetApiUtils().GetPlayerID(player_controller));
		
		}
		catch (const std::exception& e) {
			Log::GetLog()->error(e.what());
		}
		
	}

	void GetOnlineTime(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
		try {
			if (!IsStoreEnabled(player_controller))
			{
				return;
			}
			const uint64 steam_id = ArkApi::IApiUtils::GetSteamIdFromController(player_controller);
			auto onlinetime = database->GetOnlineTime(steam_id);
			ArkApi::GetApiUtils().SendChatMessage(player_controller, GetText("Sender"), *GetText("GetOnlineTimeOK"), onlinetime);
		}
		catch (const std::exception& e) {
			Log::GetLog()->error(e.what());
		}
	}

	void SetQQ(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
		try {
		if (!IsStoreEnabled(player_controller))
		{
			return;
		}
		const uint64 steam_id = ArkApi::IApiUtils::GetSteamIdFromController(player_controller);
		TArray<FString> parsed;
		message->ParseIntoArray(parsed, L" ", true);
		if (database->SetQQ(steam_id, parsed[1].ToString())) {
			ArkApi::GetApiUtils().SendChatMessage(player_controller, GetText("Sender"),
				*GetText("SetQQOK"));
		}
		}
		catch (const std::exception& e) {
			Log::GetLog()->error(e.what());
		}
		
	}

	void SetAppPass(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
		try {
			if (!IsStoreEnabled(player_controller))
			{
				return;
			}
		const uint64 steam_id = ArkApi::IApiUtils::GetSteamIdFromController(player_controller);
		TArray<FString> parsed;
		message->ParseIntoArray(parsed, L" ", true);
		if (database->SetAppPass(steam_id, parsed[1].ToString())) {
			ArkApi::GetApiUtils().SendChatMessage(player_controller, GetText("Sender"),
				*GetText("SetPassOK"));
			}
		}
		catch (const std::exception& e) {
			Log::GetLog()->error(e.what());
		}
	}

	void Init()
	{
		auto& commands = ArkApi::GetCommands();

		commands.AddChatCommand(GetText("KillCmd"), &KillPlayerSelf);
		commands.AddChatCommand(GetText("SetQQCmd"), &SetQQ);
		commands.AddChatCommand(GetText("SetPassCmd"), &SetAppPass);
		commands.AddChatCommand(GetText("GetOnlineTime"), &GetOnlineTime);

	}

	void Unload()
	{
		auto& commands = ArkApi::GetCommands();

		commands.RemoveChatCommand(GetText("KillCmd"));
		commands.RemoveChatCommand(GetText("SetQQCmd"));
		commands.RemoveChatCommand(GetText("SetPassCmd"));
		commands.RemoveChatCommand(GetText("GetOnlineTime"));

	}

	OnlineTime::OnlineTime() 
		:_count(0)
	{
		ArkApi::GetCommands().AddOnTimerCallback("OnlineTime", std::bind(&OnlineTime::addOnlineTimeEvery10min, this));
	}

	OnlineTime& OnlineTime::Get()
	{
		static OnlineTime onlinetime;
		return onlinetime;
		// TODO: 在此处插入 return 语句
	}

	void OnlineTime::playerOnline(uint64 steamid)
	{
		this->_onlineplayers[steamid] = time(NULL);
	}

	void OnlineTime::playerOffline(uint64 steamid)
	{
		time_t timeonline = time(NULL) - this->_onlineplayers[steamid];
		if (timeonline < 60)timeonline = 0;
		database->AddOnlineTime(steamid, timeonline);
		this->_onlineplayers.erase(steamid);
	}
	void OnlineTime::addOnlineTimeEvery10min()
	{
		if (this->_count++ == 600) {
			this->_count = 0;
			auto timenow = time(NULL);
			for (auto& i : this->_onlineplayers) {
				database->AddOnlineTime(i.first, timenow - i.second);
				i.second = timenow;
			}
		}
	}
}

