#if __cplusplus < 201103L
#    define ETL_STD_VER_CXX03
#elif __cplusplus <= 201103L
#    define ETL_STD_VER 11
#elif __cplusplus <= 201402L
#    define ETL_STD_VER 14
#elif __cplusplus <= 201703L
#    define ETL_STD_VER 17
#endif

#ifdef ETL_STD_VER_CXX03
#    define NOEXCEPT throw()
#    define CONSTEXPR
#else
#    define NOEXCEPT noexcept
#    define CONSTEXPR constexpr
#endif