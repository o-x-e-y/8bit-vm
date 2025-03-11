#include "headers/flags.h"

// Get flags

Flags get_cf(Flags f) { return f & CF_BIT; }

Flags get_af(Flags f) { return f & AF_BIT; }

Flags get_zf(Flags f) { return f & ZF_BIT; }

Flags get_sf(Flags f) { return f & SF_BIT; }

Flags get_tf(Flags f) { return f & TF_BIT; }

Flags get_of(Flags f) { return f & OF_BIT; }

Flags get_if(Flags f) { return f & OF_BIT; }

// Set flags

Flags set_cf(Flags f) { return f | CF_BIT; }

Flags set_af(Flags f) { return f | AF_BIT; }

Flags set_zf(Flags f) { return f | ZF_BIT; }

Flags set_sf(Flags f) { return f | SF_BIT; }

Flags set_tf(Flags f) { return f | TF_BIT; }

Flags set_of(Flags f) { return f | OF_BIT; }

Flags set_if(Flags f) { return f | OF_BIT; }

// Unset flags

Flags unset_cf(Flags f) { return f & (~CF_BIT); }

Flags unset_af(Flags f) { return f & (~AF_BIT); }

Flags unset_zf(Flags f) { return f & (~ZF_BIT); }

Flags unset_sf(Flags f) { return f & (~SF_BIT); }

Flags unset_tf(Flags f) { return f & (~TF_BIT); }

Flags unset_of(Flags f) { return f & (~OF_BIT); }

Flags unset_if(Flags f) { return f & (~OF_BIT); }

// Toggle flags

Flags toggle_cf(Flags f) { return f ^ CF_BIT; }

Flags toggle_af(Flags f) { return f ^ AF_BIT; }

Flags toggle_zf(Flags f) { return f ^ ZF_BIT; }

Flags toggle_sf(Flags f) { return f ^ SF_BIT; }

Flags toggle_tf(Flags f) { return f ^ TF_BIT; }

Flags toggle_of(Flags f) { return f ^ OF_BIT; }

Flags toggle_if(Flags f) { return f ^ OF_BIT; }
