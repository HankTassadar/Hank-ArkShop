#include"QQAndApp.h"

#include <DBHelper.h>
#include <Points.h>

#include "ArkShop.h"
#include "ShopLog.h"


namespace ArkShop::QQAndApp {

	void GetOnlineTime(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
		if (!IsStoreEnabled(player_controller))
		{
			return;
		}
		const uint64 steam_id = ArkApi::IApiUtils::GetSteamIdFromController(player_controller);
		auto onlinetime = database->GetOnlineTime(steam_id);
		ArkApi::GetApiUtils().SendChatMessage(player_controller, GetText("Sender"), *GetText("GetOnlineTimeOK"), onlinetime);
	}

	void SetQQ(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
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

	void SetAppPass(AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type /*unused*/) {
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

	void Init()
	{
		auto& commands = ArkApi::GetCommands();

		commands.AddChatCommand(GetText("SetQQCmd"), &SetQQ);
		commands.AddChatCommand(GetText("SetPassCmd"), &SetAppPass);
		commands.AddChatCommand(GetText("GetOnlineTime"), &GetOnlineTime);

	}

	void Unload()
	{
		auto& commands = ArkApi::GetCommands();

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
		// TODO: �ڴ˴����� return ���
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

