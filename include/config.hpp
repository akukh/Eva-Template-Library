#if __cplusplus < 201103L
#    define ETL_STD_VER_CXX03
#    define ETL_HAS_NO_RVALUE_REFERENCES
#elif __cplusplus <= 201103L
#    define ETL_STD_VER 11
#elif __cplusplus <= 201402L
#    define ETL_STD_VER 14
#elif __cplusplus <= 201703L
#    define ETL_STD_VER 17
#endif

// clang-format off
#ifdef ETL_STD_VER_CXX03
#    define NOEXCEPT              throw()
#    define ENOEXCEPT(expression) (expression)
#    define CONSTEXPR             /* */
#    define DEFAULT               {}
#    define DELETE                /* */
#    define EXPLICIT              /* */
#    define NODISCARD             /* */
#    define MAYBE_UNUSED          /* */
#elif defined(ETL_HAS_NO_RVALUE_REFERENCES)
#    define MOVE(expression)      (expression)
#else
#    define NOEXCEPT              noexcept
#    define ENOEXCEPT(expression) noexcept(expression)
#    define CONSTEXPR             constexpr
#    define DEFAULT               = default
#    define DELETE                = delete
#    define EXPLICIT              explicit
#    define NODISCARD             [[nodiscard]]
#    define MAYBE_UNUSED          [[maybe_unused]]
#    define MOVE(expression)      std::move(expression)
#endif