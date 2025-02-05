#pragma once

namespace Game {

	template <class T>
	class PassKey
	{
		friend T;
		PassKey() = default;
	};

}
