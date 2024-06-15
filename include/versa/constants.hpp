#pragma once

#include <cstdint>

#include <bit>
#include <stdexcept>

#include <versa/constants.inc>

namespace versa::info {
   enum class architecture : uint32_t {
      unknown = 0x0,                 /**< Unknown architecture */
      x86     = VERSA_X86_ARCH,      /**< x86 architecture */
      amd64   = VERSA_AMD64_ARCH,    /**< x64 architecture */
      arm32   = VERSA_ARM32_ARCH,    /**< ARM32 architecture */
      arm64   = VERSA_ARM64_ARCH,    /**< ARM64 architecture */
      sparc32 = VERSA_SPARC32_ARCH,  /**< SPARC32 architecture */
      sparc64 = VERSA_SPARC64_ARCH,  /**< SPARC64 architecture */
      mips32  = VERSA_MIPS32_ARCH,   /**< MIPS32 architecture */
      mips64  = VERSA_MIPS64_ARCH,   /**< MIPS64 architecture */
      ppc32   = VERSA_PPC32_ARCH,    /**< PPC32 architecture */
      ppc64   = VERSA_PPC64_ARCH,    /**< PPC64 architecture */
      riscv32 = VERSA_RISCV32_ARCH,  /**< RISC-V 32-bit architecture */
      riscv64 = VERSA_RISCV64_ARCH,  /**< RISC-V 64-bit architecture */
      s390    = VERSA_S390_ARCH,     /**< S390 architecture */
      s390x   = VERSA_S390X_ARCH,    /**< S390X architecture */
      wasm32  = VERSA_WASM32_ARCH,   /**< WebAssembly 32-bit architecture */
      wasm64  = VERSA_WASM64_ARCH    /**< WebAssembly 64-bit architecture */
   };

   enum class operating_system : uint16_t {
      unknown = 0x0,                 /**< Unknown operating system */
      windows = VERSA_WINDOWS_BUILD, /**< Windows operating system */
      macos   = VERSA_MACOS_BUILD,   /**< macOS operating system */
      ios     = VERSA_IOS_BUILD,     /**< iOS operating system */
      lnux    = VERSA_LINUX_BUILD,   /**< Linux operating system */
      unx     = VERSA_UNIX_BUILD,    /**< Unix operating system */
      bsd     = VERSA_BSD_BUILD,     /**< BSD operating system */
      android = VERSA_ANDROID_BUILD, /**< Android operating system */
      wasi    = VERSA_WASI_BUILD,    /**< WebAssembly System Interface operating system */
      posix   = VERSA_POSIX_BUILD    /**< POSIX operating system */
   };

   enum class compiler : uint16_t {
      unknown = 0x0,               /**< Unknown compiler */
      msvc    = VERSA_MSVC_BUILD,  /**< Microsoft Visual C++ compiler */
      gcc     = VERSA_GCC_BUILD,   /**< GNU Compiler Collection */
      clang   = VERSA_CLANG_BUILD, /**< Clang compiler */
      cl430   = VERSA_CL430_BUILD, /**< TI MSP430 C/C++ Compiler */
      intel   = VERSA_INTEL_BUILD  /**< Intel C++ Compiler */
   };

   enum class build_type : uint16_t {
      unknown                 = 0x0,                 /**< Unknown build */
      debug                   = VERSA_DEBUG_BUILD,   /**< Debug build */
      release                 = VERSA_RELEASE_BUILD, /**< Release build */
      profile                 = VERSA_PROFILE_BUILD, /**< Profiling build */
      trace                   = VERSA_TRACE_BUILD,   /**< Tracing build */
      minimum_size            = VERSA_MIN_SIZE_BUILD /**< Minimum size build */
   };

   enum class byte_order : uint8_t {
      unknown = 0x0,                 /**< Unknown endianness */
      little  = VERSA_LITTLE_ENDIAN, /**< Little-endian */
      big     = VERSA_BIG_ENDIAN,    /**< Big-endian */
      pdp     = VERSA_PDP_ENDIAN     /**< PDP-endian */
   };

