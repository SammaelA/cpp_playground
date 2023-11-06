#include <type_traits>
#include <cstdint>
#include <cinttypes>

template <typename T, typename... Ts>
struct are_same : std::conjunction<std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::remove_cv_t<std::remove_reference_t<Ts>>>...> {};

template< typename... Args >
inline constexpr bool are_same_v = are_same<Args...>::value;

int main()
{
  static_assert(are_same_v<int, int32_t, signed int>, "compile assert");
  return 0;
}