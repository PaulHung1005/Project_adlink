/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MmcLib.h>

#include "HwMonitorHii.h"
// Sensor Commands
UINT8 CmdBuffer_IPMI_P3V3[]      = {"[10 04 2D 01]\r\n"};
UINT8 CmdBuffer_IPMI_P12V[]      = {"[10 08 2D 02]\r\n"};
UINT8 CmdBuffer_IPMI_P5V[]       = {"[10 0C 2D 03]\r\n"};
UINT8 CmdBuffer_P1V5_VDDH[]      = {"[10 10 2D 05]\r\n"};
UINT8 CmdBuffer_P0V75_PCP[]      = {"[10 14 2D 06]\r\n"};
UINT8 CmdBuffer_P0V9_VDDC_RCA[]  = {"[10 18 2D 07]\r\n"};
UINT8 CmdBuffer_P0V75_VDDC_SOC[] = {"[10 1C 2D 08]\r\n"};
UINT8 CmdBuffer_P1V2_VDDQ_AB[]   = {"[10 20 2D 09]\r\n"};
UINT8 CmdBuffer_P1V2_VDDQ_CD[]   = {"[10 24 2D 0A]\r\n"};
UINT8 CmdBuffer_P1V8_PCP[]       = {"[10 28 2D 0B]\r\n"};
UINT8 CmdBuffer_CPU_TEMP[]       = {"[10 2C 2D 0C]\r\n"};

// Define all sensor commands
UINT8* CmdBuffers[] = {
    CmdBuffer_IPMI_P3V3, CmdBuffer_IPMI_P12V, CmdBuffer_IPMI_P5V, 
    CmdBuffer_P1V5_VDDH, CmdBuffer_P0V75_PCP, CmdBuffer_P0V9_VDDC_RCA, 
    CmdBuffer_P0V75_VDDC_SOC, CmdBuffer_P1V2_VDDQ_AB, CmdBuffer_P1V2_VDDQ_CD, 
    CmdBuffer_P1V8_PCP, CmdBuffer_CPU_TEMP
  };
 
// Define corresponding Constatnt_Multiplier, Signed_Offset, and Result_Exponent values
INT32 Constant_Multiplier[] = {
    CONSTANT_MULTIPLIER_IPMI_P3V3, CONSTANT_MULTIPLIER_IPMI_P12V, CONSTANT_MULTIPLIER_IPMI_P5V, 
    CONSTANT_MULTIPLIER_P1V5_VDDH, CONSTANT_MULTIPLIER_P0V75_PCP, CONSTANT_MULTIPLIER_P0V9_VDDC_RCA, 
    CONSTANT_MULTIPLIER_P0V75_VDDC_SOC, CONSTANT_MULTIPLIER_P1V2_VDDQ_AB, CONSTANT_MULTIPLIER_P1V2_VDDQ_CD, 
    CONSTANT_MULTIPLIER_P1V8_PCP, CONSTANT_MULTIPLIER_CPU_TEMP
  };
INT32 Result_Exponent[] = {
    RESULT_EXPONENT_IPMI_P3V3, RESULT_EXPONENT_IPMI_P12V, RESULT_EXPONENT_IPMI_P5V, 
    RESULT_EXPONENT_P1V5_VDDH, RESULT_EXPONENT_P0V75_PCP, RESULT_EXPONENT_P0V9_VDDC_RCA, 
    RESULT_EXPONENT_P0V75_VDDC_SOC, RESULT_EXPONENT_P1V2_VDDQ_AB, RESULT_EXPONENT_P1V2_VDDQ_CD, 
    RESULT_EXPONENT_P1V8_PCP, RESULT_EXPONENT_CPU_TEMP
  };
INT32 Signed_Offset[] = {
    SIGNED_OFFSET_IPMI_P3V3, SIGNED_OFFSET_IPMI_P12V, SIGNED_OFFSET_IPMI_P5V, 
    SIGNED_OFFSET_P1V5_VDDH, SIGNED_OFFSET_P0V75_PCP, SIGNED_OFFSET_P0V9_VDDC_RCA, 
    SIGNED_OFFSET_P0V75_VDDC_SOC, SIGNED_OFFSET_P1V2_VDDQ_AB, SIGNED_OFFSET_P1V2_VDDQ_CD, 
    SIGNED_OFFSET_P1V8_PCP, SIGNED_OFFSET_CPU_TEMP
  };
 
