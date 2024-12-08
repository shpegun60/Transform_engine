#ifndef ___MATH_TRANSFORM_TRANSFORM_H_
#define ___MATH_TRANSFORM_TRANSFORM_H_

#include "basic_types.h"
#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "Span.h"

#if __cplusplus > 201703L
#include <span>
#endif /* __cplusplus > 201703L */

// Template to check if type is std::array
template <typename T>
struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_array_v = is_std_array<T>::value;

// Template to check if type is std::vector
template <typename T>
struct is_std_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
constexpr bool is_std_vector_v = is_std_vector<T>::value;


// Break marker class
class Break {
public:
    Break() = default;
};

// Main Transform class
template<reg N, typename ResultType, bool UseFlags = true, typename... Transforms>
class Transform {
    static_assert(N > 0, "N must be more than 0.");
    static_assert(sizeof...(Transforms) <= 32, "Maximum number of transforms is limited to 32.");
    static_assert(std::is_arithmetic_v<ResultType>, "ResultType must be an arithmetic type.");

public:
    Transform() : m_transforms(), m_flags(0xFFFFFFFF), m_postBreakComputed(false) {}

    explicit Transform(Transforms... transforms)
        : m_transforms(std::forward<Transforms>(transforms)...), m_flags(0xFFFFFFFF), m_postBreakComputed(false) {}

    template<typename... TransformsArg>
    explicit constexpr Transform(std::tuple<TransformsArg...> transforms)
        : m_transforms(std::move(transforms)), m_flags(0xFFFFFFFF), m_postBreakComputed(false) {}

    template<std::size_t Index>
    inline constexpr auto& get() {
        static_assert(Index < sizeof...(Transforms), "Index out of bounds.");
        return std::get<Index>(m_transforms);
    }

    template<typename TransformType>
    Transform<sizeof...(Transforms) + 1, ResultType, UseFlags, Transforms..., TransformType>
    addTransform(TransformType&& transform) const {
        static_assert(sizeof...(Transforms) < 32, "Cannot add more than 32 transformations.");

        return Transform<sizeof...(Transforms) + 1, ResultType, UseFlags, Transforms..., TransformType>(
            std::tuple_cat(m_transforms, std::make_tuple(std::forward<TransformType>(transform)))
            );
    }

    inline constexpr void setFlags(u32 flags) {
        if constexpr (UseFlags) {
            m_flags = flags;
            resetPostBreakFlag();
        }
    }

    inline constexpr bool ena(std::size_t index) {
        if constexpr (UseFlags) {
            if (index >= sizeof...(Transforms)) {
                return false;
            }
            m_flags |= (1U << index);
            resetPostBreakFlag();
        }
        return true;
    }

    template<typename Input>
    bool process(const Input& input) {
        resetPostBreakFlag();

        // check array or vector if is the same type
        if constexpr (std::is_same_v<Input, std::array<ResultType, N>>) {
            m_results = input;
        } else if constexpr (std::is_same_v<Input, std::vector<ResultType>>) {
            if (input.size() < N) {
                return false;
            }

            std::memcpy(m_results.data(), input.data(), N * sizeof(ResultType));
        }

        // check if is array or vector
        else if constexpr (is_std_vector_v<Input> || is_std_array_v<Input>) {
            if (input.size() < N) { // Checking the array size
                return false; // Array too small
            }

            for (reg i = 0; i < N; ++i) {
                m_results[i] = static_cast<ResultType>(input[i]);
            }
        } else if constexpr (std::is_array_v<Input> && std::is_same_v<std::remove_extent_t<Input>, ResultType>) {
            if constexpr (std::extent_v<Input> < N) { // Checking the array size
                return false; // Array too small
            }

            // Using std::data to get a pointer to the beginning of the array
            std::memcpy(m_results.data(), std::data(input), N * sizeof(ResultType));

        } else if constexpr (std::is_array_v<Input>) {
            if constexpr (std::extent_v<Input> < N) { // Checking the array size
                return false; // Array too small
            }

            for (reg i = 0; i < N; ++i) {
                m_results[i] = static_cast<ResultType>(input[i]);
            }
        }

        // my span class
        else if constexpr (std::is_same_v<Input, Span<ResultType>>) {
            if (input.size() < N) {
                return false;
            }

            std::memcpy(m_results.data(), input.data(), N * sizeof(ResultType));
        } else if constexpr (std::is_same_v<Input, Span<typename Input::value_type>>) {
            if (input.size() < N) {
                return false;
            }

            for (reg i = 0; i < N; ++i) {
                m_results[i] = static_cast<ResultType>(input[i]);
            }
        }

#if __cplusplus > 201703L
        else if constexpr (std::is_same_v<Input, std::span<ResultType>>) {
            if (input.size() < N) {
                return false;
            }

            std::memcpy(m_results.data(), input.data(), N * sizeof(ResultType));

        } else if constexpr (std::is_same_v<Input, std::span<typename Input::value_type>>) {
            if (input.size() < N) {
                return false;
            }
            for (reg i = 0; i < N; ++i) {
                m_results[i] = static_cast<ResultType>(input[i]);
            }
        }
#endif /* __cplusplus > 201703L */

        else {
            return false;
        }

        if constexpr (UseFlags) {
            if (m_flags == 0) return true;
        }

        if constexpr (BreakExists) {
            if constexpr (BreakIndex > 0 && BreakIndex < sizeof...(Transforms) - 1) {
                applyTransformsBeforeBreak(std::make_index_sequence<BeforeBreakCount>{});
            } else if constexpr (BreakIndex == sizeof...(Transforms) - 1) {
                applyTransforms(std::make_index_sequence<sizeof...(Transforms)>{});
            }
        } else {
            applyTransforms(std::make_index_sequence<sizeof...(Transforms)>{});
        }

        return true;
    }

