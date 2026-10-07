// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80266BB0..0x80271294 (143 native functions).
// Source identity: asserted runtime basename; leading data source/header emission inferred.
// Complete emitted native/helper inventory retained; no speculative declarations.
// Leading SwarmBasicsData source/header emission is inferred; original file ownership unresolved.
// 0x80266BB0 +0x674: SwarmBasicsData tagged-property stream loader; literal unknown-property diagnostic;0x188 payload
// 0x80267224 +0x70: SwarmBasicsData/SLdrBasicSwarmProperties destructor; source/header emission inferred
// 0x80267294 +0x148: SwarmBasicsData default constructor; damage/health/vulnerability and0x188 common payload defaults
// 0x802673DC +0x84: swarm message-parameter construction helper, used by death notifications and Think
// 0x80267460 +0x4: empty swarm initialization hook, constructor6F7DC calls it
// 0x80267464 +0x8: raw mutable HealthInfo accessor,object+478; vtable806B9E28 and derived classes
// 0x8026746C +0x8: raw const HealthInfo accessor,object+478; vtable806B9E24 and derived classes
// 0x80267474 +0x60: raw seeker-target-locked query; boid index4C8 maps5DC to5CC; vtable806B9ECC
// 0x802674D4 +0x478: CSwarmBasics seeker-target maintenance; direct3221 mSeekerTargetsBoidIndices[i]!=-1 assertion
// 0x8026794C +0x78: raw freeze-boids radius test,0xB0 boid stride,freeze time594->boid58; vtable806B9EC8
// 0x802679C4 +0x10C: flush queued swarm death script notifications, calls local673DC
// 0x80267AD0 +0xFC: emit/queue boid death script notifications, calls local673DC
// 0x80267BCC +0x6C: conditional boid kill virtual dispatch at+C4
// 0x80267C38 +0x4: raw empty swarm virtual at806B9EF0
// 0x80267C3C +0x4: raw empty swarm virtual at806B9EEC
// 0x80267C40 +0x4: raw empty swarm virtual at806B9EE4
// 0x80267C44 +0x8: raw true-returning swarm virtual at806B9EE0
// 0x80267C4C +0x48: create area collision-cache bounds for a swarm partition
// 0x80267C94 +0x4: empty swarm model/update callback hook
// 0x80267C98 +0x108: create CAUD audio emitter for boid position using pool token
// 0x80267DA0 +0x70: test boid active/sound-type flags
// 0x80267E10 +0xA8: install looped-sound handle and boid index into8-byte sound pair
// 0x80267EB8 +0x1A0: update closest partition looped sounds; qsort callback70378,0x40 candidate limit
// 0x80268058 +0x110: maintain/release boid sound handles for selected partition
// 0x80268168 +0x150: find closest occupied5x5x5 partition by bounds center distance
// 0x802682B8 +0x3C: submit swarm particle generator to renderer
// 0x802682F4 +0x38: render/update swarm particle generator callback
// 0x8026832C +0xA8: emit boid death particle at boid translation
// 0x802683D4 +0xA0: remove looped sound matching boid index
// 0x80268474 +0x104: KillBoid path; damage application,particle,sound cleanup,script notification
// 0x80268578 +0x1C: raw orbit-position getter copiesobject+1AC;vtable806B9E40
// 0x80268594 +0xF0: raw aim-position prediction/interpolation using lock-on index and boid velocity;vtable806B9E44
// 0x80268684 +0xB0: lock-on transition interpolation state update
// 0x80268734 +0x3D4: choose seeker-target boid indices,local8-byte score vector and quicksort7070C
// 0x80268B08 +0x54: emitted8-byte score-vector destructor; generic reference fingerprints ambiguous
// 0x80268B5C +0x1D4: find best lock-on boid in aim cone
// 0x80268D30 +0x148: maintain current boid lock-on target
// 0x80268E78 +0x14C: validate boid lock-on by cone/line-of-sight collision ray
// 0x80268FC4 +0x6C: apply boid contact damage and reset damage cooldown
// 0x80269030 +0x658: swarm Touch; projectile/player bounds,damage-vulnerability,boid health and radius effects
// 0x80269688 +0x114: apply radius damage to active boids
// 0x8026979C +0x224: choose random matching active patrol waypoint from entity links
// 0x802699C0 +0x27C: boid path following and waypoint transition steering
// 0x80269C3C +0x164: boid alignment steering from neighbor average direction
// 0x80269DA0 +0x12C: single-neighbor cohesion steering variant
// 0x80269ECC +0x11C: cohesion steering; unique Prime CWallCrawlerSwarm normalized fingerprint corroborated by body
// 0x80269FE8 +0xE4: average-neighbor cohesion steering wrapper
// 0x8026A0CC +0x11C: separation steering; unique Prime CWallCrawlerSwarm normalized fingerprint corroborated by body
// 0x8026A1E8 +0xC8: neighbor separation steering wrapper
// 0x8026A2B0 +0xF8: find nearby boids within partitioned spatial lists
// 0x8026A3A8 +0x680: RenderUnsorted; literal Swarm::RenderUnsorted,partition lighting and virtual boid render,debug displays
// 0x8026AA28 +0x20: swarm render forwarding thunk to6A3A8; not vector destruction
// 0x8026AA48 +0x5C: conditional swarm render submission,particle render and unsorted render6A3A8
// 0x8026AAA4 +0x110: render one boid with ambient/freeze color,renderer transform and display-list state
// 0x8026ABB4 +0x68: RenderBoid selects per-model or frozen skinned state then6AAA4
// 0x8026AC1C +0x80: PreRenderBoid draw-mask test and skinned-state preparation
// 0x8026AC9C +0x6C: prepare skinned model state from animated model and skin rules
// 0x8026AD08 +0x284: compute partition ambient/dynamic-light contribution with CActorLights and color attenuation
// 0x8026AF8C +0x104: prepare partition actor lighting using area/world and bounds
// 0x8026B090 +0x134: pre-render active boids; frustum sphere tests,per-boid callback and global visibility flag
// 0x8026B1C4 +0x1C4: calculate bounds of indexed5x5x5 swarm partition
// 0x8026B388 +0x110: find partition-linked boid list for position,fall back to outlier list
// 0x8026B498 +0x3B4: rebuild active-boid index vector and partitioned linked lists,0xB0 stride
// 0x8026B84C +0x19C: advance boid movement from animation deltas or decrement freeze timer
// 0x8026B9E8 +0x178: advance swarm animated model samples and store0x1C animation deltas
// 0x8026BB60 +0x94: advance all boid movement using per-model animation delta
// 0x8026BBF4 +0x820: CSwarmBasics Think/update; partition collision caches,virtualboid updates,damage/sound/death/seeker closure
// 0x8026C414 +0x7C: emitted area collision-cache copy; retained in swarm Think TU
// 0x8026C490 +0x44: emitted reserved-vector cache copy wrapper,not transferred to WorldFormat
// 0x8026C4D4 +0x68: emitted0x910 collision-cache record copy loop
// 0x8026C53C +0x20: emitted collision-cache element placement wrapper
// 0x8026C55C +0x28: emitted conditional collision-cache copy wrapper
// 0x8026C584 +0x5C: emitted0x910 collision-cache record copy payload
// 0x8026C5E0 +0x44: emitted triangle-cache reserved-vector copy wrapper
// 0x8026C624 +0x68: emitted0x24 cache entry copy loop
// 0x8026C68C +0x20: emitted cache entry placement wrapper
// 0x8026C6AC +0x28: emitted conditional cache entry copy wrapper
// 0x8026C6D4 +0x4C: emitted0x24 cache entry copy payload
// 0x8026C720 +0x210: collect same-area door repulsor centers/radii into vector
// 0x8026C930 +0xF8: emitted0x10 repulsor-vector push,vector.h482 assert
// 0x8026CA28 +0x214: boid steering/collision method with8-way local jump table806B9EF8; decompiler switch incomplete
// 0x8026CC3C +0x87C: UpdateBoid; surface search/projection,clamped rotation,ground/freefall movement and callbacks
// 0x8026D4B8 +0x24C: nearest collision surface search in cached triangles
// 0x8026D704 +0x328: expanding-bounds surface search for boid spawn
// 0x8026DA2C +0x1A0: projected point-in-triangle surface test
// 0x8026DBCC +0x4C: ProjectVectorToPlane; Prime lineage fingerprint,body verified
// 0x8026DC18 +0x78: ProjectPointToPlane; Echoes SurfaceAlignmentHelper fingerprint,emitted in swarm,not separateTU
// 0x8026DC90 +0x3C: raw GetTouchBounds optional AABox copiesobject+170,setsengaged+18;vtable806B9E38
// 0x8026DCCC +0x94: swarm bounding box from extent and actor transform
// 0x8026DD60 +0xDC: update actor bounds from swarm virtual bounding box
// 0x8026DE3C +0x430: CreateBoid from patrol waypoint,surface projection/orientation and0xB0 boid initialization
// 0x8026E26C +0x284: allocate/update skinned swarm model states; direct635 allocation0x630
// 0x8026E4F0 +0x9C: emitted owned skinned-state replacement/destruction helper
// 0x8026E58C +0x118: emitted0x630 skinned-state vector push,vector.h482 assert
// 0x8026E6A4 +0x60: emitted skinned-state vector clear wrapper
// 0x8026E704 +0x38: emitted skinned-state range destruction wrapper
// 0x8026E73C +0x94: emitted0x630 skinned-state destructor loop
// 0x8026E7D0 +0x94: release all swarm looped-sound handles
// 0x8026E864 +0x7DC: CSwarmBasics script dispatch; direct512seeker-target allocation0x2E0,registration/deletion/area load
// 0x8026F040 +0xDC: emitted0xB0 boid-vector push,vector.h482 capacity assert
// 0x8026F11C +0x20: emitted boid placement-copy wrapper calls6F13C
// 0x8026F13C +0x28: emitted conditional boid copy wrapper calls6F164
// 0x8026F164 +0xE0: 0xB0 CBoid copy constructor payload
// 0x8026F244 +0x1B8: CSwarmBasics destructor;vtable806B9DF8,all vectors/particles/models/audio/shared actor base8029F840 cleanup
// 0x8026F3FC +0x84: emitted0xB0 boid-vector destructor; trivial element loop then free
// 0x8026F480 +0x84: emitted skinned-state vector destructor,0x630 elements
// 0x8026F504 +0xA0: emitted model-data vector destructor,0xAC elements
// 0x8026F5A4 +0x84: emitted trivial0x1C animation-delta vector destructor
// 0x8026F628 +0xAC: emitted owned skinned-state destructor
// 0x8026F6D4 +0x84: emitted trivial0x10 repulsor vector destructor
// 0x8026F758 +0x84: emitted trivial8-byte sound-pair vector destructor
// 0x8026F7DC +0x858: CSwarmBasics constructor;shared actor base8029F944,vtable806B9DF8,0xB0 boids,asserts395/397/406 models/display/particles
// 0x80270034 +0x110: emitted0x1C animation-delta vector push
// 0x80270144 +0xDC: emitted0xAC model-data vector push
// 0x80270220 +0x20: emitted model construction wrapper
// 0x80270240 +0x28: emitted conditional model construction wrapper
// 0x80270268 +0x110: create/null model-data helper; directCSwarmBasics.cpp268/271 allocation assertions
// 0x80270378 +0x34: raw qsort boid sound-distance comparator; callback parameter80267FB8,not omitted
// 0x802703AC +0x1C8: CBoid constructor;transform,velocity,surface,lifetime/index/state fields0xB0
// 0x80270574 +0x198: clamped alignment transform between normals,including opposite-normal case
// 0x8027070C +0x1C4: emitted quicksort8-byte seeker-target score records
// 0x802708D0 +0xAC: emitted8-byte score-vector reserve
// 0x8027097C +0xB8: emitted0xB0 boid-vector reserve
// 0x80270A34 +0x68: emitted0xB0 boid-vector copy loop
// 0x80270A9C +0x20: emitted boid placement-copy wrapper
// 0x80270ABC +0x28: emitted conditional boid copy wrapper calls6F164
// 0x80270AE4 +0xB8: emitted8-byte sound-pair vector reserve
// 0x80270B9C +0x3C: emitted8-byte sound-pair copy,wordhandle+ushortboidindex
// 0x80270BD8 +0xB8: emitted0x10 repulsor vector reserve
// 0x80270C90 +0x4C: emitted0x10 repulsor copy loop
// 0x80270CDC +0xAC: emitted0x630 skinned-state vector reserve
// 0x80270D88 +0x20: emitted skinned-state destruction wrapper
// 0x80270DA8 +0x90: emitted0x630 skinned-state destruction loop
// 0x80270E38 +0xAC: emitted0x630 skinned-state copy with ownership transfer flags
// 0x80270EE4 +0xB4: emitted0xAC model-data vector reserve
// 0x80270F98 +0x68: emitted0xAC model-data copy loop
// 0x80271000 +0x20: emitted model-data placement-copy wrapper
// 0x80271020 +0x28: emitted conditional model-data copy wrapper
// 0x80271048 +0xB8: emitted0x1C animation-delta vector reserve
// 0x80271100 +0x64: emitted0x1C animation-delta copy loop
// 0x80271164 +0x84: emitted8-byte seeker-target score median/swap helper
// 0x802711E8 +0x7C: emitted8-byte seeker-target score insertion-sort tail
// 0x80271264 +0x30: raw registered static initializer; .ctors8065B908,separate seven SDA constants
