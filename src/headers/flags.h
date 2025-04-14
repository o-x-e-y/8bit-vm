#ifndef __OXEY_CCE_FLAGS_H
#define __OXEY_CCE_FLAGS_H

#include <stdint.h>

#define CF_BIT 0x01
#define AF_BIT 0x02
#define ZF_BIT 0x04
#define SF_BIT 0x08
#define TF_BIT 0x10
#define OF_BIT 0x20
#define IF_BIT 0x40

typedef uint8_t Flags;

/// Sets carry flag and returns new flags
Flags set_cf(Flags f);
/// Sets auxillary carry flag and returns new flags
Flags set_af(Flags f);
/// Sets zero flag and returns new flags
Flags set_zf(Flags f);
/// Sets sign flag and returns new flags
Flags set_sf(Flags f);
/// Sets trap flag and returns new flags
Flags set_tf(Flags f);
/// Sets overflow flag and returns new flags
Flags set_of(Flags f);
/// Sets interrupt flag and returns new flags
Flags set_if(Flags f);

/// Gets carry flag, all other bits set to zero
Flags get_cf(Flags f);
/// Gets auxillary carry flag, all other bits set to zero
Flags get_af(Flags f);
/// Gets zero flag, all other bits set to zero
Flags get_zf(Flags f);
/// Gets sign flag, all other bits set to zero
Flags get_sf(Flags f);
/// Gets trap flag, all other bits set to zero
Flags get_tf(Flags f);
/// Gets overflow flag, all other bits set to zero
Flags get_of(Flags f);
/// Gets interrupt flag, all other bits set to zero
Flags get_if(Flags f);

/// Unsets carry flag and returns new flags
Flags unset_cf(Flags f);
/// Unsets auxillary carry flag and returns new flags
Flags unset_af(Flags f);
/// Unsets zero flag and returns new flags
Flags unset_zf(Flags f);
/// Unsets sign flag and returns new flags
Flags unset_sf(Flags f);
/// Unsets trap flag and returns new flags
Flags unset_tf(Flags f);
/// Unsets overflow flag and returns new flags
Flags unset_of(Flags f);
/// Unsets interrupt flag and returns new flags
Flags unset_if(Flags f);

/// Toggles carry flag and returns new flags
Flags toggle_cf(Flags f);
/// Toggles auxillary carry flag and returns new flags
Flags toggle_af(Flags f);
/// Toggles zero flag and returns new flags
Flags toggle_zf(Flags f);
/// Toggles sign flag and returns new flags
Flags toggle_sf(Flags f);
/// Toggles trap flag and returns new flags
Flags toggle_tf(Flags f);
/// Toggles overflow flag and returns new flags
Flags toggle_of(Flags f);
/// Toggles interrupt flag and returns new flags
Flags toggle_if(Flags f);

#endif
