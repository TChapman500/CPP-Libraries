#include "pch.h"
#include "Event.h"

Event::Event()
{

}

Event::~Event()
{

}

void Event::AddListener(EventListener *listener, Function function)
{
	bool found = false;

	for (int i = 0; i < (int)Listeners.size(); i++)
	{
		if (Listeners[i] == listener && Functions[i] == function)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		Listeners.push_back(listener);
		Functions.push_back(function);
	}
}

void Event::RemoveListener(EventListener *listener, Function function)
{
	for (int i = 0; i < (int)Listeners.size(); i++)
	{
		if (Listeners[i] == listener && Functions[i] == function)
		{
			Listeners.erase(Listeners.begin() + i);
			Functions.erase(Functions.begin() + i);
			break;
		}
	}
}

void Event::Fire() { for (int i = 0; i < (int)Listeners.size(); i++) (Listeners[i]->*Functions[i])(nullptr); }
void Event::Fire(void *data) { for (int i = 0; i < (int)Listeners.size(); i++) (Listeners[i]->*Functions[i])(data); }
