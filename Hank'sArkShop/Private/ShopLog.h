#pragma once

#include "Logger/spdlog/spdlog.h"
#include <Tools.h>
#include <iostream>

class ShopLog
{
public:
	ShopLog(const ShopLog&) = delete;
	ShopLog(ShopLog&&) = delete;
	ShopLog& operator=(const ShopLog&) = delete;
	ShopLog& operator=(ShopLog&&) = delete;

	static ShopLog& Get()
	{
		static ShopLog instance;
		return instance;
	}

	static std::shared_ptr<spdlog::logger>& GetLog()
	{
		return Get().logger_;
	}

private:
	ShopLog()
	{
		try
		{
			FString map_name;
			ArkApi::GetApiUtils().GetShooterGameMode()->GetMapName(&map_name);

			auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Hank'sArkShop/ShopLog_" +
				ArkApi::Tools::Utf8Encode(*map_name) + ".log",
				1024 * 1024, 5);

			logger_ = std::make_shared<spdlog::logger>("Hank'sArkShop", sink);

			logger_->set_pattern("%D %R [%l] %v");
			logger_->flush_on(spdlog::level::info);
		}
		catch (const std::exception&)
		{
			std::cout << "Failed to create log file\n";
		}
	}

	~ShopLog() = default;

	std::shared_ptr<spdlog::logger> logger_;
};

//#define DEBUGLOGOPEN 
#ifdef DEBUGLOGOPEN
#define DEBUGLOG(str) \
	ShopLog::GetLog()->info(str);

#endif // LODEBUGLOG
