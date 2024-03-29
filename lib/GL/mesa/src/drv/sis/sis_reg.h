/**************************************************************************

Copyright 2000 Silicon Integrated Systems Corp, Inc., HsinChu, Taiwan.
All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/
/* $XFree86$ */

/*
 * Authors:
 *    Sung-Ching Lin <sclin@sis.com.tw>
 *
 */

#ifndef _sis_reg_h_
#define _sis_reg_h_

/*
 * Define All the Register Address of 6327
 */
#define REG_3D_TSFSa                    0x8800
#define REG_3D_TSZa                     0x8804
#define REG_3D_TSXa                     0x8808
#define REG_3D_TSYa                     0x880C
#define REG_3D_TSARGBa                  0x8810
#define REG_3D_TSWGa                    0x8814
#define REG_3D_TSUAa                    0x8818
#define REG_3D_TSVAa                    0x881C
#define REG_3D_TSUBa                    0x8820
#define REG_3D_TSVBa                    0x8824
#define REG_3D_TSUCa                    0x8828
#define REG_3D_TSVCa                    0x882C

#define REG_3D_TSFSb                    0x8830
#define REG_3D_TSZb                     0x8834
#define REG_3D_TSXb                     0x8838
#define REG_3D_TSYb                     0x883C
#define REG_3D_TSARGBb                  0x8840
#define REG_3D_TSWGb                    0x8844
#define REG_3D_TSUAb                    0x8848
#define REG_3D_TSVAb                    0x884C
#define REG_3D_TSUBb                    0x8850
#define REG_3D_TSVBb                    0x8854
#define REG_3D_TSUCb                    0x8858
#define REG_3D_TSVCb                    0x885C

#define REG_3D_TSFSc                    0x8860
#define REG_3D_TSZc                     0x8864
#define REG_3D_TSXc                     0x8868
#define REG_3D_TSYc                     0x886C
#define REG_3D_TSARGBc                  0x8870
#define REG_3D_TSWGc                    0x8874
#define REG_3D_TSUAc                    0x8878
#define REG_3D_TSVAc                    0x887C
#define REG_3D_TSUBc                    0x8880
#define REG_3D_TSVBc                    0x8884
#define REG_3D_TSUCc                    0x8888
#define REG_3D_TSVCc                    0x888C

/*
 * REG_3D_AGPCmdSetting (89e4h-89f7)
 */
#define REG_3D_AGPCmBase                0x89E4
#define REG_3D_AGPRmDwNum               0x89E8
#define REG_3D_AGPTtDwNum               0x89EC
#define REG_3D_AGPCmFire                0x89F0

#define REG_3D_ParsingSet               0x89F4
#define REG_3D_PrimitiveSet             0x89F8
#define REG_3D_ShadeMode                0x89F8
#define REG_3D_EngineFire               0x89FC
#define REG_3D_EngineStatus             0x89FC
#define REG_3D_TEnable                  0x8A00
#define REG_3D_TEnable2                 0x8A04

#define REG_3D_ZSet                     0x8A08
#define REG_3D_ZBias                    0x8A0C
#define REG_3D_ZStWriteMask             0x8A10

#define REG_3D_ZAddress                 0x8A14
#define REG_3D_AlphaSet                 0x8A18
#define REG_3D_AlphaAddress             0x8A1C
#define REG_3D_DstSet                   0x8A20
#define REG_3D_DstAlphaWriteMask        0x8A24

#define REG_3D_DstAddress               0x8A28

#define REG_3D_LinePattern              0x8A2C

#define REG_3D_FogSet                   0x8A30

#define REG_3D_FogFarDistance           0x8A34
#define REG_3D_FogInverseDistance       0x8A38
#define REG_3D_FogFactorDensity         0x8A3C

#define REG_3D_StencilSet               0x8A44
#define REG_3D_StencilSet2              0x8A48
#define REG_3D_StencilAddress           0x8A4C

#define REG_3D_DstBlendMode             0x8A50
#define REG_3D_SrcBlendMode             0x8A50
#define REG_3D_ClipTopBottom            0x8A54
#define REG_3D_ClipLeftRight            0x8A58

#define REG_3D_Brightness               0x8A5C

#define REG_3D_BumpMapSet               0x8A68
#define REG_3D_BumpMapAddress           0x8A6C
#define REG_3D_BumpMapPitch             0x8A70
#define REG_3D_BumpMapMatrix0           0x8A74
#define REG_3D_BumpMapMatrix1           0x8A78

/*
 * Define the Texture Register Address of 6326
 */
#define REG_3D_TextureSet                     0x8A7C
#define REG_3D_TextureWidthHeight             0x8A7C
#define REG_3D_TextureMip                     0x8A80

#define REG_3D_TextureTransparencyColorHigh    0x8A84
#define REG_3D_TextureTransparencyColorLow     0x8A88
#define REG_3D_TextureBorderColor              0x8A8C
#define REG_3D_TextureAddress0                 0x8A90
#define REG_3D_TextureAddress1                 0x8A94
#define REG_3D_TextureAddress2                 0x8A98
#define REG_3D_TextureAddress3                 0x8A9C
#define REG_3D_TextureAddress4                 0x8AA0
#define REG_3D_TextureAddress5                 0x8AA4
#define REG_3D_TextureAddress6                 0x8AA8
#define REG_3D_TextureAddress7                 0x8AAC
#define REG_3D_TextureAddress8                 0x8AB0
#define REG_3D_TextureAddress9                 0x8AB4
#define REG_3D_TextureAddress10                0x8AB8
#define REG_3D_TextureAddress11                0x8ABC
#define REG_3D_TexturePitch0                   0x8AC0
#define REG_3D_TexturePitch1                   0x8AC0
#define REG_3D_TexturePitch2                   0x8AC4
#define REG_3D_TexturePitch3                   0x8AC4
#define REG_3D_TexturePitch4                   0x8AC8
#define REG_3D_TexturePitch5                   0x8AC8
#define REG_3D_TexturePitch6                   0x8ACC
#define REG_3D_TexturePitch7                   0x8ACC
#define REG_3D_TexturePitch8                   0x8AD0
#define REG_3D_TexturePitch9                   0x8AD0
#define REG_3D_TexturePitch10                  0x8AD4

