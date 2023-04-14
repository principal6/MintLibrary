#pragma once


#ifndef _MINT_RENDERING_TEST_H_
#define _MINT_RENDERING_TEST_H_


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		class Plotter;
		class MathExpressionRenderer;


		namespace GUI
		{
			class GUIContext;
		}
	}


	namespace TestRendering
	{
		bool Test_SplineGenerator(Rendering::GraphicDevice& graphicDevice);
		bool Test_Plotter(Rendering::Plotter& plotter);
		bool Test_MathExpressionRenderer(Rendering::MathExpressionRenderer& mathExpressionRenderer);
		bool Test_GUI(Rendering::GUI::GUIContext& guiContext);
	}
}


#endif // !_MINT_RENDERING_TEST_H_
