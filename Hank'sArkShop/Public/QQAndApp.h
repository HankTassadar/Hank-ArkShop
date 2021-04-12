#pragma once
#include"Base.h"

namespace ArkShop::QQAndApp {
	class OnlineTime {
	private:
		OnlineTime();
	public:
		static OnlineTime& Get();
		void playerOnline(uint64 steamid);
		void playerOffline(uint64 steamid);
		void addOnlineTimeEvery10min();
	private:
		int _count;
		std::map<uint64, time_t> _onlineplayers;
	};
	void Init();
	void Unload();
}
