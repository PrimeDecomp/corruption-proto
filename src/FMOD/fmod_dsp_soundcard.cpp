// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x80606150..0x80606428 (4 retained native functions).
// directly named by target allocation/free body.
// Evidence: Setup06150 allocates native-output conversion buffer+124 except outputformat5, naming fmod_dsp_soundcard.cpp806EDDD8 line2D, then assigns graph order072E4. Release061FC frees same field line4F then calls Filter release569AC. Process06264 calls Filter569F0 then PCM converter626F40 when needed.
// Preserve all retained helpers, callback thunks, raw-only natives and inline expansions in target order.
