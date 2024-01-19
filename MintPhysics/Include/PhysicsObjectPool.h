#pragma once


#ifndef _MINT_PHYSICS_PHYSICS_OBJECT_POOL_H_
#define _MINT_PHYSICS_PHYSICS_OBJECT_POOL_H_


#include <MintContainer/Include/Vector.h>


namespace mint
{
	namespace Physics
	{
		template<typename T>
		class PhysicsObjectPool
		{
		public:
			PhysicsObjectPool()
			{
				_objects.Resize(8);
			}
			~PhysicsObjectPool() = default;

		public:
			void Create(const uint32 slotIndex, T&& object)
			{
				MINT_ASSERT(IsFull() == false, "!!!");
				_objects[slotIndex] = object;
			}
			void Destroy(const uint32 slotIndex)
			{
				MINT_ASSERT(slotIndex < _objects.Size(), "!!!");
				_objects[slotIndex].Invalidate();
				_nextSlotIndex = Min(_nextSlotIndex, slotIndex);
			}

		public:
			bool IsFull() const { return _nextSlotIndex == kInvalidIndexUint32; }
			void GrowIfFull()
			{
				if (IsFull() == false)
				{
					return;
				}

				const uint32 oldSize = _objects.Size();
				MINT_ASSERT(oldSize > 0, "!!!");
				_objects.Resize(oldSize * 2);
				_nextSlotIndex = oldSize;
			}
			uint32 GetNextSlotIndex()
			{
				UpdateNextSlotIndex();
				return _nextSlotIndex;
			}
			T& GetObject_(const uint32 index) { MINT_ASSERT(index < _objects.Size(), "!!!"); return _objects[index]; }
			const T& GetObject_(const uint32 index) const { MINT_ASSERT(index < _objects.Size(), "!!!"); return _objects[index]; }
			const Vector<T>& GetObjects() const { return _objects; }
			uint32 GetObjectCount() const { return _objects.Size(); }

		private:
			void UpdateNextSlotIndex()
			{
				const uint32 objectCount = _objects.Size();
				for (uint32 i = _nextSlotIndex; i < objectCount; ++i)
				{
					if (_objects[i].IsValid() == false)
					{
						_nextSlotIndex = i;
						return;
					}
				}
				_nextSlotIndex = kInvalidIndexUint32;
			}

		private:
			uint32 _nextSlotIndex = 0;
			Vector<T> _objects;
		};
	}
}


#endif // !_MINT_PHYSICS_PHYSICS_OBJECT_POOL_H_
