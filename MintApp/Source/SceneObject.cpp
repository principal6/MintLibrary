#include <MintApp/Include/SceneObject.h>
#include <MintApp/Include/SceneObject.hpp>


namespace mint
{
#if defined(MINT_DEBUG)
	SceneObjectComponentPoolRegistry* SceneObjectComponentPoolRegistry::_sInstance = nullptr;
#endif // defined(MINT_DEBUG)
	inline SceneObjectComponentPoolRegistry::SceneObjectComponentPoolRegistry()
	{
		__noop;
	}

	inline SceneObjectComponentPoolRegistry::~SceneObjectComponentPoolRegistry()
	{
		__noop;
	}

	inline SceneObjectComponentPoolRegistry& SceneObjectComponentPoolRegistry::GetInstance()
	{
		static SceneObjectComponentPoolRegistry sIntance;
#if defined(MINT_DEBUG)
		SceneObjectComponentPoolRegistry::_sInstance = &sIntance;
#endif // defined(MINT_DEBUG)
		return sIntance;
	}

	inline void SceneObjectComponentPoolRegistry::RegisterComponentPool(ISceneObjectComponentPool& componentPool)
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

	inline const Vector<ISceneObjectComponentPool*>& SceneObjectComponentPoolRegistry::GetComponentPools() const
	{
		return _componentPools;
	}
}
