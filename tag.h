#ifndef TAG_H
#define TAG_H

// Input / Output value types
enum {
    tag_CutterDiameterAtDepthOfCut = 'Dcap',    // mm        - Cutter diameter at actual depth of cut
    tag_FeedPerTooth = '  fz',                  // mm        - feed per tooth
    tag_CutterTeeth = '  Zn',                   //           - total cutter teeth
    tag_EffectiveCutterTeeth = '  Zc',          //           - effective cutter teeth
    tag_TableFeed = '  Vf',                     // mm/min    - table feed
    tag_FeedPerRevolution = '  fn',             // mm        - feed per revolution
    tag_DepthOfCut = '  ap',                    // mm        - depth of cut
    tag_CuttingSpeed = '  Vc',                  // m/min     - Cutting speed
    tag_ChipRakeAngle = '  Y0',                 //           - chip rake angle
    tag_WorkingEngagement = '  ae',             // mm        - working engagement
    tag_SpindleSpeed = '   n',                  // rpm       - spindle speed
    tag_NetPower = '  Pc',                      // kW        - net power
    tag_Torque = '  Mc',                        // Nm        - Torque
    tag_MaterialRemovalRate = '   Q',           // cm3/min   - Material removal rate
    tag_AverageChipThickness = '  hm',          // mm        - Average chip thickness
    tag_MaxChipThickness = ' hex',              // mm        - Max chip thickness
    tag_EnteringAngle = '  Kr',                 // deg       - Entering angle
    tag_MachinedDiameter = '  Dm',              // mm        - Machined diameter (component diameter)
    tag_UnmachinedDiameter = '  Dw',            // mm        - Unmachined diameter
    tag_TableFeedAtMachinedDiameter = ' Vfm',   // mm/min    - Table feed of tool at Dm (machined diameter)
    tag_SpecificCuttingForce = '  kc',          // N/mm2     - Material specific cutting force
    tag_CutterOverhang = '   T',                // mm        - Unsupported cutter length
    tag_CutterMaterialElasticity = '  ZE',      // N/mm2     - Cutter modulus of elasticity (Carbide / HSS)
    tag_CutterMomentOfInertia = '   I',         // mm4       - Cutter area moment of inertia

    tag_Deflection = '   F',                    // mm        - Deflection
    tag_TangentialForce = '  Ft',               // N         - Tangiential cutting force
    tag_ChipCrossSectionalArea = '   A',        //           - Uncut chip cross-sectional area
    tag_MaterialTensileStrength = ' sig',       // MPa       - Material ultimate tensile strength
};

#endif

