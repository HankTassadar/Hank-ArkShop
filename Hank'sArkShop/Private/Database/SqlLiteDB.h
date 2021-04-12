#pragma once

#include "../hdr/sqlite_modern_cpp.h"

#include <Tools.h>

#include "IDatabase.h"
#include "../ArkShop.h"

class SqlLite : public IDatabase
{
public:
	explicit SqlLite(const std::string& path)
		: db_(path.empty()
			      ? ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Hank'sArkShop/ArkShop.db"
			      : path)
	{
		try
		{
			//db_ << "PRAGMA journal_mode=WAL;";

			db_ << "create table if not exists Players ("
				"Id integer primary key autoincrement not null,"
				"SteamId integer default 0,"
				"Groups text default 'Default,',"
				"Kits text default '{}',"
				"Points integer default 0,"
				"TotalSpent integer default 0,"
				"QQ text,"
				"AppPass text,"
				"OnlineTime integer default 0"
				");";
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
	}

	bool TryAddNewPlayer(uint64 steam_id) override
	{
		try
		{
			db_ << "INSERT INTO Players (SteamId) VALUES (?);"
				<< steam_id;
			return true;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	bool IsPlayerExists(uint64 steam_id) override
	{
		int count = 0;

		try
		{
			db_ << "SELECT count(1) FROM Players WHERE SteamId = ?;" << steam_id >> count;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return count != 0;
	}

	std::string GetPlayerKits(uint64 steam_id) override
	{
		std::string kits_config = "{}";

		try
		{
			db_ << "SELECT Kits FROM Players WHERE SteamId = ?;" << steam_id >> kits_config;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}

		return kits_config;
	}

	bool UpdatePlayerKits(uint64 steam_id, const std::string& kits_data) override
	{
		try
		{
			db_ << "UPDATE Players SET Kits = ? WHERE SteamId = ?;" << kits_data << steam_id;
			return true;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	bool DeleteAllKits() override
	{
		try
		{
			db_ << "UPDATE Players SET Kits = \"{}\";";
			return true;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	int GetPoints(uint64 steam_id) override
	{
		int points = 0;

		try
		{
			db_ << "SELECT Points FROM Players WHERE SteamId = ?;" << steam_id >> points;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}

		return points;
	}

	bool SetPoints(uint64 steam_id, int amount) override
	{
		try
		{
			db_ << "UPDATE Players SET Points = ? WHERE SteamId = ?;" << amount << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	bool AddPoints(uint64 steam_id, int amount) override
	{
		if (amount < 0)
			return false;

		try
		{
			db_ << "UPDATE Players SET Points = Points + ? WHERE SteamId = ?;" << amount << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	bool SpendPoints(uint64 steam_id, int amount) override
	{
		if (amount < 0)
			return false;

		try
		{
			db_ << "UPDATE Players SET Points = Points - ? WHERE SteamId = ?;" << amount << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	bool AddTotalSpent(uint64 steam_id, int amount) override
	{
		if (amount < 0)
			return false;

		try
		{
			db_ << "UPDATE Players SET TotalSpent = ? WHERE SteamId = ?;" << amount << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	int GetTotalSpent(uint64 steam_id) override
	{
		int points = 0;

		try
		{
			db_ << "SELECT TotalSpent FROM Players WHERE SteamId = ?;" << steam_id >> points;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}

		return points;
	}

	bool DeleteAllPoints() override
	{
		try
		{
			db_ << "UPDATE Players SET Points = 0;";
			return true;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	bool SetQQ(uint64 steam_id, std::string QQ) override
	{
		try
		{
			db_ << "UPDATE Players SET QQ = ? WHERE SteamId = ?;" << QQ << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	bool SetAppPass(uint64 steam_id, std::string Pass) override
	{
		try
		{
			db_ << "UPDATE Players SET AppPass = ? WHERE SteamId = ?;" << Pass << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	bool AddOnlineTime(uint64 steam_id, time_t time) override {
		if (time == 0)return false;
		try
		{
			db_ << "UPDATE Players SET OnlineTime = OnlineTime + ? WHERE SteamId = ?;" << (int)(time / 60) << steam_id;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}

		return true;
	}

	int GetOnlineTime(uint64 steam_id) override {
		int onlinetime = 0;

		try
		{
			db_ << "SELECT OnlineTime FROM Players WHERE SteamId = ?;" << steam_id >> onlinetime;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}

		return onlinetime;
	}

	bool IsPlayerInGroup(uint64 steam_id, const FString& group) override {
		std::string groups;
		try
		{
			db_ << "SELECT Groups FROM Players WHERE SteamId = ?;" << steam_id >> groups;
			FString Fgroups(groups);
			TArray<FString> arraygroups;
			Fgroups.ParseIntoArray(arraygroups, L",", true);
			for (const auto& i : arraygroups)
			{
				if (i == group)return true;
			}
	
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
		return false;
	}
private:
	sqlite::database db_;
};
