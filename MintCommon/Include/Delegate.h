#pragma once


#ifndef _MINT_COMMON_DELEGATE_H_
#define _MINT_COMMON_DELEGATE_H_


namespace mint
{
	template<typename... Args>
	class Delegate
	{
	public:
		Delegate() = default;
		~Delegate() = default;

	public:
		bool operator==(const Delegate& other) const
		{
			return _stub == other._stub && _objectPtr == other._objectPtr && _functionPtr == other._functionPtr;
		}

	public:
		// ��� �Լ� ���ε�
		template<typename T, void (T::* Method)(Args...)>
		void Bind(T* instance)
		{
			_stub = &MethodStub<T, Method>;
			_objectPtr = static_cast<void*>(instance);
			_functionPtr = nullptr;
		}

		// ���� �Լ� �Ǵ� static �Լ� ���ε�
		void Bind(void (*function)(Args...))
		{
			_stub = &FunctionStub;
			_objectPtr = nullptr;
			_functionPtr = reinterpret_cast<void*>(function);
		}

		void Invoke(Args... args) const
		{
			if (_stub) _stub(_objectPtr, _functionPtr, args...);
		}

		bool IsBound() const { return _stub != nullptr; }

		void Unbind()
		{
			_stub = nullptr;
			_objectPtr = nullptr;
			_functionPtr = nullptr;
		}

	private:
		using StubFunc = void(*)(void*, void*, Args...);
		StubFunc _stub = nullptr;
		void* _objectPtr = nullptr;
		void* _functionPtr = nullptr;

	private:
		// ��� �Լ� ȣ�� stub
		template<typename T, void (T::* Method)(Args...)>
		static void MethodStub(void* obj, void*, Args... args)
		{
			T* instance = static_cast<T*>(obj);
			(instance->*Method)(args...);
		}

		// ���� �Լ� ȣ�� stub
		static void FunctionStub(void*, void* func, Args... args)
		{
			auto function = reinterpret_cast<void(*)(Args...)>(func);
			function(args...);
		}
	};
}


#endif // !_MINT_COMMON_DELEGATE_H_
