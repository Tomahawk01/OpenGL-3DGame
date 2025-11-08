#pragma once

namespace Game {

	struct ComRelease
	{
		template<class T>
		void operator()(T* comPtr)
		{
			comPtr->Release();
		}
	};

}
