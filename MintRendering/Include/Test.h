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
		bool test(Rendering::GraphicDevice& graphicDevice);

		bool test_SplineGenerator(Rendering::GraphicDevice& graphicDevice);
		bool test_Plotter(Rendering::GraphicDevice& graphicDevice);
		bool test_MathExpressionRenderer(Rendering::GraphicDevice& graphicDevice);
		bool test_GUI(Rendering::GraphicDevice& graphicDevice);
	}
}


#endif // !_MINT_RENDERING_TEST_H_
