#pragma once


#ifndef _MINT_RENDERING_TEST_H_
#define _MINT_RENDERING_TEST_H_


namespace mint
{
	namespace Rendering
	{
		class GraphicsDevice;
		class Plotter;
	}


	namespace TestRendering
	{
		bool Test_SplineGenerator(Rendering::GraphicsDevice& graphicsDevice);
		bool Test_Plotter(Rendering::Plotter& plotter);
	}
}


#endif // !_MINT_RENDERING_TEST_H_