    inline constexpr std::array<ResultType, N>& results() {
        if constexpr (BreakExists && BreakIndex < sizeof...(Transforms) - 1) {
            if (!m_postBreakComputed) {
                applyTransformsAfterBreak(std::make_index_sequence<AfterBreakCount>{});
                m_postBreakComputed = true;
            }
        }
        return m_results;
    }

    inline constexpr std::array<ResultType, N>& get_array() {
        return m_results;
    }

private:
    static constexpr std::size_t findBreakIndex() {
        return findBreakIndexImpl(std::make_index_sequence<sizeof...(Transforms)>{});
    }

    template<std::size_t... Indices>
    static constexpr std::size_t findBreakIndexImpl(std::index_sequence<Indices...>) {
        constexpr std::array<bool, sizeof...(Transforms)> isBreak = {
            std::is_same_v<std::tuple_element_t<Indices, std::tuple<Transforms...>>, Break>...
        };

        // Знайти перший індекс, де isBreak[i] == true
        for (std::size_t i = 0; i < isBreak.size(); ++i) {
            if (isBreak[i]) {
                return i;
            }
        }

        // Якщо Break не знайдено
        return sizeof...(Transforms);
    }

private:
    template<std::size_t... Indices>
    inline constexpr void applyTransforms(std::index_sequence<Indices...>) {
        if constexpr (UseFlags) {
            (..., (shouldApply<Indices>() ? applyTransform<Indices>() : void()));
        } else {
            (..., applyTransform<Indices>());
        }
    }

    template<std::size_t... Indices>
    inline constexpr void applyTransformsBeforeBreak(std::index_sequence<Indices...>) {
        if constexpr (UseFlags) {
            (..., (shouldApply<Indices>() ? applyTransform<Indices>() : void()));
        } else {
            (..., applyTransform<Indices>());
        }
    }

    template<std::size_t... Indices>
    inline constexpr void applyTransformsAfterBreak(std::index_sequence<Indices...>) {
        if constexpr (UseFlags) {
            (..., (shouldApply<BreakIndex + 1 + Indices>() ? applyTransform<BreakIndex + 1 + Indices>() : void()));
        } else {
            (..., applyTransform<BreakIndex + 1 + Indices>());
        }
    }

    template<std::size_t Index>
    inline constexpr bool shouldApply() const {
        if constexpr (UseFlags) {
            return (m_flags & (1U << Index)) != 0;
        } else {
            return true;
        }
    }

    template<std::size_t Index>
    inline constexpr void applyTransform() {
        using TransformType = std::tuple_element_t<Index, std::tuple<Transforms...>>;

        if constexpr (std::is_same_v<TransformType, Break>) {
            return;
        } else {
            auto& transform = std::get<Index>(m_transforms);
            if constexpr (N == 1) {
                m_results[0] = static_cast<ResultType>(transform.apply(m_results[0]));
            } else {
                for (auto& value : m_results) {
                    value = static_cast<ResultType>(transform.apply(value));
                }
            }
        }
    }

    inline constexpr void resetPostBreakFlag() {
        if constexpr (BreakExists && BreakIndex > 0 && BreakIndex < sizeof...(Transforms) - 1) {
            m_postBreakComputed = false;
        }
    }

public:
    static constexpr std::size_t TransformSize = sizeof...(Transforms);
    static constexpr std::size_t DataSize = N;
    static constexpr std::size_t BreakIndex = findBreakIndex();
    static constexpr bool BreakExists = BreakIndex < sizeof...(Transforms);
    static constexpr std::size_t BeforeBreakCount = BreakExists ? BreakIndex : 0;
    static constexpr std::size_t AfterBreakCount = BreakExists ? sizeof...(Transforms) - BreakIndex - 1 : 0;

private:
    std::array<ResultType, N> m_results = {};
    std::tuple<Transforms...> m_transforms;
    u32 m_flags = 0xFFFFFFFF;
    bool m_postBreakComputed = false;
};

#endif /* ___MATH_TRANSFORM_TRANSFORM_H_ */

