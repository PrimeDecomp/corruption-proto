// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805FDA60..0x80605958 (13 retained native functions).
// directly named by target allocation/free body.
// Evidence: CtorFDA60 calls DSPi ctor070B8, installs Filter806EC948 thenResampler806EDA40,
// initializes fixed-point cursor+128/+12C,step+130/+134,rate+13C,history+148/+14C and ring
// cursors+15C/+160. ReleaseFDBD0/setupFDC68 name fmod_dsp_resampler.cpp806EDAC8 lines49/4F/86.
// ProcessingFDDA0 dispatches retained interpolation kernels via direct
// callsFE164->60242C,FE190->60044C,FE1BC->5FE5C8,FE1E8->603D20. Preserve all retained helpers,
// callback thunks, raw-only natives and inline expansions in target order.
