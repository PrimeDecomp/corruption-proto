/*
 * G2MEAB WorldFormat/CGamePortalAreaData.cpp translation-unit scaffold.
 * .text: 0x805AEF30..0x805B0F74 (71 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Leading overlapping-volumes5AEF30 uses volume stride0x2C and recursive BSP
 * test5AF1E8; factory5AEFE8 validates magicDEAFBEEF with CGamePortalAreaData.cpp257 and
 * allocates0x50 at259. Destructor5AF0E4 and streamctor5AF16C own volumes/portals/two ushort
 * arrays/AABoxNodeTree at+0/+10/+20/+30/+40. Preserve volume/BSP/portal ctor, center/distance
 * routines, AABoxNodeTree recursion5AF6E0 asserting CGamePortalAreaData.cpp75 and reserved
 * leaf-capacity message, wrapper5AF80C/ctor5AF838/node5AF890, eraser5AF908, complete factory
 * owner/token chain5AF980..5AFB00 and all vector construction/destruction/reserve/copy helpers
 * through final Get<vector<node>>5B0F50+24, directly called by tree constructor5AF838. Exact
 * semantic counterpart/full native inventory is Echoes MetroidPrime/CPortalAreaData; Prime has no
 * corresponding standalone file/header/TU, and related area/octree/PVS interfaces were compared.
 * Prototype assertion proves basename; WorldFormat folder/Kyoto library are recommendations from
 * contiguous resource-data placement, not source-map proof. Next5B0F74 asserts
 * CCollisionPrimitiveData.cpp250, separately assigned.
 */
