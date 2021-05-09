#pragma once
#include <vector>

namespace TChapman500
{
	class EventSubscriber { };
	typedef void (EventSubscriber:: *EventFunction)(void *);

	class Event
	{
	public:
		Event() { }

		void Subscribe(EventSubscriber *subscriber, EventFunction function)
		{
			// Search for subscriber and function in list.
			size_t subscribers = SubscriberList.size();
			for (size_t i = 0; i < subscribers; i++)
			{
				if (SubscriberList[i].first == subscriber && SubscriberList[i].second == function) return;
			}

			// Add the subscriber to the list.
			SubscriberList.push_back(std::pair<EventSubscriber*, EventFunction>(subscriber, function));
		}

		void Unsubscribe(EventSubscriber *subscriber, EventFunction function)
		{
			// Search for subscriber and function in list.
			size_t subscribers = SubscriberList.size();
			for (size_t i = 0; i < subscribers; i++)
			{
				// Found it.
				if (SubscriberList[i].first == subscriber && SubscriberList[i].second == function)
				{
					// Remove subscriber from the list.
					SubscriberList.erase(SubscriberList.begin() + i);
					break;
				}
			}
		}

		// Fire event with no data appended to it.
		void Fire()
		{
			// Get the number of subscribers.
			size_t subscribers = SubscriberList.size();

			// Call all functions subscribed to the event.
			for (size_t i = 0; i < subscribers; i++) (SubscriberList[i].first->*SubscriberList[i].second)(nullptr);
		}

		// Fire event with data appended to it.
		void Fire(void *data)
		{
			// Get the number of subscribers.
			size_t subscribers = SubscriberList.size();

			// Call all functions subscribed to the event.
			for (size_t i = 0; i < subscribers; i++) (SubscriberList[i].first->*SubscriberList[i].second)(data);
		}

		// Do we need to do something here?
		~Event() {}

	private:
		std::vector<std::pair<EventSubscriber *, EventFunction>> SubscriberList;
	};
}
