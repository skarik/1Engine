;const

vertconst 0 ViewTransformMatrix

VertConst 4 Vector ( 1,1,1,1 )
VertConst 5 Vector ( 0,0,0,0 )

vertconst 12 Light0Properties
VertConst 13 Light0Pos
VertConst 14 Light0Color



;vert

; Transform vertex positions to the view projections
mov r0, v7
m4x4 v0, r0, c0

mov r3, c4
mov r4, c5

; First light
mov r2, c12
sub r1, r0, c13
; do normal calcs
dp3 r5, v8, r1
; do attenuation
dp3 r1, r1, r1
rsq r1, r1
rcp r1, r1
; multiply distance by 1/range
mul r1, r1, r2.r
pow r1, r1, r2.g
sub r1, r3, r1
mul r1, r1, r5
; limit between 0 and 1
sge r6, r1, r4
mul r1, r1, r6
; Light intensity is now stored in r1



;frag

; Sample from the texture with UV set t0, save to output r0
texld r0 t0