#define REG_3D_Texture1Set                     0x8ADC
#define REG_3D_Texture1WidthHeight             0x8ADC
#define REG_3D_Texture1Mip                     0x8AE0

#define REG_3D_Texture1TransparencyColorHigh   0x8AE4
#define REG_3D_Texture1TransparencyColorLow    0x8AE8
#define REG_3D_Texture1BorderColor             0x8AEC
#define REG_3D_Texture1Address0                0x8AF0
#define REG_3D_Texture1Address1                0x8AF4
#define REG_3D_Texture1Address2                0x8AF8
#define REG_3D_Texture1Address3                0x8AFC
#define REG_3D_Texture1Address4                0x8B00
#define REG_3D_Texture1Address5                0x8B04
#define REG_3D_Texture1Address6                0x8B08
#define REG_3D_Texture1Address7                0x8B0C
#define REG_3D_Texture1Address8                0x8B10
#define REG_3D_Texture1Address9                0x8B14
#define REG_3D_Texture1Address10               0x8B18
#define REG_3D_Texture1Address11               0x8B1C
#define REG_3D_Texture1Pitch0                  0x8B20
#define REG_3D_Texture1Pitch1                  0x8B20
#define REG_3D_Texture1Pitch2                  0x8B24
#define REG_3D_Texture1Pitch3                  0x8B24
#define REG_3D_Texture1Pitch4                  0x8B28
#define REG_3D_Texture1Pitch5                  0x8B28
#define REG_3D_Texture1Pitch6                  0x8B2C
#define REG_3D_Texture1Pitch7                  0x8B2C
#define REG_3D_Texture1Pitch8                  0x8B30
#define REG_3D_Texture1Pitch9                  0x8B30
#define REG_3D_Texture1Pitch10                 0x8B34

#define REG_3D_TextureBlendFactor              0x8B3C
#define REG_3D_TextureColorBlendSet0           0x8B40
#define REG_3D_TextureColorBlendSet1           0x8B44
#define REG_3D_TextureAlphaBlendSet0           0x8B48
#define REG_3D_TextureAlphaBlendSet1           0x8B4C
/*
 * Define the End of Primitive List of 6326
 */
#define REG_3D_EndPrimitiveList                0X8B50


/*
 * Define the Stipple Register Address of 6326
 */
#define REG_3D_Stipple0                        0X8B60

#define REG_3D_TexturePalette                  0x8C00


/*
 *  Define All the Register Address of 6327
 */
#define X_REG_3D_TSFSa                     0
#define X_REG_3D_TSZa                      4
#define X_REG_3D_TSXa                      8
#define X_REG_3D_TSYa                     12
#define X_REG_3D_TSARGBa                  16
#define X_REG_3D_TSWGa                    20
#define X_REG_3D_TSUAa                    24
#define X_REG_3D_TSVAa                    28
#define X_REG_3D_TSUBa                    32
#define X_REG_3D_TSVBa                    36
#define X_REG_3D_TSUCa                    40
#define X_REG_3D_TSVCa                    44

#define X_REG_3D_TSFSb                    48
#define X_REG_3D_TSZb                     52
#define X_REG_3D_TSXb                     56
#define X_REG_3D_TSYb                     60
#define X_REG_3D_TSARGBb                  64
#define X_REG_3D_TSWGb                    68
#define X_REG_3D_TSUAb                    72
#define X_REG_3D_TSVAb                    76
#define X_REG_3D_TSUBb                    80
#define X_REG_3D_TSVBb                    84
#define X_REG_3D_TSUCb                    88
#define X_REG_3D_TSVCb                    92

#define X_REG_3D_TSFSc                    96
#define X_REG_3D_TSZc                    100
#define X_REG_3D_TSXc                    104
#define X_REG_3D_TSYc                    108
#define X_REG_3D_TSARGBc                 112
#define X_REG_3D_TSWGc                   116
#define X_REG_3D_TSUAc                   120
#define X_REG_3D_TSVAc                   124
#define X_REG_3D_TSUBc                   128
#define X_REG_3D_TSVBc                   132
#define X_REG_3D_TSUCc                   136
#define X_REG_3D_TSVCc                   140

#define X_REG_3D_ParsingSet              176
#define X_REG_3D_PrimitiveSet            180
#define X_REG_3D_ShadeMode               180
#define X_REG_3D_EngineFire              184
#define X_REG_3D_EngineStatus            184
#define X_REG_3D_TEnable                 188
#define X_REG_3D_TEnable2                192

#define X_REG_3D_ZSet                    196
#define X_REG_3D_ZBias                   200
#define X_REG_3D_ZStWriteMask            204

#define X_REG_3D_ZAddress                208
#define X_REG_3D_AlphaSet                212
#define X_REG_3D_AlphaAddress            216
#define X_REG_3D_DstSet                  220
#define X_REG_3D_DstAlphaWriteMask       224

#define X_REG_3D_DstAddress              228

#define X_REG_3D_LinePattern             232

#define X_REG_3D_FogSet                  236
#define X_REG_3D_FogFarDistance          240
#define X_REG_3D_FogInverseDistance      244
#define X_REG_3D_FogFactorDensity        248

#define X_REG_3D_StencilSet              256
#define X_REG_3D_StencilSet2             260
#define X_REG_3D_StencilAddress          264

