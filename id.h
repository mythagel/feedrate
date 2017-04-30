#ifndef ID_H
#define ID_H
#include "binding.h"

namespace id {
using Vc = function<tag_CuttingSpeed, tag_CutterDiameterAtDepthOfCut, tag_SpindleSpeed>;
using n = function<tag_SpindleSpeed, tag_CuttingSpeed, tag_CutterDiameterAtDepthOfCut>;
using fz = function<tag_FeedPerTooth, tag_TableFeed, tag_SpindleSpeed, tag_EffectiveCutterTeeth>;
using Q = function<tag_MaterialRemovalRate, tag_DepthOfCut, tag_WorkingEngagement, tag_TableFeed>;
using Vf = function<tag_TableFeed, tag_FeedPerTooth, tag_SpindleSpeed, tag_EffectiveCutterTeeth>;
using Mc = function<tag_Torque, tag_NetPower, tag_SpindleSpeed>;
using Pc = function<tag_NetPower, tag_DepthOfCut, tag_WorkingEngagement, tag_TableFeed, tag_SpecificCuttingForce>;

}


#endif
