#pragma once

template<typename Enum>
inline Enum operator | (Enum Lhs, Enum Rhs)
{
	static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");

	using Underlying = typename std::underlying_type<Enum>::type;

	return static_cast<Enum> (
		static_cast<Underlying>(Lhs) |
		static_cast<Underlying>(Rhs));
}