#define X_REG_3D_DstBlendMode            268
#define X_REG_3D_SrcBlendMode            268
#define X_REG_3D_ClipTopBottom           272
#define X_REG_3D_ClipLeftRight           276
#define X_REG_3D_Brightness              280

#define X_REG_3D_BumpMapSet              292
#define X_REG_3D_BumpMapAddress          296
#define X_REG_3D_BumpMapPitch            300
#define X_REG_3D_BumpMapMatrix0          304
#define X_REG_3D_BumpMapMatrix1          308


/*
 * Define the Texture Register Address of 6327
 */
#define X_REG_3D_Texture0Set                     312
#define X_REG_3D_Texture0WidthHeight             312
#define X_REG_3D_Texture0Set2                    316

#define X_REG_3D_Texture0TransparencyColorHigh   320
#define X_REG_3D_Texture0TransparencyColorLow    324
#define X_REG_3D_Texture0BorderColor             328
#define X_REG_3D_Texture0Address0                332
#define X_REG_3D_Texture0Address1                336
#define X_REG_3D_Texture0Address2                340
#define X_REG_3D_Texture0Address3                344
#define X_REG_3D_Texture0Address4                348
#define X_REG_3D_Texture0Address5                352
#define X_REG_3D_Texture0Address6                356
#define X_REG_3D_Texture0Address7                360
#define X_REG_3D_Texture0Address8                364
#define X_REG_3D_Texture0Address9                368
#define X_REG_3D_Texture0Address10               372
#define X_REG_3D_Texture0Address11               376
#define X_REG_3D_Texture0Pitch0                  380
#define X_REG_3D_Texture0Pitch1                  380
#define X_REG_3D_Texture0Pitch2                  384
#define X_REG_3D_Texture0Pitch3                  384
#define X_REG_3D_Texture0Pitch4                  388
#define X_REG_3D_Texture0Pitch5                  388
#define X_REG_3D_Texture0Pitch6                  392
#define X_REG_3D_Texture0Pitch7                  392
#define X_REG_3D_Texture0Pitch8                  396
#define X_REG_3D_Texture0Pitch9                  396
#define X_REG_3D_Texture0Pitch10                 400

#define X_REG_3D_Texture1Set                     408
#define X_REG_3D_Texture1WidthHeight             408
#define X_REG_3D_Texture1Set2                    412

#define X_REG_3D_Texture1TransparencyColorHigh   416
#define X_REG_3D_Texture1TransparencyColorLow    420
#define X_REG_3D_Texture1BorderColor             424
#define X_REG_3D_Texture1Address0                428
#define X_REG_3D_Texture1Address1                432
#define X_REG_3D_Texture1Address2                436
#define X_REG_3D_Texture1Address3                440
#define X_REG_3D_Texture1Address4                444
#define X_REG_3D_Texture1Address5                448
#define X_REG_3D_Texture1Address6                452
#define X_REG_3D_Texture1Address7                456
#define X_REG_3D_Texture1Address8                460
#define X_REG_3D_Texture1Address9                464
#define X_REG_3D_Texture1Address10               468
#define X_REG_3D_Texture1Address11               472
#define X_REG_3D_Texture1Pitch0                  476
#define X_REG_3D_Texture1Pitch1                  476
#define X_REG_3D_Texture1Pitch2                  480
#define X_REG_3D_Texture1Pitch3                  480
#define X_REG_3D_Texture1Pitch4                  484
#define X_REG_3D_Texture1Pitch5                  484
#define X_REG_3D_Texture1Pitch6                  488
#define X_REG_3D_Texture1Pitch7                  488
#define X_REG_3D_Texture1Pitch8                  492
#define X_REG_3D_Texture1Pitch9                  492
#define X_REG_3D_Texture1Pitch10                 496

#define X_REG_3D_TextureBlendFactor              504
#define X_REG_3D_TextureColorBlendSet0           508
#define X_REG_3D_TextureColorBlendSet1           512
#define X_REG_3D_TextureAlphaBlendSet0           516
#define X_REG_3D_TextureAlphaBlendSet1           520

/*
 * Define the End of Primitive List of 6326
 */
#define X_REG_3D_EndPrimitiveList                524


/*
 * Define the Stipple Register Address of 6326
 */
#define X_REG_3D_Stipple0                        540

#define X_REG_3D_TexturePalette                 1520


/*
 * REG_3D_ParsingSet -- Define Parsing Mask (89F4h-89F7h)
 */
#define MASK_VertexDWSize								0xF0000000
#define MASK_VertexDataFormat							0x0FFF0000
#define MASK_PsVertex_HAS_RHW                   0x08000000
#define MASK_PsVertex_HAS_NORMALXYZ             0x04000000
#define MASK_PsVertex_HAS_DIFFUSE               0x02000000
#define MASK_PsVertex_HAS_SPECULAR              0x01000000
#define MASK_PsUVSet                            0x00FF0000
#define MASK_PsVertex_HAS_1SetUV                0x00800000
#define MASK_PsVertex_HAS_2SetUV                0x00C00000
#define MASK_PsVertex_HAS_3SetUV                0x00E00000
#define MASK_PsVertex_HAS_UVSet1                0x00800000
#define MASK_PsVertex_HAS_UVSet2                0x00400000
#define MASK_PsVertex_HAS_UVSet3                0x00200000
#define MASK_PsCullDirection_CCW        0x00008000
#define MASK_PsShadingMode              0x00007000
#define MASK_PsTextureFrom              0x000003F0
#define MASK_PsTexture0FromA            0x00000000
#define MASK_PsTexture1FromA            0x00000000
#define MASK_PsTexture1FromB            0x00000040
#define MASK_PsBumpTextureFromA                 0x00000000
#define MASK_PsBumpTextureFromB                 0x00000010
#define MASK_PsBumpTextureFromC                 0x00000020
#define MASK_PsDataType                                 0x0000000F
#define MASK_PsPointList                                0x00000000
#define MASK_PsLineList                                 0x00000004
#define MASK_PsLineStrip                                0x00000005
#define MASK_PsTriangleList                             0x00000008
#define MASK_PsTriangleStrip                            0x00000009
#define MASK_PsTriangleFan                              0x0000000A
/*
 * REG_3D_PrimitiveSet -- Define Fire Primitive Mask (89F8h-89FBh)
 */