// Define corresponding HII string tokens
EFI_STRING_ID StringTokens[] = {
    STRING_TOKEN(STR_IPMI_P3V3_VALUE), STRING_TOKEN(STR_IPMI_P12V_VALUE),
    STRING_TOKEN(STR_IPMI_P5V_VALUE), STRING_TOKEN(STR_P1V5_VDDH_VALUE),
    STRING_TOKEN(STR_P0V75_PCP_VALUE), STRING_TOKEN(STR_P0V9_VDDC_RCA_VALUE),
    STRING_TOKEN(STR_P0V75_VDDC_SOC_VALUE), STRING_TOKEN(STR_P1V2_VDDQ_AB_VALUE),
    STRING_TOKEN(STR_P1V2_VDDQ_CD_VALUE), STRING_TOKEN(STR_P1V8_PCP_VALUE),
    STRING_TOKEN(STR_CPU_CORE_TEMP_VALUE)
  };

//
// uni string and Vfr Binary data.
//
extern UINT8 HwMonitorVfrBin[];
extern UINT8 HwMonitorDxeStrings[];

EFI_HANDLE     mDriverHandle = NULL;
EFI_HII_HANDLE mHiiHandle = NULL;

#pragma pack(1)

//
// HII specific Vendor Device Path definition.
//
typedef struct {
  VENDOR_DEVICE_PATH       VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

// HW_MONITOR_FORMSET_GUID
EFI_GUID gHardwareMonitorFormsetGuid = HW_MONITOR_FORMSET_GUID;

HII_VENDOR_DEVICE_PATH mHwMonitorHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    HW_MONITOR_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(END_DEVICE_PATH_LENGTH),
      (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};



/** 
    Structure for storing sensor reading conversion value based on 
	the IPMI v2.0 specification Document Revision 1.1.  
    The resulting sensor value is a floating-point number, represented as an integer and fractional part.
**/

typedef struct {
    INT16 Integer_part;        // Integer portion of the sensor value
    INT16 Fractional_part;     // Fractional portion of the sensor value
} SENSOR_READING;

UINTN
parse_char_to_int (
  IN CHAR16 c
)
{
    if ('0' <= c && c <= '9'){
      return c - '0';
      }
    if ('a' <= c && c <= 'f'){ 
      return 10 + c - 'a';
      }
    if ('A' <= c && c <= 'F'){ 
      return 10 + c - 'A';
      }
    else{
      return 0;
    }      
}

/**
  Compute the sensor reading from raw data based on Sensor Reading Conversion Formula 
  in IPMI v2.0 specification Document Revision 1.1.
  Refer: 36.3 Sensor Reading Conversion Formula.

  This function processes the raw sensor data buffer and applies the given 
  conversion parameters to compute the final sensor reading. The reading 
  is represented as an integer and fractional part.

  @param[in]  Buffer                Pointer to the raw sensor data buffer.
  @param[in]  CONSTANT_MULTIPLIER   Multiplier applied to the raw sensor value.
  @param[in]  RESULT_EXPONENT       Exponent used for scaling the result.
  @param[in]  SIGNED_OFFSET         Signed offset applied to the fractional part.

  @retval     SENSOR_READING        Struct containing the computed integer and fractional parts 
                                    of the sensor conversion value.
**/

SENSOR_READING 
ComputeSensorReading (
    UINT8* Buffer, 
	INT32 Constant_Multiplier, 
	INT32 Result_Exponent, 
	INT32 Signed_Offset
) 
{
  SENSOR_READING     Sensor_read;
  UINTN              Raw_value;
  CHAR16             Raw[MAX_STRING_SIZE];
  
// Convert an ASCII string to an integer. 
  AsciiStrToUnicodeStrS((const CHAR8 *)Buffer, Raw, MAX_STRING_SIZE);
  Raw_value = parse_char_to_int(Raw[0]) * HEX_BASE + parse_char_to_int(Raw[1]);
// Compute sensor readings from raw value.
  Sensor_read.Integer_part = (Raw_value * Constant_Multiplier) / Result_Exponent;
  Sensor_read.Fractional_part = (Raw_value * Constant_Multiplier) % Result_Exponent;
  Sensor_read.Fractional_part += Signed_Offset;
// Adjust integer and fractional parts for overflow/underflow based on Signed_offset. 
  if (Signed_Offset >= 0) {
    if (Sensor_read.Fractional_part >= Result_Exponent) {
      Sensor_read.Integer_part++;
      Sensor_read.Fractional_part -= Result_Exponent;
    }
  } else {
    if (Sensor_read.Fractional_part < 0) {
      Sensor_read.Integer_part--;
      Sensor_read.Fractional_part += Result_Exponent;
    }
  }
  return Sensor_read;
}

STATIC
EFI_STATUS
UpdatePlatformInfoScreen (
  IN EFI_HII_HANDLE *HiiHandle
  )
{
  CHAR16             Sensor_value[MAX_STRING_SIZE];
  VOID               *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL *StartLabel;
  VOID               *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL *EndLabel;
  EFI_STATUS         Status;
  UINT8              Buffer[2];
  SENSOR_READING     Sensor_read;
  
  // Iterate through command buffers to get sensor readings and computes the sensor value.
  for (UINTN IpmiCmdBuf = 0; IpmiCmdBuf < ARRAY_SIZE(CmdBuffers); IpmiCmdBuf++) {
    Status = Get_Sensor_Reading(Buffer, sizeof(Buffer), CmdBuffers[IpmiCmdBuf], CMD_BUFFER_SIZE);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Sensor %d command failed\n", IpmiCmdBuf));
      continue;
    }
 
    Sensor_read = ComputeSensorReading(Buffer, Constant_Multiplier[IpmiCmdBuf], Result_Exponent[IpmiCmdBuf], Signed_Offset[IpmiCmdBuf]);
 
    // CPU temperature sensor readings are displayed in degrees Celsius.  
    // Other sensor readings are displayed as voltage with three decimal places.
    if (IpmiCmdBuf == ARRAY_SIZE(CmdBuffers) - 1) { 
      UnicodeSPrint(Sensor_value, sizeof(Sensor_value), L": %d°C", Sensor_read.Integer_part); 
 } else {
      UnicodeSPrint(Sensor_value, sizeof(Sensor_value), L": %d.%03d V", Sensor_read.Integer_part, Sensor_read.Fractional_part);//
    }
    // Update the HII string with the sensor value if no error occurs.  
   // Log an error message if sensor reading fails.
    if (!EFI_ERROR(Status)) {
      HiiSetString(HiiHandle, StringTokens[IpmiCmdBuf], Sensor_value, NULL);
    } else {
      DEBUG((DEBUG_ERROR, "Error retrieving sensor %d\n", IpmiCmdBuf));
    }
  }
  /* Initialize the container for dynamic opcodes */
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  /* Create Hii Extend Label OpCode as the start opcode */
  StartLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                       );
  ASSERT (StartLabel != NULL);
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_UPDATE;

  /* Create Hii Extend Label OpCode as the end opcode */
  EndLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                     EndOpCodeHandle,
                                     &gEfiIfrTianoGuid,
                                     NULL,
                                     sizeof (EFI_IFR_GUID_LABEL)
                                     );
  ASSERT (EndLabel != NULL);
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  HiiUpdateForm (
    mHiiHandle,                 // HII handle
    &gHardwareMonitorFormsetGuid,  // Formset GUID
    HW_MONITOR_FORM_ID,      // Form ID
    StartOpCodeHandle,          // Label for where to insert opcodes
    EndOpCodeHandle             // Insert data
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformInfoUnload (
  VOID
  )
{
  if (mDriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           mDriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mHwMonitorHiiVendorDevicePath,
           NULL
           );
    mDriverHandle = NULL;
  }

  if (mHiiHandle != NULL) {
    HiiRemovePackages (mHiiHandle);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformInfoEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mDriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHwMonitorHiiVendorDevicePath,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  mHiiHandle = HiiAddPackages (
                 &gHardwareMonitorFormsetGuid,
                 mDriverHandle,
                 HwMonitorDxeStrings,
                 HwMonitorVfrBin,
                 NULL
                 );
  if (mHiiHandle == NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           mDriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mHwMonitorHiiVendorDevicePath,
           NULL
           );
    return EFI_OUT_OF_RESOURCES;
  }

  Status = UpdatePlatformInfoScreen (mHiiHandle);
  if (EFI_ERROR (Status)) {
    PlatformInfoUnload ();
    DEBUG ((
      DEBUG_ERROR,
      "%a %d Fail to update the hardware monitor screen \n",
      __FUNCTION__,
      __LINE__
      ));
    return Status;
  }

  return EFI_SUCCESS;
}
