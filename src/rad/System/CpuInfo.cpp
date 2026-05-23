#include <rad/System/CpuInfo.h>

#include <rad/Common/String.h>

#include <string>

namespace rad
{

std::string_view GetCpuBrandString() noexcept
{
    static const std::string brand = []() {
#if defined(CPU_FEATURES_ARCH_X86)
        const cpu_features::X86Info& info = GetX86Info();
        std::string name = StrTrim(info.brand_string);
        if (name.empty())
        {
            name = StrTrim(info.vendor);
        }
        return name;
#elif defined(CPU_FEATURES_ARCH_PPC)
        const cpu_features::PPCPlatformStrings& platform = GetPPCPlatformStrings();
        std::string name = StrTrim(platform.machine);
        if (name.empty())
        {
            name = StrTrim(platform.platform);
        }
        return name;
#else
        return std::string("cpu");
#endif
    }();
    return brand;
}

#if defined(CPU_FEATURES_ARCH_X86)
const cpu_features::X86Info& GetX86Info()
{
    static const cpu_features::X86Info info = cpu_features::GetX86Info();
    return info;
}
const cpu_features::CacheInfo& GetX86CacheInfo()
{
    static const cpu_features::CacheInfo info = cpu_features::GetX86CacheInfo();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_ARM)
const cpu_features::ArmInfo& GetArmInfo()
{
    static const cpu_features::ArmInfo info = cpu_features::GetArmInfo();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_AARCH64)
const cpu_features::Aarch64Info& GetAarch64Info()
{
    static const cpu_features::Aarch64Info info = cpu_features::GetAarch64Info();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_MIPS)
const cpu_features::MipsInfo& GetMipsInfo()
{
    static const cpu_features::MipsInfo info = cpu_features::GetMipsInfo();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_PPC)
const cpu_features::PPCInfo& GetPPCInfo()
{
    static const cpu_features::PPCInfo info = cpu_features::GetPPCInfo();
    return info;
}
const cpu_features::PPCPlatformStrings& GetPPCPlatformStrings()
{
    static const cpu_features::PPCPlatformStrings info = cpu_features::GetPPCPlatformStrings();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_S390X)
const cpu_features::S390XInfo& GetS390XInfo()
{
    static const cpu_features::S390XInfo info = cpu_features::GetS390XInfo();
    return info;
}
const cpu_features::S390XPlatformStrings& GetS390XPlatformStrings()
{
    static const cpu_features::S390XPlatformStrings info = cpu_features::GetS390XPlatformStrings();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_RISCV)
const cpu_features::RiscvInfo& GetRiscvInfo()
{
    static const cpu_features::RiscvInfo info = cpu_features::GetRiscvInfo();
    return info;
}
#elif defined(CPU_FEATURES_ARCH_LOONGARCH)
const cpu_features::LoongArchInfo& GetLoongArchInfo()
{
    static const cpu_features::LoongArchInfo info = cpu_features::GetLoongArchInfo();
    return info;
}
#endif

} // namespace rad