#define MASK_DrawPrimitiveCommand       0x00000007
#define MASK_SetFirePosition            0x00001F00
#define MASK_BumpTextureFrom            0x00030000
#define MASK_Texture1From               0x000C0000
#define MASK_Texture0From               0x00300000
#define MASK_ShadingMode                0x07000000
#define MASK_CullDirection              0x08000000
/*
 *           Define Command Queue Length Mask (89FCh-89FF)
 */
#define MASK_CmdQueueLen                0x0FFF0000

/*
 * REG_3D_TEnable -- Define Capility Enable Mask (8A00h-8A03h)
 */
#define MASK_DitherEnable               0x00000001
#define MASK_BlendEnable                0x00000002
#define MASK_FogTestEnable              0x00000004
#define MASK_FogEnable                  0x00000008
#define MASK_SpecularEnable             0x00000010
#define MASK_FogPerspectiveEnable      0x00000020
#define MASK_TextureCacheClear          0x00000040
#define MASK_TextureCacheEnable         0x00000080
#define MASK_BumpMapEnable              0x00000100
#define MASK_TexturePerspectiveEnable   0x00000200
#define MASK_TextureEnable              0x00000400
#define MASK_CullEnable                 0x00000800
#define MASK_TextureNumUsed             0x0000F000
#define MASK_AlphaBufferEnable          0x00010000
#define MASK_AlphaTestEnable            0x00020000
#define MASK_AlphaWriteEnable           0x00040000
#define MASK_ZTestEnable                0x00080000
#define MASK_ZWriteEnable               0x00100000
#define MASK_StencilBufferEnable        0x00200000
#define MASK_StencilTestEnable          0x00400000
#define MASK_StencilWriteEnable         0x00800000
#define MASK_Texture0TransparencyEnable 0x01000000
#define MASK_Texture1TransparencyEnable 0x02000000
#define MASK_TextureAWrapUCorrection    0x04000000
#define MASK_TextureAWrapVCorrection    0x08000000
#define MASK_TextureBWrapUCorrection    0x10000000
#define MASK_TextureBWrapVCorrection    0x20000000
#define MASK_TextureCWrapUCorrection    0x40000000
#define MASK_TextureCWrapVCorrection    0x80000000



/*
 * REG_3D_TEnable2 -- Define Capility Enable Mask2 (8A04h-8A07h)
 */
#define MASK_Texture0BlockTextureEnable 0x00000001
#define MASK_Texture1BlockTextureEnable 0x00000002
#define MASK_Texture0AnisotropicEnable  0x00000010
#define MASK_Texture1AnisotropicEnable  0x00000020
#define MASK_TextureMipmapBiasEnable    0x00000040
#define MASK_LinePatternEnable          0x00000100
#define MASK_StippleAlphaEnable         0x00000200
#define MASK_StippleEnable              0x00000400
#define MASK_AntiAliasEnable            0x00000800
#define MASK_ZMaskWriteEnable           0x00001000
#define MASK_StencilMaskWriteEnable     0x00002000
#define MASK_AlphaMaskWriteEnable       0x00004000
#define MASK_ColorMaskWriteEnable       0x00008000
#define MASK_ZCacheClear                0x00010000
#define MASK_ZCacheEnable               0x00020000
#define MASK_StencilCacheClear          0x00040000
#define MASK_StencilCacheEnable         0x00080000
#define MASK_AlphaCacheClear            0x00100000
#define MASK_AlphaCacheEnable           0x00200000
#define MASK_ColorCacheClear            0x00400000
#define MASK_ColorCacheEnable           0x00800000

/*
 * REG_3D_ZSet -- Define Z Buffer Setting Mask (8A08h-8A0Bh)
 */
#define MASK_ZBufferPitch               0x00000FFF
#define MASK_ZTestMode                  0x00070000
#define MASK_ZBufferInSystem            0x00080000
#define MASK_ZBufferFormat              0x01F00000

/*
 * REG_3D_ZBias -- Define Z Buffer Setting Mask (8A0Ch-8A0Fh)
 */
#define MASK_ZBias                      0xFFFFFFFF

/*
 * REG_3D_ZStWriteMask -- Define Z and Stencil Buffer Mask (8A10h-8A13h)
 */
#define MASK_ZWriteMask                 0x00FFFFFF

/*
 * REG_3D_ZAddress -- Define Z Buffer Base Address(8A14h-8A17h)
 */
#define MASK_ZAddress                   0xFFFFFFFF

/*
 * REG_3D_AlphaSet -- Define Alpha Buffer Setting Mask (8A18h-8A1Bh)
 */
#define MASK_AlphaBufferPitch           0x000003FF
#define MASK_AlphaRefValue              0x00FF0000
#define MASK_AlphaTestMode              0x07000000
#define MASK_AlphaBufferInSystem        0x08000000
#define MASK_AlphaBufferFormat          0x30000000

/*
 * REG_3D_AlphaAddress -- Define Alpha Buffer Base Address(8A1Ch-8A1Fh)
 */
#define MASK_AlphaAddress               0xFFFFFFFF

