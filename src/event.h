#pragma once
#include <vector>

namespace TChapman500
{
	class event_listener { };	// DO NOT PUT THIS IN THE INHERITANCE LIST
	typedef void (event_listener:: *event_function)(void *);
	typedef void (*static_function)(void *);

	class event
	{
	public:
		event() { }

		void subscribe(static_function function)
		{
			for (const sub_entry &entry : SubscriberList)
			{
				if (!entry.Subscriber && entry.Function == function)
					return;
			}

			// Add the subscriber to the list.
			sub_entry newEntry;
			newEntry.Subscriber = nullptr;
			newEntry.Function = function;
			SubscriberList.push_back(newEntry);
		}
		void subscribe(event_listener *subscriber, event_function function)
		{
			// Search for subscriber and function in list.
			for (const sub_entry &entry : SubscriberList)
			{
				if (entry.Subscriber == subscriber && entry.MemFunction == function) return;
			}

			// Add the subscriber to the list.
			sub_entry newEntry;
			newEntry.Subscriber = subscriber;
			newEntry.MemFunction = function;
			SubscriberList.push_back(newEntry);
		}

		void unsubscribe(event_listener *subscriber, event_function function)
		{
			// Search for subscriber and function in list.
			for (size_t i = 0; i < SubscriberList.size(); i++)
			{
				// Found it.
				if (SubscriberList[i].Subscriber == subscriber && SubscriberList[i].MemFunction == function)
				{
					// Remove subscriber from the list.
					SubscriberList.erase(SubscriberList.begin() + i);
					break;
				}
			}
		}
		void unsubscribe(static_function function)
		{
			// Search for subscriber and function in list.
			for (size_t i = 0; i < SubscriberList.size(); i++)
			{
				// Found it.
				if (!SubscriberList[i].Subscriber && SubscriberList[i].Function == function)
				{
					// Remove subscriber from the list.
					SubscriberList.erase(SubscriberList.begin() + i);
					break;
				}
			}
		}

		// Fire event with no data appended to it.
		void fire()
		{
			// Call all functions subscribed to the event.
			for (const sub_entry &entry : SubscriberList)
			{
				if (entry.Subscriber) (entry.Subscriber->*entry.MemFunction)(nullptr);
				else entry.Function(nullptr);
			}
		}

		// Fire event with data appended to it.
		void fire(void *data)
		{
			// Call all functions subscribed to the event.
			for (const sub_entry &entry : SubscriberList)
			{
				if (entry.Subscriber) (entry.Subscriber->*entry.MemFunction)(data);
				else entry.Function(data);
			}
		}

	private:
		struct sub_entry
		{
			event_listener *Subscriber;
			union
			{
				event_function MemFunction;
				static_function Function;
			};
		};
		std::vector<sub_entry> SubscriberList;
	};
}
