/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef HW_MONITOR_HII_H_
#define HW_MONITOR_HII_H_

/*
#define PLATFORM_INFO_FORMSET_GUID \
  { \
    0x8DF0F6FB, 0x65A5, 0x434B, { 0xB2, 0xA6, 0xCE, 0xDF, 0xD2, 0x0A, 0x96, 0x8A } \
  }
*/
// {1F99F615-2A3D-4FB1-AA4B-83FB47ECB2EF} 
#define HW_MONITOR_FORMSET_GUID \
  { \
    0x1F99F615, 0x2A3D, 0x4FB1, {0xAA, 0x4B, 0x83, 0xFB, 0x47, 0xEC, 0xB2, 0xEF} \
  }

#define LABEL_UPDATE             0x2223
#define LABEL_END                0x2224

#define HW_MONITOR_FORM_ID       0x1
#define HEX_BASE                 16  //Base value for hexadecimal conversion

// Signed integer constant multiplier value of sensors
#define CONSTANT_MULTIPLIER_IPMI_P5V               53
#define CONSTANT_MULTIPLIER_IPMI_P12V              70
#define CONSTANT_MULTIPLIER_P1V5_VDDH              100
#define CONSTANT_MULTIPLIER_P0V75_PCP              100
#define CONSTANT_MULTIPLIER_P0V9_VDDC_RCA          100
#define CONSTANT_MULTIPLIER_P0V75_VDDC_SOC         100
#define CONSTANT_MULTIPLIER_P1V2_VDDQ_AB           100
#define CONSTANT_MULTIPLIER_P1V2_VDDQ_CD           100
#define CONSTANT_MULTIPLIER_P1V8_PCP               100
#define CONSTANT_MULTIPLIER_CPU_TEMP               100
#define CONSTANT_MULTIPLIER_IPMI_P3V3              337

// Signed additive offset value of sensors
#define SIGNED_OFFSET_CPU_TEMP                     0
#define SIGNED_OFFSET_IPMI_P12V                    20
#define SIGNED_OFFSET_IPMI_P3V3                    32
#define SIGNED_OFFSET_IPMI_P5V                     -37
#define SIGNED_OFFSET_P1V5_VDDH                    50
#define SIGNED_OFFSET_P0V75_PCP                    50
#define SIGNED_OFFSET_P0V9_VDDC_RCA                50
#define SIGNED_OFFSET_P0V75_VDDC_SOC               50
#define SIGNED_OFFSET_P1V2_VDDQ_AB                 50
#define SIGNED_OFFSET_P1V2_VDDQ_CD                 50
#define SIGNED_OFFSET_P1V8_PCP                     50

// Signed Result Exponent value of sensors
#define RESULT_EXPONENT_CPU_TEMP                   100
#define RESULT_EXPONENT_IPMI_P12V                  1000
#define RESULT_EXPONENT_IPMI_P5V                   1000
#define RESULT_EXPONENT_IPMI_P3V3                  10000
#define RESULT_EXPONENT_P1V5_VDDH                  10000
#define RESULT_EXPONENT_P0V75_PCP                  10000
#define RESULT_EXPONENT_P0V9_VDDC_RCA              10000
#define RESULT_EXPONENT_P0V75_VDDC_SOC             10000
#define RESULT_EXPONENT_P1V2_VDDQ_AB               10000
#define RESULT_EXPONENT_P1V2_VDDQ_CD               10000
#define RESULT_EXPONENT_P1V8_PCP                   10000



#endif
