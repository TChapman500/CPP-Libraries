#include "state_machine.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;
using std::pair;

namespace TChapman500 {

// Interface does nothing
IState::~IState() {}

// Null state does nothing but provide an intantiatable object
NullState::NullState() {}
NullState::~NullState() {}
bool NullState::Initialize(IState *context) { return true; }
void NullState::Execute(IState *context) {}
void NullState::CleanUp(IState *context) {}

shared_ptr<NullState> StateMachine::_NullState = make_shared<NullState>();

StateMachine::StateMachine()
{
	_State = _NullState;
	_NewState = nullptr;
}

StateMachine::StateMachine(shared_ptr<IState> state)
{
	if (!state || state.get() == this) _State = _NullState;
	else _State = state;
}

StateMachine::~StateMachine()
{
	
}

shared_ptr<NullState> StateMachine::GetNullState() { return _NullState; }

bool StateMachine::Initialize(IState *context)
{
	_State->Initialize(this);
	return true;
}

void StateMachine::Execute(IState *context)
{
	// Initailize the new state
	if (_NewState)
	{
		_State = _NewState;
		_State->Initialize(this);
		_NewState = nullptr;
	}
	
	// Execute the current state
	_State->Execute(this);
	
	// Clean up old state
	if (_NewState)
	{
		_State->CleanUp(this);
		_State = _NullState;
	}
}

void StateMachine::CleanUp(IState *context) { _State->CleanUp(this); };

bool StateMachine::SetState(shared_ptr<IState> state)
{
	if (state == _State || state.get() == this) return false;
	
	if (state) _NewState = state;
	else _NewState = _NullState;

	return true;
}

shared_ptr<IState> StateMachine::GetState() { return _State; }



StateMachineEx::StateMachineEx() {}
StateMachineEx::StateMachineEx(shared_ptr<IState> state) : StateMachine(state) {}
StateMachineEx::~StateMachineEx() {}

void StateMachineEx::Execute(IState *context)
{
	// Add new children
	if (!_NewChildren.empty())
	{
		for (size_t i = 0; i < _NewChildren.size(); i++)
		{
			const pair<shared_ptr<IState>, int> &child = _NewChildren[i];
			_AddChild(child.first, child.second);
			child.first->Initialize(this);
		}
		_NewChildren.clear();
	}
	
	// Move children
	if (!_ReorderedChildren.empty())
	{
		// Iterate through the list to move all children to their new locations.
		for (const pair<shared_ptr<IState>, int> &order : _ReorderedChildren)
		{
			// Temporarilly remove child from the list.
			for (size_t i = 0; i < _Children.size(); i++)
			{
				if (_Children[i].first == order.first)
				{
					_Children.erase(_Children.begin() + i);
					break;
				}
			}
			
			// Re-add the child to a different location on the list.
			_AddChild(order.first, order.second);
		}
		
		// We're done here.
		_ReorderedChildren.clear();
	}
	
	// Execute current state
	StateMachine::Execute(this);
	
	// Execute child state machines
	for (const pair<shared_ptr<IState>, int> &child : _Children) child.first->Execute(this);
	
	// Remove children
	if (!_RemovedChildren.empty())
	{
		// Remove each child on the remove list.
		for (const pair<shared_ptr<IState>, int> &child : _RemovedChildren)
		{
			// Found the child to remove from the list.
			for (size_t i = 0; i < _Children.size(); i++)
			{
				const pair<shared_ptr<IState>, int> &xChild = _Children[i];
				if (xChild.first == child.first)
				{
					_Children.erase(_Children.begin() + i);
					break;
				}
			}
			
			// Let the child state clean up.
			child.first->CleanUp(this);
		}
		
		// Done removing all the children on the list.
		_RemovedChildren.clear();
	}
}

void StateMachineEx::CleanUp(IState *context)
{
	StateMachine::CleanUp(context);
	for (const pair<shared_ptr<IState>, int> &state : _Children)
		state.first->CleanUp(this);
}

bool StateMachineEx::AddChild(shared_ptr<IState> child, int order)
{
	if (!child || child.get() == this || child == GetNullState()) return false;
	
	// Check to see if the child already exists
	for (const pair<shared_ptr<IState>, int> &iChild : _Children)
	{
		if (iChild.first == child)
			return false;
	}
	
	// Check to see if the child is already being added
	for (const pair<shared_ptr<IState>, int> &iChild : _NewChildren)
	{
		if (iChild.first == child)
			return false;
	}
	
	// Add the child to the list
	for (size_t i = 0; i < _NewChildren.size(); i++)
	{
		const pair<shared_ptr<IState>, int> &iChild = _NewChildren[i];
		
		// Insert child into the list
		if (order < iChild.second)
		{
			_NewChildren.insert(_NewChildren.begin() + i, pair<shared_ptr<IState>, int>(child, order));
			return true;
		}
	}
	
	// Append the child to the end of the list
	_NewChildren.push_back(pair<shared_ptr<IState>, int>(child, order));
	return true;
}

void StateMachineEx::_AddChild(shared_ptr<IState> child, int order)
{
	// Add the child to the list
	for (size_t i = 0; i < _Children.size(); i++)
	{
		const pair<shared_ptr<IState>, int> &iChild = _Children[i];
		
		// Insert child into the list
		if (order < iChild.second)
			_Children.insert(_Children.begin() + i, pair<shared_ptr<IState>, int>(child, order));
	}
	
	// Append the child to the end of the list
	_Children.push_back(pair<shared_ptr<IState>, int>(child, order));
}

bool StateMachineEx::RemoveChild(shared_ptr<IState> child)
{
	// Child is already being removed.
	for (const pair<shared_ptr<IState>, int> &xChild : _RemovedChildren)
	{
		if (xChild.first == child)
			return false;
	}
	
	// Search for child
	for (const pair<shared_ptr<IState>, int> &xChild : _Children)
	{
		if (xChild.first == child)
		{
			// Remove from reorder list
			for (size_t i = 0; i < _ReorderedChildren.size(); i++)
			{
				if (_ReorderedChildren[i].first == child)
				{
					_ReorderedChildren.erase(_ReorderedChildren.begin() + i);
					break;
				}
			}
			
			// Add child to the remove list
			for (size_t i = 0; i < _RemovedChildren.size(); i++)
			{
				// Make sure that it's in the right order
				if (xChild.second < _RemovedChildren[i].second)
				{
					_RemovedChildren.insert(_RemovedChildren.begin() + i, pair<shared_ptr<IState>, int>(child, xChild.second));
					return true;
				}
			}
			
			// Add to the end of the list instead.
			_RemovedChildren.push_back(pair<shared_ptr<IState>, int>(child, xChild.second));
			return true;
		}
	}
	
	// Child not found
	return false;
}

bool StateMachineEx::MoveChild(shared_ptr<IState> child, int order)
{
	// Check if the child is already being moved
	for (const pair<shared_ptr<IState>, int> &xChild : _ReorderedChildren)
	{
		if (xChild.first == child)
			return false;
	}
	
	// Check if the child is on the "new children" list
	for (const pair<shared_ptr<IState>, int> &xChild : _NewChildren)
	{
		if (xChild.first == child)
			return false;
	}
	
	// Check if the child is on the remove child list
	for (const pair<shared_ptr<IState>, int> &xChild : _RemovedChildren)
	{
		if (xChild.first == child)
			return false;
	}
	
	// Check if the child is on the child list
	for (const pair<shared_ptr<IState>, int> &xChild : _Children)
	{
		if (xChild.first == child)
		{
			_ReorderedChildren.push_back(pair<shared_ptr<IState>, int>(child, order));
			return true;
		}
	}
	
	// Child not found
	return false;
}

}