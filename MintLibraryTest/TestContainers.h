#pragma once


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintCommon/Include/Logger.h>


namespace mint
{
    namespace TestContainers
    {
        enum class TellerAt
        {
            DefaultCtor,
            CtorWithInitialization,
            CopyCtor,
            MoveCtor,
            CopyAssignment,
            MoveAssignment,
        };

        static constexpr const char* const kTellerAtString[]
        {
            "Default ctor",
            "Ctor with initialization",
            "Copy ctor",
            "Move ctor",
            "Copy assignment",
            "Move assignment",
        };

        class Teller
        {
        public:
            Teller() : _className{}, _i{ 0 } { __noop; }
            Teller(const char* const className, const TellerAt tellerAt) : _className{}, _i{ 0 } { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s", _className, kTellerAtString[static_cast<uint32>(tellerAt)]); }
            Teller(const char* const className, const TellerAt tellerAt, const int32 i) : _className{}, _i{ i } { ::strcpy_s(_className, className); MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(tellerAt)], i); }
            Teller(const Teller& rhs) { ::strcpy_s(_className, rhs._className); _i = rhs._i; MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(TellerAt::CopyCtor)], _i); }
            Teller(Teller&& rhs) noexcept { ::strcpy_s(_className, rhs._className); _i = rhs._i; rhs._i = 0; MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(TellerAt::MoveCtor)], _i); }
            ~Teller() { MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, "Dtor", _i); }

        public:
            Teller& operator=(const Teller& rhs)
            { 
                if (this != &rhs)
                {
                    ::strcpy_s(_className, rhs._className); 
                    _i = rhs._i; 
                    MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(TellerAt::CopyAssignment)], _i);
                }
                return *this;
            }
            Teller& operator=(Teller&& rhs) noexcept 
            {
                if (this != &rhs)
                {
                    ::strcpy_s(_className, rhs._className);
                    _i = rhs._i; rhs._i = 0; 
                    MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(TellerAt::MoveAssignment)], _i);
                }
                return *this;
            }

        public:
            void tell(const TellerAt tellerAt) { MINT_LOG_UNTAGGED("±èÀå¿ø", "%s - %s, %d", _className, kTellerAtString[static_cast<uint32>(tellerAt)], _i); }

        private:
            char    _className[260];
            int32   _i;
        };


        class Notable
        {
            static constexpr const char* const kClassName = "Notable";

        public:
            Notable() : _i{ 0 }, _teller{ kClassName, TellerAt::DefaultCtor, _i } { __noop; }
            Notable(const int32 i) : _i{ i }, _teller{ kClassName, TellerAt::CtorWithInitialization, _i } { __noop; }
            Notable(const Notable& rhs) = default;
            Notable(Notable&& rhs) noexcept = default;
            ~Notable() = default;

        public:
            Notable& operator=(const Notable& rhs) = default;
            Notable& operator=(Notable&& rhs) noexcept = default;
            
        private:
            int32   _i;
            Teller  _teller;
        };


        class Uncopiable
        {
            static constexpr const char* const kClassName = "Uncopiable";

        public:
            Uncopiable() : _teller{ kClassName, TellerAt::DefaultCtor } { __noop; }
            Uncopiable(const int32 i) : _teller{ kClassName, TellerAt::CtorWithInitialization }, _notable{ i } { __noop; }
            Uncopiable(const Uncopiable& rhs) = delete;
            Uncopiable(Uncopiable&& rhs) noexcept = default;
            ~Uncopiable() = default;

        public:
            Uncopiable& operator=(const Uncopiable& rhs) = delete;
            Uncopiable& operator=(Uncopiable&& rhs) noexcept = default;

        private:
            Teller  _teller;
            Notable _notable;
        };


        class Unmovable
        {
            static constexpr const char* const kClassName = "Unmovable";

        public:
            Unmovable() : _teller{ kClassName, TellerAt::DefaultCtor } { __noop; }
            Unmovable(const int32 i) : _teller{ kClassName, TellerAt::CtorWithInitialization }, _notable{ i } { __noop; }
            Unmovable(const Unmovable& rhs) = default;
            Unmovable(Unmovable&& rhs) noexcept = delete;
            ~Unmovable() = default;

        public:
            Unmovable& operator=(const Unmovable& rhs) = default;
            Unmovable& operator=(Unmovable&& rhs) noexcept = delete;

        private:
            Teller  _teller;
            Notable _notable;
        };
    }
}
