#include <memory>
#include <vector>

namespace TChapman500
{
	class IState
	{
	public:
		virtual ~IState();
		
		virtual bool Initialize(IState *context) = 0;
		virtual void Execute(IState *context) = 0;
		virtual void CleanUp(IState *context) = 0;
	};
	
	class NullState : public IState
	{
	public:
		NullState();
		~NullState();
		
		virtual bool Initialize(IState *context) final;
		virtual void Execute(IState *context) final;
		virtual void CleanUp(IState *context) final;
	};
	
	class StateMachine : public IState
	{
	public:
		StateMachine();
		StateMachine(std::shared_ptr<IState> state);
		~StateMachine();
		
		static std::shared_ptr<NullState> GetNullState();
		
		virtual bool Initialize(IState *context) override;
		virtual void Execute(IState *context) override;
		virtual void CleanUp(IState *context) override;
		
		virtual bool SetState(std::shared_ptr<IState> state);
		std::shared_ptr<IState> GetState();
		
	private:
		static std::shared_ptr<NullState> _NullState;
		std::shared_ptr<IState> _State;
		std::shared_ptr<IState> _NewState;
	};
	
	class StateMachineEx : public StateMachine
	{
	public:
		StateMachineEx();
		StateMachineEx(std::shared_ptr<IState> state);
		~StateMachineEx();
		
		virtual void Execute(IState *context) override;
		virtual void CleanUp(IState *context) override;
		
		virtual bool AddChild(std::shared_ptr<IState> child, int order);
		virtual bool RemoveChild(std::shared_ptr<IState> child);
		virtual bool MoveChild(std::shared_ptr<IState> child, int order);
		
	private:
		void _AddChild(std::shared_ptr<IState> child, int order);
		
		std::vector<std::pair<std::shared_ptr<IState>, int>> _Children;
		std::vector<std::pair<std::shared_ptr<IState>, int>> _NewChildren;
		std::vector<std::pair<std::shared_ptr<IState>, int>> _ReorderedChildren;
		std::vector<std::pair<std::shared_ptr<IState>, int>> _RemovedChildren;
	};
}

