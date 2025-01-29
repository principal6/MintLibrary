#include <MintGUI/Include/GUIComponents.h>
#include <MintGUI/Include/GUIComponents.hpp>


namespace mint
{
	namespace GUI
	{
#if defined(MINT_DEBUG)
		GUIComponentPoolRegistry* GUIComponentPoolRegistry::_sInstance = nullptr;
#endif // defined(MINT_DEBUG)
		GUIComponentPoolRegistry::GUIComponentPoolRegistry()
		{
			__noop;
		}

		GUIComponentPoolRegistry::~GUIComponentPoolRegistry()
		{
			__noop;
		}

		GUIComponentPoolRegistry& GUIComponentPoolRegistry::GetInstance()
		{
			static GUIComponentPoolRegistry sIntance;
#if defined(MINT_DEBUG)
			GUIComponentPoolRegistry::_sInstance = &sIntance;
#endif // defined(MINT_DEBUG)
			return sIntance;
		}

		void GUIComponentPoolRegistry::RegisterComponentPool(IGUIComponentPool& componentPool)
		{
			for (const auto& iter : _componentPools)
			{
				if (iter == &componentPool)
				{
					MINT_ASSERT(false, "This component pool is already registered!!!");
					return;
				}
			}

			_componentPools.PushBack(&componentPool);
		}

		const Vector<IGUIComponentPool*>& GUIComponentPoolRegistry::GetComponentPools() const
		{
			return _componentPools;
		}
	}
}
