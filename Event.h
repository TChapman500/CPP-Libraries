#pragma once
#include <vector>

namespace TChapman500
{
	class EventListener
	{
	public:
		virtual ~EventListener() {}
	};

	typedef void (EventListener:: *Function)(void *);

	class Event
	{
	public:
		Event();
		~Event();

		void AddListener(EventListener *listener, Function function);
		void RemoveListener(EventListener *listener, Function function);

		void Fire();
		void Fire(void *data);

	private:
		std::vector<EventListener *> Listeners;
		std::vector<Function> Functions;
	};
}