   enum class language : uint8_t {
      unknown = 0x0,           /**< Unknown language */
      c       = VERSA_C_LANG,  /**< C language */
      cpp     = VERSA_CPP_LANG /**< C++ language */
   };

   struct build_info {
      architecture arch     = architecture::unknown;
      byte_order   order    = byte_order::unknown;
      operating_system os   = operating_system::unknown;
      compiler comp         = compiler::unknown;
      uint64_t comp_version = 0;
      language lang         = language::unknown;
      uint64_t lang_version = 0;
      build_type build      = build_type::unknown;
   };

   constexpr static inline build_info build_info_v = 
      {
         static_cast<architecture>(VERSA_ARCH), 
         static_cast<byte_order>(VERSA_BYTE_ORDER), 
         static_cast<operating_system>(VERSA_OS), 
         static_cast<compiler>(VERSA_COMPILER), 
         VERSA_COMPILER_VERSION, 
         static_cast<language>(VERSA_LANG), 
         VERSA_LANG_VERSION, 
         static_cast<build_type>(VERSA_BUILD)
      };

   struct version_t {
      version_t(const version_t&) = default;
      version_t(version_t&&) = default;
      constexpr inline version_t(std::uint16_t major=0, std::uint16_t minor=0, std::uint16_t patch=0, std::uint16_t tweak=0)
         : major(major), minor(minor), patch(patch), tweak(tweak) {}
      uint64_t major : 16 = 0;
      uint64_t minor : 16 = 0;
      uint64_t patch : 16 = 0;
      uint64_t tweak : 16 = 0; 
   };

   struct version_info : public version_t {
      enum class parts : uint8_t {
         major,   /**< Major version */
         minor,   /**< Minor version */
         patch,   /**< Patch version */
         tweak,   /**< Tweak version */
         suffix,  /**< Version suffix */
         git_hash /**< Git hash */
      }; 

      std::string_view suffix;
      std::string_view git_hash;
      
      version_info() = default;
      version_info(const version_info&) = default;
      version_info(version_info&&) = default;

      constexpr inline version_info(std::uint16_t major=0, std::uint16_t minor=0, std::uint16_t patch=0, std::uint16_t tweak=0, std::string_view suffix="", std::string_view git_hash="")
         : version_t(major, minor, patch, tweak), suffix(suffix), git_hash(git_hash) {}

      constexpr inline std::strong_ordering operator<=>(const version_info& other) const noexcept {
         return number() <=> other.number();
      }

      constexpr inline uint64_t number() const {
         return (major * 1000000) + (minor * 10000) + (patch * 100) + tweak;
      }

      template <parts part>
      constexpr inline auto get() const noexcept {
         if constexpr (part == parts::major) {
            return major;
         } else if constexpr (part == parts::minor) {
            return minor;
         } else if constexpr (part == parts::patch) {
            return patch;
         } else if constexpr (part == parts::tweak) {
            return tweak;
         } else if constexpr (part == parts::suffix) {
            return suffix;
         } else if constexpr (part == parts::git_hash) {
            return git_hash;
         } else {
            throw std::runtime_error("Invalid part requested");
         }
      }

      template <parts part>
      inline std::string to_string() const {
         if constexpr (part == parts::major) {
            return std::to_string(major);
         } else if constexpr (part == parts::minor) {
            return std::to_string(minor);
         } else if constexpr (part == parts::patch) {
            return std::to_string(patch);
         } else if constexpr (part == parts::tweak) {
            return std::to_string(tweak);
         } else if constexpr (part == parts::suffix) {
            return std::string(suffix.data(), suffix.size());
         } else if constexpr (part == parts::git_hash) {
            return std::string(git_hash.data(), git_hash.size());
         } else {
            return std::string();
         }
      }

      inline std::string to_string() const {
         const auto suffix = to_string<parts::suffix>().empty() ? "" : "-" + to_string<parts::suffix>();
         const auto git_hash = to_string<parts::git_hash>().empty() ? "" : " (" + to_string<parts::git_hash>() + ")";
         return to_string<parts::major>() + "." + 
                to_string<parts::minor>() + "." + 
                to_string<parts::patch>() + "." + 
                to_string<parts::tweak>() +
                suffix + git_hash; 
      }
   };

} // namespace versa::info