/*
 * REG_3D_DstSet -- Define Destination Buffer Setting Mask (8A20h-8A23h)
 */
#define MASK_DstBufferPitch             0x00000FFF
#define MASK_DstBufferFormat            0x000F0000
#define MASK_DstBufferBitDepth          0x00300000
#define MASK_DstBufferRgbOrder          0x00400000
#define MASK_DstBufferInSystem          0x00800000
#define MASK_Dst7BitFormat              0x007F0000
#define MASK_ROP2                       0x0F000000

/*
 * REG_3D_DstAlphaWriteMask -- Define Destination/Alpha  Buffer Write Mask (8A24h-8A27h)
 */
#define MASK_ColorWriteMask             0x00FFFFFF
#define MASK_AlphaWriteMask             0xFF000000

/*
 * REG_3D_DstAddress -- Define Destination Buffer Base Address(8A1Ch-8A1Fh)
 */
#define MASK_DstAddress                 0xFFFFFFFF

/*
 * REG_3D_LinePattern -- Define Line Pattern (8A2Ch-8A2Fh)
 */
#define MASK_LinePatternRepeatFactor    0x00007FFF
#define MASK_LinePatternLastPixelFlag   0x00008000
#define MASK_LinePattern                0xFFFF0000

/*
 * REG_3D_FogSet -- Define Fog Mask (8A30h-8A33h)
 */
#define MASK_FogColor                   0x00FFFFFF
#define MASK_FogMode                    0x07000000
#define MASK_FogZLookup		        0x08000000

/*
 * REG_3D_FogStartEnd -- Define Fog Start End Setting (8A34h-8A37h)
 */
#define MASK_FogFarDistance                             0x0007FFFF

/*
 * REG_3D_FogStartEnd -- Define Fog End Setting (8A38h-8A3Bh)
 */
#define MASK_FogInvFarDistance          0x0007FFFF

#define MASK_FogDensity                 0x0003FFFF
#define MASK_FogFactor                  0xFF000000

#define MASK_StencilMaskValue           0x000000FF
#define MASK_StencilReferenceValue      0x0000FF00
#define MASK_StencilTestMode            0x07000000
#define MASK_StencilBufferInSystem      0x08000000
#define MASK_StencilFormat              0x30000000

#define MASK_StencilMode_ALWAYS         0x07000000
#define MASK_StencilBufferPitch         0x00000FFF
#define MASK_StencilZPassOp             0x00007000
#define MASK_StencilZFailOp             0x00070000
#define MASK_StencilFailOp              0x00700000
#define MASK_StencilWriteMask           0xFF000000

#define MASK_StencilZPassOp_REPLACE     0x00002000
#define MASK_StencilZFailOp_REPLACE     0x00020000
#define MASK_StencilFailOp_REPLACE      0x00200000
#define MASK_StencilAddress             0xFFFFFFFF

#define MASK_SrcBlendMode                       0x0000000F
#define MASK_DstBlendMode                       0x000000F0
#define MASK_DSTBLEND_ZERO                      0x00000000
#define MASK_DSTBLEND_ONE                       0x00000010
#define MASK_DSTBLEND_SRC_COLOR                 0x00000020
#define MASK_DSTBLEND_INV_SRC_COLOR             0x00000030
#define MASK_DSTBLEND_SRC_ALPHA                 0x00000040
#define MASK_DSTBLEND_INV_SRC_ALPHA             0x00000050
#define MASK_DSTBLEND_DST_ALPHA                 0x00000060
#define MASK_DSTBLEND_INV_DST_ALPHA             0x00000070
#define MASK_DSTBLEND_DST_COLOR                 0x00000080
#define MASK_DSTBLEND_INV_DST_COLOR             0x00000090
#define MASK_DSTBLEND_SRC_ALPHA_SAT             0x000000A0

#define MASK_SRCBLEND_ZERO                      0x00000000
#define MASK_SRCBLEND_ONE                       0x00000001
#define MASK_SRCBLEND_SRC_COLOR                 0x00000002
#define MASK_SRCBLEND_INV_SRC_COLOR             0x00000003
#define MASK_SRCBLEND_SRC_ALPHA                 0x00000004
#define MASK_SRCBLEND_INV_SRC_ALPHA             0x00000005
#define MASK_SRCBLEND_DST_ALPHA                 0x00000006
#define MASK_SRCBLEND_INV_DST_ALPHA             0x00000007
#define MASK_SRCBLEND_DST_COLOR                 0x00000008
#define MASK_SRCBLEND_INV_DST_COLOR             0x00000009
#define MASK_SRCBLEND_SRC_ALPHA_SAT             0x0000000A
#define MASK_SRCBLEND_BOTH_SRC_ALPHA            0x0000000B
#define MASK_SRCBLEND_BOTH_INV_SRC_ALPHA        0x0000000C

#define MASK_BottomClip                 0x00001FFF
#define MASK_TopClip                    0x03FFE000

#define MASK_RightClip                  0x00001FFF
#define MASK_LeftClip                   0x03FFE000

#define MASK_BMMemoryInSystem           0x00000080
#define MASK_BMHeight                   0x00000F00
#define MASK_BMWidth                    0x0000F000
#define MASK_BMFilter                   0x00010000
#define MASK_BMMappingMode              0x007E0000
#define MASK_BMFormat                   0x07800000
#define MASK_BMTxBumpmap                0x08000000

#define MASK_BMAddress                  0xFFFFFFFC

#define MASK_BMOffset                   0xFF800000
#define MASK_BMScale                    0x007FE000
#define MASK_BMPitch                    0x00001FFF

#define MASK_BMMatrix00                 0x000007FF
#define MASK_BMMatrix01                 0x07FF0000
#define MASK_BMMatrix10                 0x000007FF
#define MASK_BMMatrix11                 0x07FF0000

