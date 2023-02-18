#pragma once


#include <MintRenderingBase/Include/GUI/GUIInteractionModules.h>


namespace mint
{
	namespace Rendering
	{
		namespace GUI
		{
			template<typename Input>
			inline bool InteractionMousePressModule<Input>::IsInteracting() const
			{
				return _input._controlID.IsValid();
			}

			template<typename Input>
			inline bool InteractionMousePressModule<Input>::IsInteractingWith(const ControlID& controlID) const
			{
				return _input._controlID == controlID;
			}

			template<typename Input>
			inline bool InteractionMousePressModule<Input>::Begin(const Input& input)
			{
				if (IsInteracting())
				{
					return false;
				}

				_input = input;
				return true;
			}

			template<typename Input>
			inline void InteractionMousePressModule<Input>::End()
			{
				_input._controlID.Invalidate();
			}

			template<typename Input>
			inline Float2 InteractionMousePressModule<Input>::ComputeRelativeMousePressedPosition() const
			{
				return _input._mousePressedPosition - _input._controlPosition;
			}
		}
	}
}
