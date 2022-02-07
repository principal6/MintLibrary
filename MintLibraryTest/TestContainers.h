#pragma once


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintCommon/Include/Logger.h>


namespace mint
{
    namespace TestContainers
    {
        template <typename T>
        class Teller
        {
            static constexpr uint32 kClassNameBufferSize = 260;

        public:
            Teller(const char* const className) : _className{}, _value{} { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("%s - %s", _className, "Ctor [default]"); }
            Teller(const char* const className, const T& value) : _className{}, _value{ value } { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [with initialization]", value); }
            Teller(const Teller& rhs) { ::strcpy_s(_className, rhs._className); _value = rhs._value; MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [copy]", _value); }
            Teller(Teller&& rhs) noexcept { ::strcpy_s(_className, rhs._className); _value = std::move(rhs._value); MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Ctor [move]", _value); }
            ~Teller() { MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Dtor", _value); ::memset(_className, 0, kClassNameBufferSize); }

        public:
            Teller& operator=(const Teller& rhs)
            { 
                if (this != &rhs)
                {
                    ::strcpy_s(_className, rhs._className); 
                    _value = rhs._value; 
                    MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Assign [Copy]", _value);
                }
                return *this;
            }
            Teller& operator=(Teller&& rhs) noexcept 
            {
                if (this != &rhs)
                {
                    ::strcpy_s(_className, rhs._className);
                    _value = std::move(rhs._value);
                    MINT_LOG_UNTAGGED("%s - %s, %d", _className, "Assign [Move]", _value);
                }
                return *this;
            }

        private:
            char    _className[kClassNameBufferSize];
            T       _value;
        };


        template <typename T>
        class Notable
        {
            static constexpr const char* const kClassName = "Notable";

        public:
            Notable() : _value{}, _teller{ kClassName, _value } { __noop; }
            Notable(const T value) : _value{ value }, _teller{ kClassName, _value } { __noop; }
            Notable(const Notable& rhs) = default;
            Notable(Notable&& rhs) noexcept = default;
            ~Notable() = default;

        public:
            Notable& operator=(const Notable& rhs) = default;
            Notable& operator=(Notable&& rhs) noexcept = default;
            
        private:
            T           _value;
            Teller<T>   _teller;
        };


        template <typename T>
        class Uncopiable
        {
            static constexpr const char* const kClassName = "Uncopiable";

        public:
            Uncopiable() : _teller{ kClassName } { __noop; }
            Uncopiable(const Notable<T> notable) : _notable{ notable }, _teller{ kClassName, notable } { __noop; }
            Uncopiable(const Uncopiable& rhs) = delete;
            Uncopiable(Uncopiable&& rhs) noexcept = default;
            ~Uncopiable() = default;

        public:
            Uncopiable& operator=(const Uncopiable& rhs) = delete;
            Uncopiable& operator=(Uncopiable&& rhs) noexcept = default;

        private:
            Teller<T>   _teller;
            Notable<T>  _notable;
        };


        template <typename T>
        class Unmovable
        {
            static constexpr const char* const kClassName = "Unmovable";

        public:
            Unmovable() : _teller{ kClassName } { __noop; }
            Unmovable(const Notable<T> notable) : _notable{ notable }, _teller{ kClassName, notable } { __noop; }
            Unmovable(const Unmovable& rhs) = default;
            Unmovable(Unmovable&& rhs) noexcept = delete;
            ~Unmovable() = default;

        public:
            Unmovable& operator=(const Unmovable& rhs) = default;
            Unmovable& operator=(Unmovable&& rhs) noexcept = delete;

        private:
            Teller<T>   _teller;
            Notable<T>  _notable;
        };


        const bool testQueue() noexcept;
        const bool testVector() noexcept;
        const bool testStaticArray() noexcept;
        const bool testStackHolder() noexcept;
        const bool testBitVector() noexcept;
        const bool testHashMap() noexcept;
        const bool testStringTypes() noexcept;
        const bool testStringUtil() noexcept;
        const bool testTree() noexcept;

        const bool testAll() noexcept;
    }
}
