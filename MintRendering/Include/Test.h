#pragma once


#ifndef _MINT_RENDERING_TEST_H_
#define _MINT_RENDERING_TEST_H_


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
	}


	namespace TestRendering
	{
		bool Test(Rendering::GraphicDevice& graphicDevice);

		bool Test_SplineGenerator(Rendering::GraphicDevice& graphicDevice);
		bool Test_Plotter(Rendering::GraphicDevice& graphicDevice);
		bool Test_MathExpressionRenderer(Rendering::GraphicDevice& graphicDevice);
		bool Test_GUI(Rendering::GraphicDevice& graphicDevice);
	}
}


#endif // !_MINT_RENDERING_TEST_H_
