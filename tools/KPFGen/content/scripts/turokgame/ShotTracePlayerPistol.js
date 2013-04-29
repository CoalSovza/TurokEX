//-----------------------------------------------------------------------------
//
// ShotTracePlayerPistol.js
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

ShotTracePlayerPistol = class.extends(ShotTrace);

class.properties(ShotTracePlayerPistol,
{
    //------------------------------------------------------------------------
    // VARS
    //------------------------------------------------------------------------
    
    left    : 0.0,
    up      : 0.0,
    spray   : 0.015,
    hitFx   : "fx/fx_011.kfx",
    hitSnd  : "sounds/shaders/bullet_impact_3.ksnd"
});
