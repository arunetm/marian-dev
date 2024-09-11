#pragma once

#include <array>

#if defined(_WIN32) || defined(_WIN64)
  #include <intrin.h> // Windows specific header
#else
  #include <cpuid.h> // Gcc/Clang specific header
#endif

// Identifying x86_64 arch across compilers. 
#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
  #define _ARCH_X86_64 1
#endif

// This class is a singleton that encapusates CPU feature detection using cpuid.
// It provides static methods for checking the support for AVX512, VPOPCNTDQ, AVX2, and various SSE instructions. 
// This is a PoC implementation verfied on Intel platforms and hasnt gone through comprehensive cross platform testing yet. 
class CpuFeatures {
  private:
    std::array<int, 4> cpuidData_; //data from EAX, EBX, ECX, EDX registers
    static CpuFeatures* instance_;
    CpuFeatures(){
      #if defined(_WIN32) || defined(_WIN64) //If compiling on windows
        __cpuidex(cpuid_data.data(), 7, 0);
      #else //If compiling with GCC or Clang
        __cpuid_count(7, 0, cpuidData_[0], cpuidData_[1], cpuidData_[2], cpuidData_[3]);
      #endif
    }
  public:
    CpuFeatures(const CpuFeatures& ) = delete;
    CpuFeatures& operator=(const CpuFeatures& ) = delete;
    static inline const CpuFeatures* get_instance(){
      return (instance_)? instance_ : (instance_ = new CpuFeatures());
    }

    static inline const std::array<int, 4>& getCpuidData(){
      return get_instance()->cpuidData_;
    }
    // Predicates for runtime checking for specific cpu feature support.
    static inline bool isAVX512Supported(){ 
      return (getCpuidData()[1] & (1 << 16)) != 0;
    }
    static inline bool isVPOPCNTDQSupported(){ 
      return (getCpuidData()[1] & (1 << 30)) != 0;
    }
    static inline bool isAVX2Supported(){
      return (getCpuidData()[1] & (1 << 5)) != 0;
    }
    static inline bool isSSESupported(){
      return (getCpuidData()[3] & (1 << 25)) != 0;
    }
    static inline bool isSSES2upported(){
      return (getCpuidData()[3] & (1 << 26)) != 0;
    }
    static inline bool isSSE41Supported(){
      return (getCpuidData()[2] & (1 << 19)) != 0;
     }
};

CpuFeatures* CpuFeatures::instance_ = nullptr;