#define MASK_TextureHeight             0x0000000F
#define MASK_TextureWidth              0x000000F0
#define MASK_TextureLevel              0x00000F00
#define MASK_TextureSignYUVFormat      0x00001000
#define MASK_TextureMappingMode        0x00FF0000
#define MASK_TextureWrapU              0x00010000
#define MASK_TextureWrapV              0x00020000
#define MASK_TextureMirrorU            0x00040000
#define MASK_TextureMirrorV            0x00080000
#define MASK_TextureClampU             0x00100000
#define MASK_TextureClampV             0x00200000
#define MASK_TextureBorderU            0x00400000
#define MASK_TextureBorderV            0x00800000
#define MASK_TextureFormat             0xFF000000
#define MASK_TextureBitDepth           0x70000000
#define MASK_TextureRgbOrder           0x80000000

#define MASK_TextureAnisotropyRatio    0x0000000F
#define MASK_TextureMipmapLodBias      0x00003FF0
#define MASK_TextureFilterMin          0x0001C000
#define MASK_TextureFilterMag          0x00020000
#define MASK_TextureFilter             0x0003C000
#define MASK_TextureLevelInSystem      0x3FFC0000
#define MASK_TextureLevel0InSystem     0x00040000
#define MASK_TextureBlockLength        0xF0000000

#define MASK_TextureTransparencyColorHighB  0x000000FF
#define MASK_TextureTransparencyColorHighG  0x0000FF00
#define MASK_TextureTransparencyColorHighR  0x00FF0000
#define MASK_TextureAlphaTransparencyMode   0x08000000

#define MASK_TextureTransparencyColorLowB   0x000000FF
#define MASK_TextureTransparencyColorLowG   0x0000FF00
#define MASK_TextureTransparencyColorLowR   0x00FF0000
#define MASK_TextureBlockHeight             0x07000000
#define MASK_TextureBlockWidth              0x70000000

#define MASK_TextureBorderColorB       0x000000FF
#define MASK_TextureBorderColorG       0x0000FF00
#define MASK_TextureBorderColorR       0x00FF0000
#define MASK_TextureBorderColorA       0xFF000000

#define MASK_TexturePitchOdd            0x000003FF
#define MASK_TexturePitchEven           0x03FF0000
#define SHIFT_TexturePitchEven          16

#define MASK_TextureRealInSystem        0x00000001
#define MASK_TextureDowngrade           0x00000002

#define OP_3D_POINT_DRAW                0x00000000
#define OP_3D_LINE_DRAW                 0x00000001
#define OP_3D_TRIANGLE_DRAW             0x00000002

#define OP_3D_DIRECTION_RIGHT           0x00000000
#define OP_3D_DIRECTION_LEFT            0x00000100
#define OP_3D_DIRECTION_HORIZONTAL      0x00000000
#define OP_3D_DIRECTION_VERTICAL        0x00000100

#define OP_3D_FIRE_TFIRE                0x00000000
#define OP_3D_FIRE_TSARGBa              0x00000100
#define OP_3D_FIRE_TSWa                0x00000200
#define OP_3D_FIRE_TSVAa                0x00000300
#define OP_3D_FIRE_TSVBa                0x00000400
#define OP_3D_FIRE_TSVCa                0x00000500

#define OP_3D_FIRE_TSARGBb              0x00000900
#define OP_3D_FIRE_TSWb                0x00000a00
#define OP_3D_FIRE_TSVAb                0x00000b00
#define OP_3D_FIRE_TSVBb                0x00000c00
#define OP_3D_FIRE_TSVCb                0x00000d00

#define OP_3D_FIRE_TSARGBc              0x00001100
#define OP_3D_FIRE_TSWc                0x00001200
#define OP_3D_FIRE_TSVAc                0x00001300
#define OP_3D_FIRE_TSVBc                0x00001400
#define OP_3D_FIRE_TSVCc                0x00001500

#define OP_3D_Texture0FromA             0x00000000
#define OP_3D_Texture0FromB             0x00100000
#define OP_3D_Texture0FromC             0x00200000
#define OP_3D_Texture1FromA             0x00000000
#define OP_3D_Texture1FromB             0x00040000
#define OP_3D_Texture1FromC             0x00080000
#define OP_3D_TextureBumpFromA          0x00000000
#define OP_3D_TextureBumpFromB          0x00010000
#define OP_3D_TextureBumpFromC          0x00020000

#define SHADE_FLAT_VertexA              0x01000000
#define SHADE_FLAT_VertexB              0x02000000
#define SHADE_FLAT_VertexC              0x03000000
#define SHADE_GOURAUD                   0x04000000

#define Z_BUFFER_FORMAT_Z16             0x00000000
#define Z_BUFFER_FORMAT_Z16_INT         0x00100000
#define Z_BUFFER_FORMAT_S1Z15           0x00400000
#define Z_BUFFER_FORMAT_S1Z15_INT       0x00500000
#define Z_BUFFER_FORMAT_Z32             0x00800000
#define Z_BUFFER_FORMAT_S1Z31           0x00C00000
#define Z_BUFFER_FORMAT_S2Z30           0x00D00000
#define Z_BUFFER_FORMAT_S4Z28           0x00E00000
#define Z_BUFFER_FORMAT_S8Z24           0x00F00000
#define Z_BUFFER_FORMAT_FZ30            0x01800000
#define Z_BUFFER_FORMAT_FS1Z30          0x01C00000
#define Z_BUFFER_FORMAT_FS2Z30          0x01D00000
#define MASK_Z_BUFFER_FORMAT                    0x01F00000

