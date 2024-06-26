#pragma once

#include <source_location>
#include <type_traits>
#include <utility>
#include "traits.hpp"
#include "string.hpp"

namespace versa::frozen {
   template <typename T>
   struct meta {
   };

   constexpr std::string_view function_name(std::string_view fn = std::source_location::current().function_name()) { return fn; }
   constexpr std::string_view file_name(std::string_view fn = std::source_location::current().file_name()) { return fn; }
   constexpr std::size_t line_number(std::size_t ln = std::source_location::current().line()) { return ln; }
   constexpr std::size_t column_number(std::size_t cn = std::source_location::current().column()) { return cn; }

   namespace detail {
      template <auto V>
      struct frozen_wrapper {
         static constexpr auto value = V;
      };

      template <typename T>
      concept wrapper_type = requires {
         { std::is_base_of_v<frozen_wrapper<T::value>, T> };
      };

      template <typename T>
      concept enum_type = requires {
         { std::is_enum_v<T> };
      };

      constexpr auto wrapper_s = string{"frozen_wrapper<"};
      constexpr auto nameof_s  = string{"nameof<"};
      constexpr auto t_eq_s    = string{"T = "};
      constexpr auto e_eq_s    = string{"E = "};
      constexpr auto enum_s    = string{"enum "};
      constexpr auto void_s    = string{">(void)"};
      constexpr auto angle_s   = string{">"};
      constexpr auto sq_s      = string{"]"};
      constexpr auto nil_s     = string{""};

      template <typename T>
      constexpr auto prefix() { 
         if constexpr (wrapper_type<T>)
            return wrapper_s; 
         else
            return nil_s;
      }

      template <typename T>
      constexpr auto enum_prefix() {
         if constexpr (enum_type<T>)
            return enum_s;
         else
            return nil_s;
      }

      template <typename T>
      constexpr auto start_str() {
         constexpr bool msvc = versa::info::build_info_v.comp == versa::info::compiler::msvc;

         if constexpr (msvc)
            return nameof_s + prefix<T>() + enum_prefix<T>();
         else
            return t_eq_s + prefix<T>() + enum_prefix<T>();
      }

      template <typename T>
      constexpr auto wrapper_suffix() {
         if constexpr (wrapper_type<T>)
            return angle_s;
         else
            return nil_s;
      }

      template <typename T>
      constexpr auto suffix() {
         constexpr bool msvc = versa::info::build_info_v.comp == versa::info::compiler::msvc;

         if constexpr (msvc)
            return void_s;
         else
            return sq_s;
      }

      template <typename T>
      constexpr auto end_str() {
         return wrapper_suffix<T>() + suffix<T>();
      }

      template <typename T>
      constexpr auto nameof() {
         constexpr auto full_name = std::string_view{VERSA_PRETTY_FUNCTION};
         constexpr auto start     = full_name.find(start_str<T>().to_string_view());
         constexpr auto offset    = start_str<T>().size();
         constexpr auto end       = full_name.find(end_str<T>().to_string_view(), start + offset);
         return full_name.substr(start + offset, end - start - offset);
      }

      template <typename T>
      constexpr std::string_view nameof_only() {
         constexpr auto sv = nameof<T>();
         constexpr auto start = []() constexpr {
            for (size_t i = sv.size(); i > 0; --i) {
               if (sv[i - 1] == ':') return i;
            }
            return size_t(0);
         }();
         return sv.substr(start);
      }

      template <enum_type E, E v>
      constexpr auto vnameof() {
         constexpr auto full_name = std::string_view{VERSA_PRETTY_FUNCTION};
         constexpr auto start     = full_name.find(nameof<E>());
         constexpr auto offset    = nameof<E>().size() + 1;
         constexpr auto end       = full_name.find(end_str<E>().to_string_view(), start + offset);
         return full_name.substr(start + offset, end - start - offset);
      }

      template <enum_type E, E v>
      constexpr std::string_view vnameof_only() {
         constexpr auto sv = vnameof<E, v>();
         constexpr auto start = []() constexpr {
            for (size_t i = sv.size(); i > 0; --i) {
               if (sv[i - 1] == ':') return i;
            }
            return size_t(0);
         }();
         return sv.substr(start);
      }

      constexpr bool is_valid_nm(std::string_view sv) {
         for (auto c : sv) {
            if (c == ')' || c == '(' || c == '{' || c == '}' || c == '<' || c == '>')
               return false;
         }
         return true;
      }

      constexpr auto valid_vnameof(std::string_view sv) { 
         return is_valid_nm(sv) ? sv : "";
      }
   } // namespace detail

   template <typename T, bool Full = true>
   constexpr std::string_view nameof() {
      if constexpr (Full) {
         return detail::nameof<T>();
      } else {
         return detail::nameof_only<T>();
      }
   }

   template <auto X, bool Full = true>
   constexpr std::string_view nameof() {
      return nameof<detail::frozen_wrapper<X>, Full>();
   }

   template <detail::enum_type E, E v, bool Full = true>
   constexpr std::string_view vnameof() {
      if constexpr (Full) {
         return detail::vnameof<E, v>();
      } else {
         return detail::vnameof_only<E, v>();
      }
   }

   template <typename T, bool Full = true>
   constexpr std::string_view type_name_v = nameof<T, Full>();

   template <auto V, bool Full = true>
   constexpr std::string_view enum_name_v = detail::valid_vnameof(
      vnameof<std::decay_t<decltype(V)>, V, Full>());

   constexpr int32_t enum_lb_v    = VERSA_ENUM_LOWER_BOUND;
   constexpr int32_t enum_ub_v    = VERSA_ENUM_UPPER_BOUND;
   constexpr auto    enum_range_v = VERSA_ENUM_MAX_ELEMS;

   using enum_idx_mapping_t = std::pair<std::string_view, int64_t>;
   using enum_name_mapping_t = std::pair<int64_t, std::string_view>;

   namespace detail::enums {
      template <typename E, auto V>
      constexpr decltype(auto) name() noexcept {
         return versa::frozen::enum_name_v<static_cast<E>(V), false>;
      }

      template <typename P, std::size_t N>
      constexpr auto filter(std::array<P, N> ms) noexcept {
         std::size_t i = 0;
         for (const auto& e : ms) {
            if (!e.first.empty()) {
               ms[i++] = e;
            }
         }
         return std::make_pair(ms, i);
      }

      template <typename E, std::size_t... Is>
      constexpr auto mappings(std::index_sequence<Is...>) noexcept {
         return filter(std::array{ std::make_pair(name<E, enum_lb_v + Is>(), enum_lb_v + Is)... });
      }

      template <typename E>
      constexpr decltype(auto) mappings() noexcept {
         return mappings<E>(std::make_index_sequence<VERSA_ENUM_MAX_ELEMS>{});
      }
   } // namespace detail::enums
} // namespace versa::frozen