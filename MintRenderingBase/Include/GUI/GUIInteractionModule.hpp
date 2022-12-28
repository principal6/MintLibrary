#pragma once


#include <MintRenderingBase/Include/GUI/GUIInteractionModules.h>


namespace mint
{
	namespace Rendering
	{
		namespace GUI
		{
			template<typename Input>
			inline bool InteractionMousePressModule<Input>::isInteracting() const
			{
				return _input._controlID.isValid();
			}

			template<typename Input>
			inline bool InteractionMousePressModule<Input>::isInteractingWith(const ControlID& controlID) const
			{
				return _input._controlID == controlID;
			}

			template<typename Input>
			inline bool InteractionMousePressModule<Input>::begin(const Input& input)
			{
				if (isInteracting())
				{
					return false;
				}

				_input = input;
				return true;
			}

			template<typename Input>
			inline void InteractionMousePressModule<Input>::end()
			{
				_input._controlID.invalidate();
			}

			template<typename Input>
			inline Float2 InteractionMousePressModule<Input>::computeRelativeMousePressedPosition() const
			{
				return _input._mousePressedPosition - _input._controlPosition;
			}
		}
	}
}