#define ALPHA_BUFFER_FORMAT_1           0x00000000
#define ALPHA_BUFFER_FORMAT_2           0x10000000
#define ALPHA_BUFFER_FORMAT_4           0x20000000
#define ALPHA_BUFFER_FORMAT_8           0x30000000

#define DST_FORMAT_RGB_555              0x00100000
#define DST_FORMAT_RGB_565              0x00110000
#define DST_FORMAT_ARGB_1555            0x00120000
#define DST_FORMAT_ARGB_4444            0x00130000
#define DST_FORMAT_ARGB_1888            0x00300000
#define DST_FORMAT_ARGB_2888            0x00310000
#define DST_FORMAT_ARGB_4888            0x00320000
#define DST_FORMAT_ARGB_8888            0x00330000
#define DST_FORMAT_ARGB_0888            0x00340000

#define DST_FORMAT_BGR_555              0x00500000
#define DST_FORMAT_BGR_565              0x00510000
#define DST_FORMAT_ABGR_1555            0x00520000
#define DST_FORMAT_ABGR_4444            0x00530000
#define DST_FORMAT_ABGR_1888            0x00700000
#define DST_FORMAT_ABGR_2888            0x00710000
#define DST_FORMAT_ABGR_4888            0x00720000
#define DST_FORMAT_ABGR_8888            0x00730000
#define DST_FORMAT_ABGR_0888            0x00740000

#define MASK_TEXTURE_FORMAT                             0xFF000000

#define TEXEL_INDEX1                    0x00000000
#define TEXEL_INDEX2                    0x01000000
#define TEXEL_INDEX4                    0x02000000
#define TEXEL_INDEX8                    0x03000000

#define TEXEL_INDEX1WithAlpha           0x04000000
#define TEXEL_INDEX2WithAlpha           0x05000000
#define TEXEL_INDEX4WithAlpha           0x06000000
#define TEXEL_INDEX8WithAlpha           0x07000000

#define TEXEL_I1                        0x10000000
#define TEXEL_I2                        0x11000000
#define TEXEL_I4                        0x12000000
#define TEXEL_I8                        0x13000000

#define TEXEL_DXT1                      0x19000000
#define TEXEL_DXT2                      0x1A000000
#define TEXEL_DXT3                      0x1B000000

#define TEXEL_YUV422                    0x20000000
#define TEXEL_YVU422                    0x21000000
#define TEXEL_UVY422                    0x22000000
#define TEXEL_VUY422                    0x23000000
#define TEXEL_YUV411                    0x24000000

#define TEXEL_L1                        0x30000000
#define TEXEL_L2                        0x31000000
#define TEXEL_L4                        0x32000000
#define TEXEL_L8                        0x33000000

#define TEXEL_AL11                      0x34000000
#define TEXEL_AL44                      0x35000000
#define TEXEL_AL26                      0x37000000
#define TEXEL_AL88                      0x38000000

#define TEXEL_A1                        0x40000000
#define TEXEL_A2                        0x41000000
#define TEXEL_A4                        0x42000000
#define TEXEL_A8                        0x43000000

#define TEXEL_RGB_332_8                 0x50000000
#define TEXEL_RGB_233_8                 0x51000000
#define TEXEL_RGB_232_8                 0x52000000
#define TEXEL_ARGB_1232_8               0x53000000
#define TEXEL_ARGB_2222_8               0x54000000

#define TEXEL_RGB_555_16                0x60000000
#define TEXEL_RGB_565_16                0x61000000
#define TEXEL_ARGB_1555_16              0x62000000
#define TEXEL_ARGB_4444_16              0x63000000

#define TEXEL_ARGB_1888_32              0x70000000
#define TEXEL_ARGB_2888_32              0x71000000
#define TEXEL_ARGB_4888_32              0x72000000
#define TEXEL_ARGB_8888_32              0x73000000
#define TEXEL_ARGB_0888_32              0x74000000

#define TEXEL_BGR_332_8                 0xD0000000
#define TEXEL_BGR_233_8                 0xD1000000
#define TEXEL_BGR_232_8                 0xD2000000
#define TEXEL_ABGR_1232_8               0xD3000000
#define TEXEL_ABGR_2222_8               0xD4000000

#define TEXEL_BGR_555_16                0xE0000000
#define TEXEL_BGR_565_16                0xE1000000
#define TEXEL_ABGR_1555_16              0xE2000000
#define TEXEL_ABGR_4444_16              0xE3000000

#define TEXEL_ABGR_1888_32              0xF0000000
#define TEXEL_ABGR_2888_32              0xF1000000
#define TEXEL_ABGR_4888_32              0xF2000000
#define TEXEL_ABGR_8888_32              0xF3000000
#define TEXEL_ABGR_0888_32              0xF4000000

#define TEXEL_VU88                      0x00000000
#define TEXEL_LVU655                    0x00800000
#define TEXEL_LVU888                    0x01000000
#define TEXEL_UV88                      0x02000000
#define TEXEL_LUV655                    0x02800000
#define TEXEL_LUV888                    0x03000000

#define FOGMODE_CHEAP                  0x04000000
#define FOGMODE_LINEAR                  0x05000000
#define FOGMODE_EXP                     0x06000000
#define FOGMODE_EXP2                    0x07000000

#define Z_16												0x00000000
#define S_1_Z_15										0x00400000
#define Z_32												0x00800000
#define S_1_Z_31										0x00c00000
#define S_2_Z_30										0x00d00000
#define S_4_Z_28										0x00e00000
#define S_8_Z_24										0x00f00000

#define S_1													0x00000000
#define S_2													0x10000000
#define S_4													0x20000000
#define S_8													0x30000000

#define TEXTURE_FILTER_NEAREST									0x00000000
#define TEXTURE_FILTER_LINEAR										0x00004000
#define TEXTURE_FILTER_NEAREST_MIP_NEAREST		0x00008000
#define TEXTURE_FILTER_NEAREST_MIP_LINEAR			0x00010000
#define TEXTURE_FILTER_LINEAR_MIP_NEAREST			0x0000c000
#define TEXTURE_FILTER_LINEAR_MIP_LINEAR				0x00014000

#define A_REPLACE_RGB_STAGE0						0xa1485000
#define A_REPLACE_A_STAGE0								0xc3230000
#define L_REPLACE__RGB_STAGE0						0xc1485000
#define L_REPLACE__A_STAGE0								0x63230000
#define LA_REPLACE__RGB_STAGE0					0xc1485000
#define LA_REPLACE__A_STAGE0						0xc3230000
#define I_REPLACE__RGB_STAGE0						0xc1485000
#define I_REPLACE__A_STAGE0								0xc3230000
#define RGB_REPLACE__RGB_STAGE0					0xc1485000
#define RGB_REPLACE__A_STAGE0						0x63230000
#define RGBA_REPLACE__RGB_STAGE0			0xc1485000
#define RGBA_REPLACE__A_STAGE0					0xc3230000

#define A_MODULATE_RGB_STAGE0					0xa1485000
#define A_MODULATE_A_STAGE0							0x63c30000
#define L_MODULATE__RGB_STAGE0					0xa1705000
#define L_MODULATE__A_STAGE0						0x63230000
#define LA_MODULATE__RGB_STAGE0				0xa1705000
#define LA_MODULATE__A_STAGE0					0x63c30000
#define I_MODULATE__RGB_STAGE0					0xa1705000
#define I_MODULATE__A_STAGE0						0x63c30000
#define RGB_MODULATE__RGB_STAGE0			0xa1705000
#define RGB_MODULATE__A_STAGE0					0x63230000
#define RGBA_MODULATE__RGB_STAGE0		0xa1705000
#define RGBA_MODULATE__A_STAGE0				0x63c30000

#define RGB_DECAL__RGB_STAGE0			0xc1485000
#define RGB_DECAL__A_STAGE0					0x63230000
#define RGBA_DECAL__RGB_STAGE0			0xc534c001
#define RGBA_DECAL__A_STAGE0				0x63230000

#define A_BLEND_RGB_STAGE0					0xa1485000
#define A_BLEND_A_STAGE0						0x63c30000
#define L_BLEND__RGB_STAGE0					0x4530c001
#define L_BLEND__A_STAGE0						0x63230000
#define LA_BLEND__RGB_STAGE0			0x4530c001
#define LA_BLEND__A_STAGE0					0x63c30000
#define I_BLEND__RGB_STAGE0					0x4530c001
#define I_BLEND__A_STAGE0						0x46c60001
#define RGB_BLEND__RGB_STAGE0			0x4530c001
#define RGB_BLEND__A_STAGE0					0x63230000
#define RGBA_BLEND__RGB_STAGE0		0x4530c001
#define RGBA_BLEND__A_STAGE0			0x63c30000

#define RGB_STAGE1										0xa1485000
#define A_STAGE1											0xa3230000

#define A_REPLACE_RGB_STAGE1				0xa1485000
#define A_REPLACE_A_STAGE1						0xe3230000
#define L_REPLACE__RGB_STAGE1			0xe1485000
#define L_REPLACE__A_STAGE1					0xa3230000
#define LA_REPLACE__RGB_STAGE1			0xe1485000
#define LA_REPLACE__A_STAGE1				0xe3230000
#define I_REPLACE__RGB_STAGE1				0xe1485000
#define I_REPLACE__A_STAGE1					0xe3230000
#define RGB_REPLACE__RGB_STAGE1			0xe1485000
#define RGB_REPLACE__A_STAGE1				0xa3230000
#define RGBA_REPLACE__RGB_STAGE1		0xe1485000
#define RGBA_REPLACE__A_STAGE1				0xe3230000

#define A_MODULATE_RGB_STAGE1		0xa1485000
#define A_MODULATE_A_STAGE1				0xa3e30000
#define L_MODULATE__RGB_STAGE1		0xa1785000
#define L_MODULATE__A_STAGE1				0xa3230000
#define LA_MODULATE__RGB_STAGE1			0xa1785000
#define LA_MODULATE__A_STAGE1				0xa3e30000
#define I_MODULATE__RGB_STAGE1				0xa1785000
#define I_MODULATE__A_STAGE1					0xa3e30000
#define RGB_MODULATE__RGB_STAGE1		0xa1785000
#define RGB_MODULATE__A_STAGE1				0xa3230000
#define RGBA_MODULATE__RGB_STAGE1	0xa1785000
#define RGBA_MODULATE__A_STAGE1			0xa3e30000

#define RGB_DECAL__RGB_STAGE1		0xe1485000
#define RGB_DECAL__A_STAGE1					0xa3230000
#define RGBA_DECAL__RGB_STAGE1			0xe5394001
#define RGBA_DECAL__A_STAGE1			0xa3230000

#define A_BLEND_RGB_STAGE1				0xa1485000
#define A_BLEND_A_STAGE1					0xa3e30000
#define L_BLEND__RGB_STAGE1				0x45394001
#define L_BLEND__A_STAGE1					0xa3230000
#define LA_BLEND__RGB_STAGE1		0x45394001
#define LA_BLEND__A_STAGE1				0xa3e30000
#define I_BLEND__RGB_STAGE1				0x45394001
#define I_BLEND__A_STAGE1						0x4aea0001
#define RGB_BLEND__RGB_STAGE1			0x45394001
#define RGB_BLEND__A_STAGE1					0xa3230000
#define RGBA_BLEND__RGB_STAGE1		0x45394001
#define RGBA_BLEND__A_STAGE1			0xa3e30000

#endif
