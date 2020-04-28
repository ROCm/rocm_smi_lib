/*
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2017, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD ROC Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimers in
 *    the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *    nor the names of its contributors may be used to endorse or promote
 *    products derived from this Software without specific prior written
 *    permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */
#ifndef INCLUDE_ROCM_SMI_ROCM_SMI_H_
#define INCLUDE_ROCM_SMI_ROCM_SMI_H_

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif  // __cplusplus

#include <stddef.h>

/** \file rocm_smi.h
 *  Main header file for the ROCm SMI library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @brief The rocm_smi library api is new, and therefore subject to change
 *  either at the ABI or API level. Instead of marking every function prototype as "unstable", we are
 *  instead saying the API is unstable (i.e., changes are possible) while the
 *  major version remains 0. This means that if the API/ABI changes, we will
 *  not increment the major version to 1. Once the ABI stabilizes, we will
 *  increment the major version to 1, and thereafter increment it on all ABI
 *  breaks.
 */

//! Guaranteed maximum possible number of supported frequencies
#define RSMI_MAX_NUM_FREQUENCIES 32

//! Maximum possible value for fan speed. Should be used as the denominator
//! when determining fan speed percentage.
#define RSMI_MAX_FAN_SPEED 255

//! The number of points that make up a voltage-frequency curve definition
#define RSMI_NUM_VOLTAGE_CURVE_POINTS 3

/**
 * @brief Error codes retured by rocm_smi_lib functions
 */
typedef enum {
  RSMI_STATUS_SUCCESS = 0x0,             //!< Operation was successful
  RSMI_STATUS_INVALID_ARGS,              //!< Passed in arguments are not valid
  RSMI_STATUS_NOT_SUPPORTED,             //!< The requested information or
                                         //!< action is not available for the
                                         //!< given input, on the given system
  RSMI_STATUS_FILE_ERROR,                //!< Problem accessing a file. This
                                         //!< may because the operation is not
                                         //!< supported by the Linux kernel
                                         //!< version running on the executing
                                         //!< machine
  RSMI_STATUS_PERMISSION,                //!< Permission denied/EACCESS file
                                         //!< error. Many functions require
                                         //!< root access to run.
  RSMI_STATUS_OUT_OF_RESOURCES,          //!< Unable to acquire memory or other
                                         //!< resource
  RSMI_STATUS_INTERNAL_EXCEPTION,        //!< An internal exception was caught
  RSMI_STATUS_INPUT_OUT_OF_BOUNDS,       //!< The provided input is out of
                                         //!< allowable or safe range
  RSMI_STATUS_INIT_ERROR,                //!< An error occurred when rsmi
                                         //!< initializing internal data
                                         //!< structures
  RSMI_INITIALIZATION_ERROR = RSMI_STATUS_INIT_ERROR,
  RSMI_STATUS_NOT_YET_IMPLEMENTED,       //!< The requested function has not
                                         //!< yet been implemented in the
                                         //!< current system for the current
                                         //!< devices
  RSMI_STATUS_NOT_FOUND,                 //!< An item was searched for but not
                                         //!< found
  RSMI_STATUS_INSUFFICIENT_SIZE,         //!< Not enough resources were
                                         //!< available for the operation
  RSMI_STATUS_INTERRUPT,                 //!< An interrupt occurred during
                                         //!< execution of function
  RSMI_STATUS_UNEXPECTED_SIZE,           //!< An unexpected amount of data
                                         //!< was read
  RSMI_STATUS_NO_DATA,                   //!< No data was found for a given
                                         //!< input
  RSMI_STATUS_UNEXPECTED_DATA,           //!< The data read or provided to
                                         //!< function is not what was expected
  RSMI_STATUS_UNKNOWN_ERROR = 0xFFFFFFFF,  //!< An unknown error occurred
} rsmi_status_t;

/**
 * @brief Initialization flags
 *
 * Initialization flags may be OR'd together and passed to ::rsmi_init().
 */

typedef enum {
  RSMI_INIT_FLAG_ALL_GPUS      = 0x1,    //!< Attempt to add all GPUs found
                                         //!< (including non-AMD) to the list
                                         //!< of devices from which SMI
                                         //!< information can be retrieved. By
                                         //!< default, only AMD devices are
                                         //!<  ennumerated by RSMI.
} rsmi_init_flags_t;

/**
 * @brief PowerPlay performance levels
 */
typedef enum {
  RSMI_DEV_PERF_LEVEL_AUTO = 0,       //!< Performance level is "auto"
  RSMI_DEV_PERF_LEVEL_FIRST = RSMI_DEV_PERF_LEVEL_AUTO,

  RSMI_DEV_PERF_LEVEL_LOW,              //!< Keep PowerPlay levels "low",
                                        //!< regardless of workload
  RSMI_DEV_PERF_LEVEL_HIGH,             //!< Keep PowerPlay levels "high",
                                        //!< regardless of workload
  RSMI_DEV_PERF_LEVEL_MANUAL,           //!< Only use values defined by manually
                                        //!< setting the RSMI_CLK_TYPE_SYS speed
  RSMI_DEV_PERF_LEVEL_STABLE_STD,       //!< Stable power state with profiling
                                        //!< clocks
  RSMI_DEV_PERF_LEVEL_STABLE_PEAK,      //!< Stable power state with peak clocks
  RSMI_DEV_PERF_LEVEL_STABLE_MIN_MCLK,  //!< Stable power state with minimum
                                        //!< memory clock
  RSMI_DEV_PERF_LEVEL_STABLE_MIN_SCLK,  //!< Stable power state with minimum
                                        //!< system clock

  RSMI_DEV_PERF_LEVEL_LAST = RSMI_DEV_PERF_LEVEL_STABLE_MIN_SCLK,

  RSMI_DEV_PERF_LEVEL_UNKNOWN = 0x100   //!< Unknown performance level
} rsmi_dev_perf_level_t;
/// \cond Ignore in docs.
typedef rsmi_dev_perf_level_t rsmi_dev_perf_level;
/// \endcond
/**
 * @brief Available clock types.
 */

/**
 * @brief Software components
 */
typedef enum {
  RSMI_SW_COMP_FIRST = 0x0,

  RSMI_SW_COMP_DRIVER = RSMI_SW_COMP_FIRST,    //!< Driver

  RSMI_SW_COMP_LAST = RSMI_SW_COMP_DRIVER
} rsmi_sw_component_t;

/**
 * Event counter types
 */

/**
 * @brief Handle to performance event counter
 */
typedef uintptr_t rsmi_event_handle_t;

/**
 * Event Groups
 *
 * @brief Enum denoting an event group. The value of the enum is the
 * base value for all the event enums in the group.
 */
typedef enum {
  RSMI_EVNT_GRP_XGMI = 0,         //!< Data Fabric (XGMI) related events

  RSMI_EVNT_GRP_INVALID = 0xFFFFFFFF
} rsmi_event_group_t;

/**
 * Event types
 * @brief Event type enum. Events belonging to a particular event group
 * ::rsmi_event_group_t should begin ennumerating at the ::rsmi_event_group_t
 * value for that group.
 */
typedef enum {
  RSMI_EVNT_FIRST = RSMI_EVNT_GRP_XGMI,

  RSMI_EVNT_XGMI_FIRST = RSMI_EVNT_GRP_XGMI,
  RSMI_EVNT_XGMI_0_NOP_TX = RSMI_EVNT_XGMI_FIRST,  //!< NOPs sent to neighbor 0
  RSMI_EVNT_XGMI_0_REQUEST_TX,                    //!< Outgoing requests to
                                                  //!< neighbor 0
  RSMI_EVNT_XGMI_0_RESPONSE_TX,                   //!< Outgoing responses to
                                                  //!< neighbor 0
  RSMI_EVNT_XGMI_0_BEATS_TX,                      //!< Data beats sent to
                                                  //!< neighbor 0
  RSMI_EVNT_XGMI_1_NOP_TX,                        //!< NOPs sent to neighbor 1
  RSMI_EVNT_XGMI_1_REQUEST_TX,                        //!< Outgoing requests to
                                                  //!< neighbor 1
  RSMI_EVNT_XGMI_1_RESPONSE_TX,                   //!< Outgoing responses to
                                                  //!< neighbor 1
  RSMI_EVNT_XGMI_1_BEATS_TX,                      //!< Data beats sent to
                                                  //!< neighbor 1

  RSMI_EVNT_XGMI_LAST = RSMI_EVNT_XGMI_1_BEATS_TX,

  RSMI_EVNT_LAST = RSMI_EVNT_XGMI_LAST
} rsmi_event_type_t;

/**
 * Event counter commands
 */
typedef enum {
  RSMI_CNTR_CMD_START = 0,  //!< Start the counter
  RSMI_CNTR_CMD_STOP,       //!< Stop the counter
} rsmi_counter_command_t;

/**
 * Counter value
 */
typedef struct {
  uint64_t value;            //!< Counter value
  uint64_t time_enabled;     //!< Time that the counter was enabled
  uint64_t time_running;     //!< Time that che counter was running
} rsmi_counter_value_t;

/**
 * Clock types
 */
typedef enum {
  RSMI_CLK_TYPE_SYS = 0x0,            //!< System clock
  RSMI_CLK_TYPE_FIRST = RSMI_CLK_TYPE_SYS,
  RSMI_CLK_TYPE_DF,                   //!< Data Fabric clock (for ASICs
                                      //!< running on a separate clock)
  RSMI_CLK_TYPE_DCEF,                 //!< Display Controller Engine clock
  RSMI_CLK_TYPE_SOC,                  //!< SOC clock
  RSMI_CLK_TYPE_MEM,                  //!< Memory clock

  // Add new clocks to the end (not in the middle) and update
  // RSMI_CLK_TYPE_LAST
  RSMI_CLK_TYPE_LAST = RSMI_CLK_TYPE_MEM,
  RSMI_CLK_INVALID = 0xFFFFFFFF
} rsmi_clk_type_t;
/// \cond Ignore in docs.
typedef rsmi_clk_type_t rsmi_clk_type;
/// \endcond

/**
 * @brief Temperature Metrics.  This enum is used to identify various
 * temperature metrics. Corresponding values will be in millidegress
 * Celcius.
 */
typedef enum {
  RSMI_TEMP_CURRENT = 0x0,   //!< Temperature current value.
  RSMI_TEMP_FIRST = RSMI_TEMP_CURRENT,

  RSMI_TEMP_MAX,             //!< Temperature max value.
  RSMI_TEMP_MIN,             //!< Temperature min value.
  RSMI_TEMP_MAX_HYST,        //!< Temperature hysteresis value for max limit.
                             //!< (This is an absolute temperature, not a
                             //!< delta).
  RSMI_TEMP_MIN_HYST,        //!< Temperature hysteresis value for min limit.
                             //!< (This is an absolute temperature,
                             //!<  not a delta).
  RSMI_TEMP_CRITICAL,        //!< Temperature critical max value, typically
                             //!<  greater than corresponding temp_max values.
  RSMI_TEMP_CRITICAL_HYST,   //!< Temperature hysteresis value for critical
                             //!<  limit. (This is an absolute temperature,
                             //!<  not a delta).
  RSMI_TEMP_EMERGENCY,       //!< Temperature emergency max value, for chips
                             //!<  supporting more than two upper temperature
                             //!<  limits. Must be equal or greater than
                             //!<  corresponding temp_crit values.
  RSMI_TEMP_EMERGENCY_HYST,  //!< Temperature hysteresis value for emergency
                             //!<  limit. (This is an absolute temperature,
                             //!<  not a delta).
  RSMI_TEMP_CRIT_MIN,        //!< Temperature critical min value, typically
                             //!<  lower than corresponding temperature
                             //!<  minimum values.
  RSMI_TEMP_CRIT_MIN_HYST,   //!< Temperature hysteresis value for critical
                             //!< minimum limit. (This is an absolute
                             //!< temperature, not a delta).
  RSMI_TEMP_OFFSET,          //!< Temperature offset which is added to the
                             //!  temperature reading by the chip.
  RSMI_TEMP_LOWEST,          //!< Historical minimum temperature.
  RSMI_TEMP_HIGHEST,         //!< Historical maximum temperature.

  RSMI_TEMP_LAST = RSMI_TEMP_HIGHEST
} rsmi_temperature_metric_t;
/// \cond Ignore in docs.
typedef rsmi_temperature_metric_t rsmi_temperature_metric;
/// \endcond

/**
 * @brief This ennumeration is used to indicate from which part of the device a
 * temperature reading should be obtained.
 */
typedef enum {
  RSMI_TEMP_TYPE_FIRST = 0,

  RSMI_TEMP_TYPE_EDGE = RSMI_TEMP_TYPE_FIRST,  //!< Edge GPU temperature
  RSMI_TEMP_TYPE_JUNCTION,                     //!< Junction/hotspot
                                               //!< temperature
  RSMI_TEMP_TYPE_MEMORY,                       //!< VRAM temperature

  RSMI_TEMP_TYPE_LAST = RSMI_TEMP_TYPE_MEMORY,
  RSMI_TEMP_TYPE_INVALID = 0xFFFFFFFF          //!< Invalid type
} rsmi_temperature_type_t;

/**
 * @brief Pre-set Profile Selections. These bitmasks can be AND'd with the
 * ::rsmi_power_profile_status_t.available_profiles returned from
 * ::rsmi_dev_power_profile_presets_get to determine which power profiles
 * are supported by the system.
 */
typedef enum {
  RSMI_PWR_PROF_PRST_CUSTOM_MASK = 0x1,        //!< Custom Power Profile
  RSMI_PWR_PROF_PRST_VIDEO_MASK = 0x2,         //!< Video Power Profile
  RSMI_PWR_PROF_PRST_POWER_SAVING_MASK = 0x4,  //!< Power Saving Profile
  RSMI_PWR_PROF_PRST_COMPUTE_MASK = 0x8,       //!< Compute Saving Profile
  RSMI_PWR_PROF_PRST_VR_MASK = 0x10,           //!< VR Power Profile

  //!< 3D Full Screen Power Profile
  RSMI_PWR_PROF_PRST_3D_FULL_SCR_MASK = 0x20,
  RSMI_PWR_PROF_PRST_BOOTUP_DEFAULT = 0x40,    //!< Default Boot Up Profile
  RSMI_PWR_PROF_PRST_LAST = RSMI_PWR_PROF_PRST_BOOTUP_DEFAULT,

  //!< Invalid power profile
  RSMI_PWR_PROF_PRST_INVALID = 0xFFFFFFFFFFFFFFFF
} rsmi_power_profile_preset_masks_t;
/// \cond Ignore in docs.
typedef rsmi_power_profile_preset_masks_t rsmi_power_profile_preset_masks;
/// \endcond

/**
 * @brief This enum is used to identify different GPU blocks.
 */
typedef enum {
  RSMI_GPU_BLOCK_INVALID =   0x0000000000000000,  //!< Used to indicate an
                                                  //!< invalid block
  RSMI_GPU_BLOCK_FIRST =     0x0000000000000001,

  RSMI_GPU_BLOCK_UMC = RSMI_GPU_BLOCK_FIRST,      //!< UMC block
  RSMI_GPU_BLOCK_SDMA =      0x0000000000000002,  //!< SDMA block
  RSMI_GPU_BLOCK_GFX =       0x0000000000000004,  //!< GFX block
  RSMI_GPU_BLOCK_MMHUB =     0x0000000000000008,  //!< MMHUB block
  RSMI_GPU_BLOCK_ATHUB =     0x0000000000000010,  //!< ATHUB block
  RSMI_GPU_BLOCK_PCIE_BIF =  0x0000000000000020,  //!< PCIE_BIF block
  RSMI_GPU_BLOCK_HDP =       0x0000000000000040,  //!< HDP block
  RSMI_GPU_BLOCK_XGMI_WAFL = 0x0000000000000080,  //!< XGMI block
  RSMI_GPU_BLOCK_DF =        0x0000000000000100,  //!< DF block
  RSMI_GPU_BLOCK_SMN =       0x0000000000000200,  //!< SMN block
  RSMI_GPU_BLOCK_SEM =       0x0000000000000400,  //!< SEM block
  RSMI_GPU_BLOCK_MP0 =       0x0000000000000800,  //!< MP0 block
  RSMI_GPU_BLOCK_MP1 =       0x0000000000001000,  //!< MP1 block
  RSMI_GPU_BLOCK_FUSE =      0x0000000000002000,  //!< Fuse block

  RSMI_GPU_BLOCK_LAST = RSMI_GPU_BLOCK_FUSE,       //!< The highest bit position
                                                  //!< for supported blocks
  RSMI_GPU_BLOCK_RESERVED =  0x8000000000000000
} rsmi_gpu_block_t;
/// \cond Ignore in docs.
typedef rsmi_gpu_block_t rsmi_gpu_block;
/// \endcond

/**
 * @brief The current ECC state
 */
typedef enum {
  RSMI_RAS_ERR_STATE_NONE = 0,   //!< No current errors
  RSMI_RAS_ERR_STATE_DISABLED,   //!< ECC is disabled
  RSMI_RAS_ERR_STATE_PARITY,     //!< ECC errors present, but type unknown
  RSMI_RAS_ERR_STATE_SING_C,     //!< Single correctable error
  RSMI_RAS_ERR_STATE_MULT_UC,    //!< Multiple uncorrectable errors
  RSMI_RAS_ERR_STATE_POISON,     //!< Firmware detected error and isolated
                                 //!< page. Treat as uncorrectable.
  RSMI_RAS_ERR_STATE_ENABLED,    //!< ECC is enabled

  RSMI_RAS_ERR_STATE_LAST = RSMI_RAS_ERR_STATE_ENABLED,
  RSMI_RAS_ERR_STATE_INVALID = 0xFFFFFFFF
} rsmi_ras_err_state_t;

/**
 * @brief Types of memory
 */
typedef enum {
  RSMI_MEM_TYPE_FIRST = 0,

  RSMI_MEM_TYPE_VRAM = RSMI_MEM_TYPE_FIRST,  //!< VRAM memory
  RSMI_MEM_TYPE_VIS_VRAM,                    //!< VRAM memory that is visible
  RSMI_MEM_TYPE_GTT,                         //!< GTT memory

  RSMI_MEM_TYPE_LAST = RSMI_MEM_TYPE_GTT
} rsmi_memory_type_t;

/**
 * @brief The values of this enum are used as frequency identifiers.
 */
typedef enum {
  RSMI_FREQ_IND_MIN = 0,  //!< Index used for the minimum frequency value
  RSMI_FREQ_IND_MAX = 1,  //!< Index used for the maximum frequency value
  RSMI_FREQ_IND_INVALID = 0xFFFFFFFF  //!< An invalid frequency index
} rsmi_freq_ind_t;
/// \cond Ignore in docs.
typedef rsmi_freq_ind_t rsmi_freq_ind;
/// \endcond


/**
 * @brief The values of this enum are used to identify the various firmware
 * blocks.
 */
typedef enum {
  RSMI_FW_BLOCK_FIRST = 0,

  RSMI_FW_BLOCK_ASD = RSMI_FW_BLOCK_FIRST,
  RSMI_FW_BLOCK_CE,
  RSMI_FW_BLOCK_DMCU,
  RSMI_FW_BLOCK_MC,
  RSMI_FW_BLOCK_ME,
  RSMI_FW_BLOCK_MEC,
  RSMI_FW_BLOCK_MEC2,
  RSMI_FW_BLOCK_PFP,
  RSMI_FW_BLOCK_RLC,
  RSMI_FW_BLOCK_RLC_SRLC,
  RSMI_FW_BLOCK_RLC_SRLG,
  RSMI_FW_BLOCK_RLC_SRLS,
  RSMI_FW_BLOCK_SDMA,
  RSMI_FW_BLOCK_SDMA2,
  RSMI_FW_BLOCK_SMC,
  RSMI_FW_BLOCK_SOS,
  RSMI_FW_BLOCK_TA_RAS,
  RSMI_FW_BLOCK_TA_XGMI,
  RSMI_FW_BLOCK_UVD,
  RSMI_FW_BLOCK_VCE,
  RSMI_FW_BLOCK_VCN,

  RSMI_FW_BLOCK_LAST = RSMI_FW_BLOCK_VCN
} rsmi_fw_block_t;

/**
 * @brief XGMI Status
 */
typedef enum {
  RSMI_XGMI_STATUS_NO_ERRORS = 0,
  RSMI_XGMI_STATUS_ERROR,
  RSMI_XGMI_STATUS_MULTIPLE_ERRORS,
} rsmi_xgmi_status_t;

/**
 * @brief Bitfield used in various RSMI calls
 */
typedef uint64_t rsmi_bit_field_t;
/// \cond Ignore in docs.
typedef rsmi_bit_field_t rsmi_bit_field;
/// \endcond

/**
 * @brief Reserved Memory Page States
 */
typedef enum {
  RSMI_MEM_PAGE_STATUS_RESERVED = 0,  //!< Reserved. This gpu page is reserved
                                      //!<  and not available for use
  RSMI_MEM_PAGE_STATUS_PENDING,       //!< Pending. This gpu page is marked
                                      //!<  as bad and will be marked reserved
                                      //!<  at the next window.
  RSMI_MEM_PAGE_STATUS_UNRESERVABLE   //!< Unable to reserve this page
} rsmi_memory_page_status_t;

/**
 * @brief Reserved Memory Page Record
 */
typedef struct {
  uint64_t page_address;                  //!< Start address of page
  uint64_t page_size;                     //!< Page size
  rsmi_memory_page_status_t status;       //!< Page "reserved" status
} rsmi_retired_page_record_t;

/**
 * @brief Number of possible power profiles that a system could support
 */
#define RSMI_MAX_NUM_POWER_PROFILES (sizeof(rsmi_bit_field_t) * 8)

/**
 * @brief This structure contains information about which power profiles are
 * supported by the system for a given device, and which power profile is
 * currently active.
 */
typedef struct {
    /**
    * Which profiles are supported by this system
    */
    rsmi_bit_field_t available_profiles;

    /**
    * Which power profile is currently active
    */
    rsmi_power_profile_preset_masks_t current;

    /**
    * How many power profiles are available
    */
    uint32_t num_profiles;
} rsmi_power_profile_status_t;
/// \cond Ignore in docs.
typedef rsmi_power_profile_status_t rsmi_power_profile_status;
/// \endcond

/**
 * @brief This structure holds information about clock frequencies.
 */
typedef struct {
    /**
     * The number of supported frequencies
     */
    uint32_t num_supported;

    /**
     * The current frequency index
     */
    uint32_t current;

    /**
     * List of frequencies.
     * Only the first num_supported frequencies are valid.
     */
    uint64_t frequency[RSMI_MAX_NUM_FREQUENCIES];
} rsmi_frequencies_t;
/// \cond Ignore in docs.
typedef rsmi_frequencies_t rsmi_frequencies;
/// \endcond

/**
 * @brief This structure holds information about the possible PCIe
 * bandwidths. Specifically, the possible transfer rates and their
 * associated numbers of lanes are stored here.
 */
typedef struct {
    /**
     * Transfer rates (T/s) that are possible
     */
    rsmi_frequencies_t transfer_rate;

    /**
     * List of lanes for corresponding transfer rate.
     * Only the first num_supported bandwidths are valid.
     */
    uint32_t lanes[RSMI_MAX_NUM_FREQUENCIES];
} rsmi_pcie_bandwidth_t;

/// \cond Ignore in docs.
typedef rsmi_pcie_bandwidth_t rsmi_pcie_bandwidth;
/// \endcond

/**
 * @brief This structure holds version information.
 */
typedef struct {
    uint32_t major;     //!< Major version
    uint32_t minor;     //!< Minor version
    uint32_t patch;     //!< Patch, build  or stepping version
    const char *build;  //!< Build string
} rsmi_version_t;
/// \cond Ignore in docs.
typedef rsmi_version_t rsmi_version;
/// \endcond
/**
 * @brief This structure represents a range (e.g., frequencies or voltages).
 */
typedef struct {
    uint64_t lower_bound;      //!< Lower bound of range
    uint64_t upper_bound;      //!< Upper bound of range
} rsmi_range_t;
/// \cond Ignore in docs.
typedef rsmi_range_t rsmi_range;
/// \endcond

/**
 * @brief This structure represents a point on the frequency-voltage plane.
 */
typedef struct {
    uint64_t frequency;      //!< Frequency coordinate (in Hz)
    uint64_t voltage;        //!< Voltage coordinate (in mV)
} rsmi_od_vddc_point_t;
/// \cond Ignore in docs.
typedef rsmi_od_vddc_point_t rsmi_od_vddc_point;
/// \endcond

/**
 * @brief This structure holds 2 ::rsmi_range_t's, one for frequency and one for
 * voltage. These 2 ranges indicate the range of possible values for the
 * corresponding ::rsmi_od_vddc_point_t.
 */
typedef struct {
    rsmi_range_t freq_range;  //!< The frequency range for this VDDC Curve point
    rsmi_range_t volt_range;  //!< The voltage range for this VDDC Curve point
} rsmi_freq_volt_region_t;
/// \cond Ignore in docs.
typedef rsmi_freq_volt_region_t rsmi_freq_volt_region;
/// \endcond

/**
 * ::RSMI_NUM_VOLTAGE_CURVE_POINTS number of ::rsmi_od_vddc_point_t's
 */
typedef struct {
    /**
     * Array of ::RSMI_NUM_VOLTAGE_CURVE_POINTS ::rsmi_od_vddc_point_t's that
     * make up the voltage frequency curve points.
     */
    rsmi_od_vddc_point_t vc_points[RSMI_NUM_VOLTAGE_CURVE_POINTS];
} rsmi_od_volt_curve_t;
/// \cond Ignore in docs.
typedef rsmi_od_volt_curve_t rsmi_od_volt_curve;
/// \endcond

/**
 * @brief This structure holds the frequency-voltage values for a device.
 */
typedef struct {
  rsmi_range_t curr_sclk_range;          //!< The current SCLK frequency range
  rsmi_range_t curr_mclk_range;          //!< The current MCLK frequency range;
                                         //!< (upper bound only)
  rsmi_range_t sclk_freq_limits;         //!< The range possible of SCLK values
  rsmi_range_t mclk_freq_limits;         //!< The range possible of MCLK values

  /**
   * @brief The current voltage curve
   */
  rsmi_od_volt_curve_t curve;
  uint32_t num_regions;                //!< The number of voltage curve regions
} rsmi_od_volt_freq_data_t;
/// \cond Ignore in docs.
typedef rsmi_od_volt_freq_data_t rsmi_od_volt_freq_data;
/// \endcond

/**
 * @brief This structure holds error counts.
 */
typedef struct {
    uint64_t correctable_err;            //!< Accumulated correctable errors
    uint64_t uncorrectable_err;          //!< Accumulated uncorrectable errors
} rsmi_error_count_t;

/**
 * @brief This structure contains information specific to a process.
 */
typedef struct {
    uint32_t process_id;  //!< Process ID
    uint32_t pasid;    //!< PASID
} rsmi_process_info_t;


/**
 * @brief Opaque handle to function-support object
 */
typedef struct rsmi_func_id_iter_handle * rsmi_func_id_iter_handle_t;

//! Place-holder "variant" for functions that have don't have any variants,
//! but do have monitors or sensors.
#define RSMI_DEFAULT_VARIANT 0xFFFFFFFFFFFFFFFF

/**
 * @brief This union holds the value of an ::rsmi_func_id_iter_handle_t. The
 * value may be a function name, or an ennumerated variant value of types
 * such as ::rsmi_memory_type_t, ::rsmi_temperature_metric_t, etc.
 */
typedef union id {
        uint64_t id;           //!< uint64_t representation of value
        const char *name;      //!< name string (applicable to functions only)
        union {
            //!< Used for ::rsmi_memory_type_t variants
            rsmi_memory_type_t memory_type;
            //!< Used for ::rsmi_temperature_metric_t variants
            rsmi_temperature_metric_t temp_metric;
            //!< Used for ::rsmi_event_type_t variants
            rsmi_event_type_t evnt_type;
            //!< Used for ::rsmi_event_group_t variants
            rsmi_event_group_t evnt_group;
            //!< Used for ::rsmi_clk_type_t variants
            rsmi_clk_type_t clk_type;
            //!< Used for ::rsmi_fw_block_t variants
            rsmi_fw_block_t fw_block;
            //!< Used for ::rsmi_gpu_block_t variants
            rsmi_gpu_block_t gpu_block_type;
        };
} rsmi_func_id_value_t;


/*****************************************************************************/
/** @defgroup InitShutAdmin Initialization and Shutdown
 *  These functions are used for initialization of ROCm SMI and clean up when
 *  done.
 *  @{
 */
/**
 *  @brief Initialize ROCm SMI.
 *
 *  @details When called, this initializes internal data structures,
 *  including those corresponding to sources of information that SMI provides.
 *
 *  @param[in] init_flags Bit flags that tell SMI how to initialze. Values of
 *  ::rsmi_init_flags_t may be OR'd together and passed through @p init_flags
 *  to modify how RSMI initializes.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 */
rsmi_status_t rsmi_init(uint64_t init_flags);

/**
 *  @brief Shutdown ROCm SMI.
 *
 *  @details Do any necessary clean up.
 */
rsmi_status_t rsmi_shut_down(void);

/** @} */  // end of InitShut

/*****************************************************************************/
/** @defgroup IDQuer Identifier Queries
 *  These functions provide identification information.
 *  @{
 */
/**
 *  @brief Get the number of devices that have monitor information.
 *
 *  @details The number of devices which have monitors is returned. Monitors
 *  are referenced by the index which can be between 0 and @p num_devices - 1.
 *
 *  @param[inout] num_devices Caller provided pointer to uint32_t. Upon
 *  successful call, the value num_devices will contain the number of monitor
 *  devices.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 */
rsmi_status_t rsmi_num_monitor_devices(uint32_t *num_devices);

/**
 *  @brief Get the device id associated with the device with provided device
 *  index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p id,
 *  this function will write the device id value to the uint64_t pointed to by
 *  @p id. This ID is an identification of the type of device, so calling this
 *  function for different devices will give the same value if they are kind
 *  of device. Consequently, this function should not be used to distinguish
 *  one device from another. rsmi_dev_pci_id_get() should be used to get a
 *  unique identifier.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] id a pointer to uint64_t to which the device id will be
 *  written
 * If this parameter is nullptr, this function will return
 * ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 * arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 * provided arguments.
 *
 * @retval ::RSMI_STATUS_SUCCESS call was successful
 * @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 * support this function with the given arguments
 * @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_id_get(uint32_t dv_ind, uint16_t *id);

/**
 *  @brief Get the device vendor id associated with the device with provided
 *  device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p id,
 *  this function will write the device vendor id value to the uint64_t pointed
 *  to by @p id.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] id a pointer to uint64_t to which the device vendor id will
 *  be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_vendor_id_get(uint32_t dv_ind, uint16_t *id);

/**
 *  @brief Get the name string of a gpu device.
 *
 *  @details Given a device index @p dv_ind, a pointer to a caller provided
 *  char buffer @p name, and a length of this buffer @p len, this function
 *  will write the name of the device (up to @p len characters) to the buffer
 *  @p name.
 *
 *  If the integer ID associated with the device is not found in one of the
 *  system files containing device name information (e.g.
 *  /usr/share/misc/pci.ids), then this function will return the hex device ID
 *  as a string. Updating the system name files can be accompplished with
 *  "sudo update-pciids".
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] name a pointer to a caller provided char buffer to which the
 *  name will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[in] len the length of the caller provided buffer @p name.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 *
 */
rsmi_status_t rsmi_dev_name_get(uint32_t dv_ind, char *name, size_t len);

/**
 *  @brief Get the brand string of a gpu device.
 *
 *  @details Given a device index @p dv_ind, a pointer to a caller provided
 *  char buffer @p brand, and a length of this buffer @p len, this function
 *  will write the brand of the device (up to @p len characters) to the buffer
 *  @p brand.
 *
 *  If the sku associated with the device is not found as one of the values
 *  contained within rsmi_dev_brand_get, then this function will return the
 *  device marketing name as a string instead of the brand name.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] brand a pointer to a caller provided char buffer to which the
 *  brand will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[in] len the length of the caller provided buffer @p brand.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 *
 */
rsmi_status_t rsmi_dev_brand_get(uint32_t dv_ind, char *brand, uint32_t len);

/**
 *  @brief Get the name string for a give vendor ID
 *
 *  @details Given a device index @p dv_ind, a pointer to a caller provided
 *  char buffer @p name, and a length of this buffer @p len, this function will
 *  write the name of the vendor (up to @p len characters) buffer @p name. The
 *  @p id may be a device vendor or subsystem vendor ID.
 *
 *  If the integer ID associated with the vendor is not found in one of the
 *  system files containing device name information (e.g.
 *  /usr/share/misc/pci.ids), then this function will return the hex vendor ID
 *  as a string. Updating the system name files can be accompplished with
 *  "sudo update-pciids".
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] name a pointer to a caller provided char buffer to which the
 *  name will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[in] len the length of the caller provided buffer @p name.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 *
 */
rsmi_status_t rsmi_dev_vendor_name_get(uint32_t dv_ind, char *name,
                                                                  size_t len);

/**
 *  @brief Get the vram vendor string of a gpu device.
 *
 *  @details Given a device index @p dv_ind, a pointer to a caller provided
 *  char buffer @p brand, and a length of this buffer @p len, this function
 *  will write the vram vendor of the device (up to @p len characters) to the
 *  buffer @p brand.
 *
 *  If the vram vendor for the device is not found as one of the values
 *  contained within rsmi_dev_vram_vendor_get, then this function will return
 *  the string 'unknown' instead of the vram vendor.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] brand a pointer to a caller provided char buffer to which the
 *  vram vendor will be written
 *
 *  @param[in] len the length of the caller provided buffer @p brand.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t rsmi_dev_vram_vendor_get(uint32_t dv_ind, char *brand,
                                                                uint32_t len);

/**
 * @brief Get the serial number string for a device
 *
 * @details Given a device index @p dv_ind, a pointer to a buffer of chars
 * @p serial_num, and the length of the provided buffer @p len, this function
 * will write the serial number string (up to @p len characters) to the buffer
 * pointed to by @p serial_num.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] serial_num a pointer to caller-provided memory to which the
 *  serial number will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.

 *  @param[in] len the length of the caller provided buffer @p serial_num.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 *
 */
rsmi_status_t rsmi_dev_serial_number_get(uint32_t dv_ind,
                                              char *serial_num, uint32_t len);
/**
 *  @brief Get the subsystem device id associated with the device with
 *  provided device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p id,
 *  this function will write the subsystem device id value to the uint64_t
 *  pointed to by @p id.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] id a pointer to uint64_t to which the subsystem device id
 *  will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_subsystem_id_get(uint32_t dv_ind, uint16_t *id);

/**
 *  @brief Get the name string for the device subsytem
 *
 *  @details Given a device index @p dv_ind, a pointer to a caller provided
 *  char buffer @p name, and a length of this buffer @p len, this function
 *  will write the name of the device subsystem (up to @p len characters)
 *  to the buffer @p name.
 *
 *  If the integer ID associated with the sub-system is not found in one of the
 *  system files containing device name information (e.g.
 *  /usr/share/misc/pci.ids), then this function will return the hex sub-system
 *  ID as a string. Updating the system name files can be accompplished with
 *  "sudo update-pciids".
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] name a pointer to a caller provided char buffer to which the
 *  name will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.

 *  @param[in] len the length of the caller provided buffer @p name.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 *
 */
rsmi_status_t
rsmi_dev_subsystem_name_get(uint32_t dv_ind, char *name, size_t len);

/**
 *  @brief Get the drm minor number associated with this device
 *
 *  @details Given a device index @p dv_ind, find its render device file
 *  /dev/dri/renderDN where N corresponds to its minor number.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] minor a pointer to a uint32_t into which minor number will
 *  be copied
 *
 *  @retval :: RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval :: RSMI_STATUS_INIT_ERROR if failed to get minor number during
 *  initialization.
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_drm_render_minor_get(uint32_t dv_ind, uint32_t *minor);

/**
 *  @brief Get the device subsystem vendor id associated with the device with
 *  provided device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p id,
 *  this function will write the device subsystem vendor id value to the
 *  uint64_t pointed to by @p id.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] id a pointer to uint64_t to which the device subsystem vendor
 *  id will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_subsystem_vendor_id_get(uint32_t dv_ind, uint16_t *id);

/**
 *  @brief Get Unique ID
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint64_t @p
 *  id, this function will write the unique ID of the GPU pointed to @p
 *  id.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] id a pointer to uint64_t to which the unique ID of the GPU
 *  is written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_unique_id_get(uint32_t dv_ind, uint64_t *id);

/** @} */  // end of IDQuer

/*****************************************************************************/
/** @defgroup PCIeQuer PCIe Queries
 *  These functions provide information about PCIe.
 *  @{
 */
/**
 *  @brief Get the list of possible PCIe bandwidths that are available.
 *
 *  @details Given a device index @p dv_ind and a pointer to a to an
 *  ::rsmi_pcie_bandwidth_t structure @p bandwidth, this function will fill in
 *  @p bandwidth with the possible T/s values and associated number of lanes,
 *  and indication of the current selection.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] bandwidth a pointer to a caller provided
 *  ::rsmi_pcie_bandwidth_t structure to which the frequency information will be
 *  written
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_dev_pci_bandwidth_get(uint32_t dv_ind, rsmi_pcie_bandwidth_t *bandwidth);

/**
 *  @brief Get the unique PCI device identifier associated for a device
 *
 *  @details Give a device index @p dv_ind and a pointer to a uint64_t @p
 *  bdfid, this function will write the Bus/Device/Function PCI identifier
 *  (BDFID) associated with device @p dv_ind to the value pointed to by
 *  @p bdfid.
 *
 *  The format of @p bdfid will be as follows:
 *
 *      BDFID = ((DOMAIN & 0xffffffff) << 32) | ((BUS & 0xff) << 8) |
 *                                   ((DEVICE & 0x1f) <<3 ) | (FUNCTION & 0x7)
 *
 *  | Name     | Field   |
 *  ---------- | ------- |
 *  | Domain   | [64:32] |
 *  | Reserved | [31:16] |
 *  | Bus      | [15: 8] |
 *  | Device   | [ 7: 3] |
 *  | Function | [ 2: 0] |
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] bdfid a pointer to uint64_t to which the device bdfid value
 *  will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_pci_id_get(uint32_t dv_ind, uint64_t *bdfid);

/**
 *  @brief Get the NUMA node associated with a device
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p
 *  numa_node, this function will retrieve the NUMA node value associated
 *  with device @p dv_ind and store the value at location pointed to by
 *  @p numa_node.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] numa_node pointer to location where NUMA node value will
 *  be written.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_topo_numa_affinity_get(uint32_t dv_ind, uint32_t *numa_node);

/**
 *  @brief Get PCIe traffic information
 *
 *  @details Give a device index @p dv_ind and pointers to a uint64_t's, @p
 *  sent, @p received and @p max_pkt_sz, this function will write the number
 *  of bytes sent and received in 1 second to @p sent and @p received,
 *  respectively. The maximum possible packet size will be written to
 *  @p max_pkt_sz.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] sent a pointer to uint64_t to which the number of bytes sent
 *  will be written in 1 second. If pointer is NULL, it will be ignored.
 *
 *  @param[inout] received a pointer to uint64_t to which the number of bytes
 *  received will be written. If pointer is NULL, it will be ignored.
 *
 *  @param[inout] max_pkt_sz a pointer to uint64_t to which the maximum packet
 *  size will be written. If pointer is NULL, it will be ignored.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 */
rsmi_status_t rsmi_dev_pci_throughput_get(uint32_t dv_ind, uint64_t *sent,
                                    uint64_t *received, uint64_t *max_pkt_sz);

/**
 *  @brief Get PCIe replay counter
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint64_t @p
 *  counter, this function will write the sum of the number of NAK's received
 *  by the GPU and the NAK's generated by the GPU to memory pointed to by @p
 *  counter.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] counter a pointer to uint64_t to which the sum of the NAK's
 *  received and generated by the GPU is written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_pci_replay_counter_get(uint32_t dv_ind,
                                                           uint64_t *counter);

/** @} */  // end of PCIeQuer
/*****************************************************************************/
/** @defgroup PCIeCont PCIe Control
 *  These functions provide some control over PCIe.
 *  @{
 */

/**
 *  @brief Control the set of allowed PCIe bandwidths that can be used.
 *
 *  @details Given a device index @p dv_ind and a 64 bit bitmask @p bw_bitmask,
 *  this function will limit the set of allowable bandwidths. If a bit in @p
 *  bw_bitmask has a value of 1, then the frequency (as ordered in an
 *  ::rsmi_frequencies_t returned by ::rsmi_dev_gpu_clk_freq_get()) corresponding
 *  to that bit index will be allowed.
 *
 *  This function will change the performance level to
 *  ::RSMI_DEV_PERF_LEVEL_MANUAL in order to modify the set of allowable
 *  band_widths. Caller will need to set to ::RSMI_DEV_PERF_LEVEL_AUTO in order
 *  to get back to default state.
 *
 *  All bits with indices greater than or equal to the value of the
 *  ::rsmi_frequencies_t::num_supported field of ::rsmi_pcie_bandwidth_t will be
 *  ignored.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] bw_bitmask A bitmask indicating the indices of the
 *  bandwidths that are to be enabled (1) and disabled (0). Only the lowest
 *  ::rsmi_frequencies_t::num_supported (of ::rsmi_pcie_bandwidth_t) bits of
 *  this mask are relevant.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t rsmi_dev_pci_bandwidth_set(uint32_t dv_ind, uint64_t bw_bitmask);

/** @} */  // end of PCIeCont

/*****************************************************************************/
/** @defgroup PowerQuer Power Queries
 *  These functions provide information about power usage.
 *  @{
 */
/**
 *  @brief Get the average power consumption of the device with provided
 *  device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint64_t
 *  @p power, this function will write the current average power consumption
 *  (in microwatts) to the uint64_t pointed to by @p power.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] power a pointer to uint64_t to which the average power
 *  consumption will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t
rsmi_dev_power_ave_get(uint32_t dv_ind, uint32_t sensor_ind, uint64_t *power);

/**
 *  @brief Get the cap on power which, when reached, causes the system to take
 *  action to reduce power.
 *
 *  @details When power use rises above the value @p power, the system will
 *  take action to reduce power use. The power level returned through
 *  @p power will be in microWatts.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] cap a pointer to a uint64_t that indicates the power cap,
 *  in microwatts
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t
rsmi_dev_power_cap_get(uint32_t dv_ind, uint32_t sensor_ind, uint64_t *cap);

/**
 *  @brief Get the range of valid values for the power cap
 *
 *  @details This function will return the maximum possible valid power cap
 *  @p max and the minimum possible valid power cap @p min
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] max a pointer to a uint64_t that indicates the maximum
 *  possible power cap, in microwatts
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[inout] min a pointer to a uint64_t that indicates the minimum
 *  possible power cap, in microwatts
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_power_cap_range_get(uint32_t dv_ind, uint32_t sensor_ind,
                                                uint64_t *max, uint64_t *min);

/** @} */  // end of PowerQuer

/*****************************************************************************/
/** @defgroup PowerCont Power Control
 *  These functions provide ways to control power usage.
 *  @{
 */
/**
 *  @brief Set the power cap value
 *
 *  @details This function will set the power cap to the provided value @p cap.
 *  @p cap must be between the minimum and maximum power cap values set by the
 *  system, which can be obtained from ::rsmi_dev_power_cap_range_get.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[in] cap a uint64_t that indicates the desired power cap, in
 *  microwatts
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_dev_power_cap_set(uint32_t dv_ind, uint32_t sensor_ind, uint64_t cap);

/**
 *  @brief Set the power profile
 *
 *  @details Given a device index @p dv_ind and a @p profile, this function will
 *  attempt to set the current profile to the provided profile. The provided
 *  profile must be one of the currently supported profiles, as indicated by a
 *  call to ::rsmi_dev_power_profile_presets_get()
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] reserved Not currently used. Set to 0.
 *
 *  @param[in] profile a ::rsmi_power_profile_preset_masks_t that hold the mask
 *  of the desired new power profile
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_dev_power_profile_set(uint32_t dv_ind, uint32_t reserved,
                                   rsmi_power_profile_preset_masks_t profile);
/** @} */  // end of PowerCont
/*****************************************************************************/



/*****************************************************************************/
/** @defgroup MemQuer Memory Queries
 *  These functions provide information about memory systems.
 *  @{
 */

/**
 *  @brief Get the total amount of memory that exists
 *
 *  @details Given a device index @p dv_ind, a type of memory @p mem_type, and
 *  a pointer to a uint64_t @p total, this function will write the total amount
 *  of @p mem_type memory that exists to the location pointed to by @p total.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] mem_type The type of memory for which the total amount will be
 *  found
 *
 *  @param[inout] total a pointer to uint64_t to which the total amount of
 *  memory will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_memory_total_get(uint32_t dv_ind, rsmi_memory_type_t mem_type,
                                                             uint64_t *total);

/**
 *  @brief Get the current memory usage
 *
 *  @details Given a device index @p dv_ind, a type of memory @p mem_type, and
 *  a pointer to a uint64_t @p usage, this function will write the amount of
 *  @p mem_type memory that that is currently being used to the location
 *  pointed to by @p total.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] mem_type The type of memory for which the amount being used will
 *  be found
 *
 *  @param[inout] used a pointer to uint64_t to which the amount of memory
 *  currently being used will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_memory_usage_get(uint32_t dv_ind, rsmi_memory_type_t mem_type,
                                                              uint64_t *used);

/**
 *  @brief Get percentage of time any device memory is being used
 *
 *  @details Given a device index @p dv_ind, this function returns the
 *  percentage of time that any device memory is being used for the specified
 *  device.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] busy_percent a pointer to the uint32_t to which the busy
 *  percent will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_memory_busy_percent_get(uint32_t dv_ind, uint32_t *busy_percent);

/**
 *  @brief Get information about reserved ("retired") memory pages
 *
 *  @details Given a device index @p dv_ind, this function returns retired page
 *  information @p records corresponding to the device with the provided device
 *  index @p dv_ind. The number of retired page records is returned through @p
 *  num_pages. @p records may be NULL on input. In this case, the number of
 *  records available for retrieval will be returned through @p num_pages.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] num_pages a pointer to a uint32. As input, the value passed
 *  through this parameter is the number of ::rsmi_retired_page_record_t's that
 *  may be safely written to the memory pointed to by @p records. This is the
 *  limit on how many records will be written to @p records. On return, @p
 *  num_pages will contain the number of records written to @p records, or the
 *  number of records that could have been written if enough memory had been
 *  provided.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[inout] records A pointer to a block of memory to which the
 *  ::rsmi_retired_page_record_t values will be written. This value may be NULL.
 *  In this case, this function can be used to query how many records are
 *  available to read.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if more records were available
 *  than allowed by the provided, allocated memory.
 */
rsmi_status_t
rsmi_dev_memory_reserved_pages_get(uint32_t dv_ind, uint32_t *num_pages,
                                         rsmi_retired_page_record_t *records);
/** @} */  // end of MemQuer

/** @defgroup PhysQuer Physical State Queries
 *  These functions provide information about the physical characteristics of
 *  the device.
 *  @{
 */
/**
 *  @brief Get the fan speed in RPMs of the device with the specified device
 *  index and 0-based sensor index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t
 *  @p speed, this function will write the current fan speed in RPMs to the
 *  uint32_t pointed to by @p speed
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] speed a pointer to uint32_t to which the speed will be
 *  written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_fan_rpms_get(uint32_t dv_ind, uint32_t sensor_ind,
                                                              int64_t *speed);

/**
 *  @brief Get the fan speed for the specified device as a value relative to
 *  ::RSMI_MAX_FAN_SPEED
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t
 *  @p speed, this function will write the current fan speed (a value
 *  between 0 and the maximum fan speed, ::RSMI_MAX_FAN_SPEED) to the uint32_t
 *  pointed to by @p speed
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] speed a pointer to uint32_t to which the speed will be
 *  written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_fan_speed_get(uint32_t dv_ind,
                                        uint32_t sensor_ind, int64_t *speed);

/**
 *  @brief Get the max. fan speed of the device with provided device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t
 *  @p max_speed, this function will write the maximum fan speed possible to
 *  the uint32_t pointed to by @p max_speed
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] max_speed a pointer to uint32_t to which the maximum speed
 *  will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_fan_speed_max_get(uint32_t dv_ind,
                                    uint32_t sensor_ind, uint64_t *max_speed);

/**
 *  @brief Get the temperature metric value for the specified metric, from the
 *  specified temperature sensor on the specified device.
 *
 *  @details Given a device index @p dv_ind, a sensor type @p sensor_type, a
 *  ::rsmi_temperature_metric_t @p metric and a pointer to an int64_t @p
 *  temperature, this function will write the value of the metric indicated by
 *  @p metric and @p sensor_type to the memory location @p temperature.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_type part of device from which temperature should be
 *  obtained. This should come from the enum ::rsmi_temperature_type_t
 *
 *  @param[in] metric enum indicated which temperature value should be
 *  retrieved
 *
 *  @param[inout] temperature a pointer to int64_t to which the temperature
 *  will be written, in millidegrees Celcius.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_temp_metric_get(uint32_t dv_ind, uint32_t sensor_type,
                      rsmi_temperature_metric_t metric, int64_t *temperature);
/** @} */  // end of PhysQuer

/*****************************************************************************/
/** @defgroup PhysCont Physical State Control
 *  These functions provide control over the physical state of a device.
 *  @{
 */
/**
 *  @brief Reset the fan to automatic driver control
 *
 *  @details This function returns control of the fan to the system
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments

 */
rsmi_status_t rsmi_dev_fan_reset(uint32_t dv_ind, uint32_t sensor_ind);

/**
 *  @brief Set the fan speed for the specified device with the provided speed,
 *  in RPMs.
 *
 *  @details Given a device index @p dv_ind and a integer value indicating
 *  speed @p speed, this function will attempt to set the fan speed to @p speed.
 *  An error will be returned if the specified speed is outside the allowable
 *  range for the device. The maximum value is 255 and the minimum is 0.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[in] speed the speed to which the function will attempt to set the fan
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t rsmi_dev_fan_speed_set(uint32_t dv_ind, uint32_t sensor_ind,
                                                              uint64_t speed);

/** @} */  // end of PhysCont
/*****************************************************************************/
/** @defgroup PerfQuer Clock, Power and Performance Queries
 *  These functions provide information about clock frequencies and
 *  performance.
 *  @{
 */

/**
 *  @brief Get percentage of time device is busy doing any processing
 *
 *  @details Given a device index @p dv_ind, this function returns the
 *  percentage of time that the specified device is busy. The device is
 *  considered busy if any one or more of its sub-blocks are working, and idle
 *  if none of the sub-blocks are working.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] busy_percent a pointer to the uint32_t to which the busy
 *  percent will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_busy_percent_get(uint32_t dv_ind, uint32_t *busy_percent);

/**
 *  @brief Get the performance level of the device with provided
 *  device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p
 *  perf, this function will write the ::rsmi_dev_perf_level_t to the uint32_t
 *  pointed to by @p perf
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] perf a pointer to ::rsmi_dev_perf_level_t to which the
 *  performance level will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_perf_level_get(uint32_t dv_ind,
                                                 rsmi_dev_perf_level_t *perf);

/**
 *  @brief Get the overdrive percent associated with the device with provided
 *  device index.
 *
 *  @details Given a device index @p dv_ind and a pointer to a uint32_t @p od,
 *  this function will write the overdrive percentage to the uint32_t pointed
 *  to by @p od
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] od a pointer to uint32_t to which the overdrive percentage
 *  will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_overdrive_level_get(uint32_t dv_ind, uint32_t *od);

/**
 *  @brief Get the list of possible system clock speeds of device for a
 *  specified clock type.
 *
 *  @details Given a device index @p dv_ind, a clock type @p clk_type, and a
 *  pointer to a to an ::rsmi_frequencies_t structure @p f, this function will
 *  fill in @p f with the possible clock speeds, and indication of the current
 *  clock speed selection.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] clk_type the type of clock for which the frequency is desired
 *
 *  @param[inout] f a pointer to a caller provided ::rsmi_frequencies_t structure
 *  to which the frequency information will be written. Frequency values are in
 *  Hz.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_gpu_clk_freq_get(uint32_t dv_ind,
                             rsmi_clk_type_t clk_type, rsmi_frequencies_t *f);

/**
 *  @brief This function retrieves the voltage/frequency curve information
 *
 *  @details Given a device index @p dv_ind and a pointer to a
 *  ::rsmi_od_volt_freq_data_t structure @p odv, this function will populate @p
 *  odv. See ::rsmi_od_volt_freq_data_t for more details.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] odv a pointer to an ::rsmi_od_volt_freq_data_t structure
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_od_volt_info_get(uint32_t dv_ind,
                                               rsmi_od_volt_freq_data_t *odv);

/**
 *  @brief This function will retrieve the current valid regions in the
 *  frequency/voltage space.
 *
 *  @details Given a device index @p dv_ind, a pointer to an unsigned integer
 *  @p num_regions and a buffer of ::rsmi_freq_volt_region_t structures, @p
 *  buffer, this function will populate @p buffer with the current
 *  frequency-volt space regions. The caller should assign @p buffer to memory
 *  that can be written to by this function. The caller should also
 *  indicate the number of ::rsmi_freq_volt_region_t structures that can safely
 *  be written to @p buffer in @p num_regions.
 *
 *  The number of regions to expect this function provide (@p num_regions) can
 *  be obtained by calling ::rsmi_dev_od_volt_info_get().
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] num_regions As input, this is the number of
 *  ::rsmi_freq_volt_region_t structures that can be written to @p buffer. As
 *  output, this is the number of ::rsmi_freq_volt_region_t structures that were
 *  actually written.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[inout] buffer a caller provided buffer to which
 *  ::rsmi_freq_volt_region_t structures will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_od_volt_curve_regions_get(uint32_t dv_ind,
                      uint32_t *num_regions, rsmi_freq_volt_region_t *buffer);

/**
 *  @brief Get the list of available preset power profiles and an indication of
 *  which profile is currently active.
 *
 *  @details Given a device index @p dv_ind and a pointer to a
 *  ::rsmi_power_profile_status_t @p status, this function will set the bits of
 *  the ::rsmi_power_profile_status_t.available_profiles bit field of @p status to
 *  1 if the profile corresponding to the respective
 *  ::rsmi_power_profile_preset_masks_t profiles are enabled. For example, if both
 *  the VIDEO and VR power profiles are available selections, then
 *  ::RSMI_PWR_PROF_PRST_VIDEO_MASK AND'ed with
 *  ::rsmi_power_profile_status_t.available_profiles will be non-zero as will
 *  ::RSMI_PWR_PROF_PRST_VR_MASK AND'ed with
 *  ::rsmi_power_profile_status_t.available_profiles. Additionally,
 *  ::rsmi_power_profile_status_t.current will be set to the
 *  ::rsmi_power_profile_preset_masks_t of the profile that is currently active.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] sensor_ind a 0-based sensor index. Normally, this will be 0.
 *  If a device has more than one sensor, it could be greater than 0.
 *
 *  @param[inout] status a pointer to ::rsmi_power_profile_status_t that will be
 *  populated by a call to this function
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_power_profile_presets_get(uint32_t dv_ind, uint32_t sensor_ind,
                                         rsmi_power_profile_status_t *status);

/** @} */  // end of PerfQuer
/*****************************************************************************/

/** @defgroup PerfCont Clock, Power and Performance Control
 *  These functions provide control over clock frequencies, power and
 *  performance.
 *  @{
 */
/**
 *  @brief Set the PowerPlay performance level associated with the device with
 *  provided device index with the provided value.
 *
 *  @details Given a device index @p dv_ind and an ::rsmi_dev_perf_level_t @p
 *  perf_level, this function will set the PowerPlay performance level for the
 *  device to the value @p perf_lvl.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] perf_lvl the value to which the performance level should be set
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_dev_perf_level_set(int32_t dv_ind, rsmi_dev_perf_level_t perf_lvl);

/**
 *  @brief Set the overdrive percent associated with the device with provided
 *  device index with the provided value. See details for WARNING.
 *
 *  @details Given a device index @p dv_ind and an overdrive level @p od,
 *  this function will set the overdrive level for the device to the value
 *  @p od. The overdrive level is an integer value between 0 and 20, inclusive,
 *  which represents the overdrive percentage; e.g., a value of 5 specifies
 *  an overclocking of 5%.
 *
 *  The overdrive level is specific to the gpu system clock.
 *
 *  The overdrive level is the percentage above the maximum Performance Level
 *  to which overclocking will be limited. The overclocking percentage does
 *  not apply to clock speeds other than the maximum. This percentage is
 *  limited to 20%.
 *
 *   ******WARNING******
 *  Operating your AMD GPU outside of official AMD specifications or outside of
 *  factory settings, including but not limited to the conducting of
 *  overclocking (including use of this overclocking software, even if such
 *  software has been directly or indirectly provided by AMD or otherwise
 *  affiliated in any way with AMD), may cause damage to your AMD GPU, system
 *  components and/or result in system failure, as well as cause other problems.
 *  DAMAGES CAUSED BY USE OF YOUR AMD GPU OUTSIDE OF OFFICIAL AMD SPECIFICATIONS
 *  OR OUTSIDE OF FACTORY SETTINGS ARE NOT COVERED UNDER ANY AMD PRODUCT
 *  WARRANTY AND MAY NOT BE COVERED BY YOUR BOARD OR SYSTEM MANUFACTURER'S
 *  WARRANTY. Please use this utility with caution.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] od the value to which the overdrive level should be set
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t rsmi_dev_overdrive_level_set(int32_t dv_ind, uint32_t od);

/**
 * @brief Control the set of allowed frequencies that can be used for the
 * specified clock.
 *
 * @details Given a device index @p dv_ind, a clock type @p clk_type, and a
 * 64 bit bitmask @p freq_bitmask, this function will limit the set of
 * allowable frequencies. If a bit in @p freq_bitmask has a value of 1, then
 * the frequency (as ordered in an ::rsmi_frequencies_t returned by
 * rsmi_dev_gpu_clk_freq_get()) corresponding to that bit index will be
 * allowed.
 *
 * This function will change the performance level to
 * ::RSMI_DEV_PERF_LEVEL_MANUAL in order to modify the set of allowable
 * frequencies. Caller will need to set to ::RSMI_DEV_PERF_LEVEL_AUTO in order
 * to get back to default state.
 *
 * All bits with indices greater than or equal to
 * ::rsmi_frequencies_t::num_supported will be ignored.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] clk_type the type of clock for which the set of frequencies
 *  will be modified
 *
 *  @param[in] freq_bitmask A bitmask indicating the indices of the
 *  frequencies that are to be enabled (1) and disabled (0). Only the lowest
 *  ::rsmi_frequencies_t.num_supported bits of this mask are relevant.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t rsmi_dev_gpu_clk_freq_set(uint32_t dv_ind,
                             rsmi_clk_type_t clk_type, uint64_t freq_bitmask);

/** @} */  // end of PerfCont

/*****************************************************************************/
/** @defgroup VersQuer Version Queries
 *  These functions provide version information about various subsystems.
 *  @{
 */

/**
 * @brief Get the build version information for the currently running build of
 * RSMI.
 *
 * @details  Get the major, minor, patch and build string for RSMI build
 * currently in use through @p version
 *
 * @param[inout] version A pointer to an ::rsmi_version_t structure that will
 * be updated with the version information upon return.
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *
 */
rsmi_status_t
rsmi_version_get(rsmi_version_t *version);

/**
 *  @brief Get the driver version string for the current system.
 *
 *  @details Given a software component @p component, a pointer to a char
 *  buffer, @p ver_str, this function will write the driver version string
 *  (up to @p len characters) for the current system to @p ver_str. The caller
 *  must ensure that it is safe to write at least @p len characters to @p
 *  ver_str.
 *
 *  @param[in] component The component for which the version string is being
 *  requested
 *
 *  @param[inout] ver_str A pointer to a buffer of char's to which the version
 *  of @p component will be written
 *
 *  @param[in] len the length of the caller provided buffer @p name.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if @p len bytes is not
 *  large enough to hold the entire name. In this case, only @p len bytes will
 *  be written.
 */
rsmi_status_t
rsmi_version_str_get(rsmi_sw_component_t component, char *ver_str,
                                                                uint32_t len);

/**
 *  @brief Get the VBIOS identifer string
 *
 *  @details Given a device ID @p dv_ind, and a pointer to a char buffer,
 *  @p vbios, this function will write the VBIOS string (up to @p len
 *  characters) for device @p dv_ind to @p vbios. The caller must ensure that
 *  it is safe to write at least @p len characters to @p vbios.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] vbios A pointer to a buffer of char's to which the VBIOS name
 *  will be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @param[in] len The number of char's pointed to by @p vbios which can safely
 *  be written to by this function.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_vbios_version_get(uint32_t dv_ind, char *vbios, uint32_t len);

/**
 *  @brief Get the firmware versions for a device
 *
 *  @details Given a device ID @p dv_ind, and a pointer to a uint64_t,
 *  @p fw_version, this function will write the FW Versions as a string (up to @p len
 *  characters) for device @p dv_ind to @p vbios. The caller must ensure that
 *  it is safe to write at least @p len characters to @p vbios.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] block The firmware block for which the version is being requested
 *
 *  @param[inout] fw_version The version for the firmware block
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_firmware_version_get(uint32_t dv_ind, rsmi_fw_block_t block,
                                                        uint64_t *fw_version);

/** @} */  // end of VersQuer

/*****************************************************************************/
/** @defgroup ErrQuer Error Queries
 *  These functions provide error information about RSMI calls as well as
 *  device errors.
 *  @{
 */

/**
 *  @brief Retrieve the error counts for a GPU block
 *
 *  @details Given a device index @p dv_ind, an ::rsmi_gpu_block_t @p block and a
 *  pointer to an ::rsmi_error_count_t @p ec, this function will write the error
 *  count values for the GPU block indicated by @p block to memory pointed to by
 *  @p ec.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] block The block for which error counts should be retrieved
 *
 *  @param[inout] ec A pointer to an ::rsmi_error_count_t to which the error
 *  counts should be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_ecc_count_get(uint32_t dv_ind,
                              rsmi_gpu_block_t block, rsmi_error_count_t *ec);

/**
 *  @brief Retrieve the enabled ECC bit-mask
 *
 *  @details Given a device index @p dv_ind, and a pointer to a uint64_t @p
 *  enabled_mask, this function will write bits to memory pointed to by
 *  @p enabled_blocks. Upon a successful call, @p enabled_blocks can then be
 *  AND'd with elements of the ::rsmi_gpu_block_t ennumeration to determine if
 *  the corresponding block has ECC enabled. Note that whether a block has ECC
 *  enabled or not in the device is independent of whether there is kernel
 *  support for error counting for that block. Although a block may be enabled,
 *  but there may not be kernel support for reading error counters for that
 *  block.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] enabled_blocks A pointer to a uint64_t to which the enabled
 *  blocks bits will be written.
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 */
rsmi_status_t rsmi_dev_ecc_enabled_get(uint32_t dv_ind,
                                                    uint64_t *enabled_blocks);

/**
 *  @brief Retrieve the ECC status for a GPU block
 *
 *  @details Given a device index @p dv_ind, an ::rsmi_gpu_block_t @p block and
 *  a pointer to an ::rsmi_ras_err_state_t @p state, this function will write
 *  the current state for the GPU block indicated by @p block to memory pointed
 *  to by @p state.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] block The block for which error counts should be retrieved
 *
 *  @param[inout] state A pointer to an ::rsmi_ras_err_state_t to which the
 *  ECC state should be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t rsmi_dev_ecc_status_get(uint32_t dv_ind, rsmi_gpu_block_t block,
                                                  rsmi_ras_err_state_t *state);
/**
 *  @brief Get a description of a provided RSMI error status
 *
 *  @details Set the provided pointer to a const char *, @p status_string, to
 *  a string containing a description of the provided error code @p status.
 *
 *  @param[in] status The error status for which a description is desired
 *
 *  @param[inout] status_string A pointer to a const char * which will be made
 *  to point to a description of the provided error code
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *
 */
rsmi_status_t
rsmi_status_string(rsmi_status_t status, const char **status_string);

/** @} */  // end of ErrQuer

/*****************************************************************************/
/** @defgroup PerfCntr Performance Counter Functions
 *  These functions are used to configure, query and control performance
 *  counting.
 *  @{
 */

/**
 *  @brief Tell if an event group is supported by a given device
 *
 *  @details Given a device index @p dv_ind and an event group specifier @p
 *  group, tell if @p group type events are supported by the device associated
 *  with @p dv_ind
 *
 *  @param[in] dv_ind device index of device being queried
 *
 *  @param[in] group ::rsmi_event_group_t identifier of group for which support
 *  is being queried
 *
 *  @retval ::RSMI_STATUS_SUCCESS if the device associatee with @p dv_ind
 *  support counting events of the type indicated by @p group.
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  group
 *
 */
rsmi_status_t
rsmi_dev_counter_group_supported(uint32_t dv_ind, rsmi_event_group_t group);

/**
 *  @brief Create a performance counter object
 *
 *  @details Create a performance counter object of type @p type for the device
 *  with a device index of @p dv_ind, and write a handle to the object to the
 *  memory location pointed to by @p evnt_handle. @p evnt_handle can be used
 *  with other performance event operations. The handle should be deallocated
 *  with ::rsmi_dev_counter_destroy() when no longer needed.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] type the ::rsmi_event_type_t of performance event to create
 *
 *  @param[inout] evnt_handle A pointer to a ::rsmi_event_handle_t which will be
 *  associated with a newly allocated counter
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_OUT_OF_RESOURCES unable to allocate memory for counter
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_dev_counter_create(uint32_t dv_ind, rsmi_event_type_t type,
                                            rsmi_event_handle_t *evnt_handle);

/**
 *  @brief Deallocate a performance counter object
 *
 *  @details Deallocate the performance counter object with the provided
 *  ::rsmi_event_handle_t @p evnt_handle
 *
 *  @param[in] evnt_handle handle to event object to be deallocated
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_dev_counter_destroy(rsmi_event_handle_t evnt_handle);

/**
 *  @brief Issue performance counter control commands
 *
 *  @details Issue a command @p cmd on the event counter associated with the
 *  provided handle @p evt_handle.
 *
 *  @param[in] evt_handle an event handle
 *
 *  @param[in] cmd The event counter command to be issued
 *
 *  @param[inout] cmd_args Currently not used. Should be set to NULL.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_counter_control(rsmi_event_handle_t evt_handle,
                                  rsmi_counter_command_t cmd, void *cmd_args);

/**
 *  @brief Read the current value of a performance counter
 *
 *  @details Read the current counter value of the counter associated with the
 *  provided handle @p evt_handle and write the value to the location pointed
 *  to by @p value.
 *
 *  @param[in] evt_handle an event handle
 *
 *  @param[inout] value pointer to memory of size of ::rsmi_counter_value_t to
 *  which the counter value will be written
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_PERMISSION function requires root access
 *
 */
rsmi_status_t
rsmi_counter_read(rsmi_event_handle_t evt_handle,
                                                 rsmi_counter_value_t *value);

/**
 *  @brief Get the number of currently available counters
 *
 *  @details Given a device index @p dv_ind, a performance event group @p grp,
 *  and a pointer to a uint32_t @p available, this function will write the
 *  number of @p grp type counters that are available on the device with index
 *  @p dv_ind to the memory that @p available points to.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[in] grp an event device group
 *
 *  @param[inout] available A pointer to a uint32_t to which the number of
 *  available counters will be written
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_counter_available_counters_get(uint32_t dv_ind,
                                 rsmi_event_group_t grp, uint32_t *available);
/** @} */  // end of PerfCntr

/*****************************************************************************/
/** @defgroup SysInfo System Information Functions
 *  These functions are used to configure, query and control performance
 *  counting.
 *  @{
 */

/**
 *  @brief Get process information about processes currently using GPU
 *
 *  @details Given a non-NULL pointer to an array @p procs of
 *  ::rsmi_process_info_t's, of length *@p num_items, this function will write
 *  up to *@p num_items instances of ::rsmi_process_info_t to the memory pointed
 *  to by @p procs. These instances contain information about each process
 *  utilizing a GPU. If @p procs is not NULL, @p num_items will be updated with
 *  the number of processes actually written. If @p procs is NULL, @p num_items
 *  will be updated with the number of processes for which there is current
 *  process information. Calling this function with @p procs being NULL is a way
 *  to determine how much memory should be allocated for when @p procs is not
 *  NULL.
 *
 *  @param[inout] procs a pointer to memory provided by the caller to which
 *  process information will be written. This may be NULL in which case only @p
 *  num_items will be updated with the number of processes found.
 *
 *  @param[inout] num_items A pointer to a uint32_t, which on input, should
 *  contain the amount of memory in ::rsmi_process_info_t's which have been
 *  provided by the @p procs argument. On output, if @p procs is non-NULL, this
 *  will be updated with the number ::rsmi_process_info_t structs actually
 *  written. If @p procs is NULL, this argument will be updated with the number
 *  processes for which there is information.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if there were more
 *  processes for which information was available, but not enough space was
 *  provided as indicated by @p procs and @p num_items, on input.
 */
rsmi_status_t
rsmi_compute_process_info_get(rsmi_process_info_t *procs, uint32_t *num_items);

/**
 *  @brief Get process information about a specific process
 *
 *  @details Given a pointer to an ::rsmi_process_info_t @p proc and a process
 *  id
 *  @p pid, this function will write the process information for @p pid, if
 *  available, to the memory pointed to by @p proc.
 *
 *  @param[in] pid The process ID for which process information is being
 *  requested
 *
 *  @param[inout] proc a pointer to a ::rsmi_process_info_t to which
 *  process information for @p pid will be written if it is found.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_NOT_FOUND is returned if there was no process
 *  information
 *  found for the provided @p pid
 *
 */
rsmi_status_t
rsmi_compute_process_info_by_pid_get(uint32_t pid, rsmi_process_info_t *proc);

/**
 *  @brief Get the device indices currently being used by a process
 *
 *  @details Given a process id @p pid, a non-NULL pointer to an array of
 *  uint32_t's @p dv_indices of length *@p num_devices, this function will
 *  write up to @p num_devices device indices to the memory pointed to by
 *  @p dv_indices. If @p dv_indices is not NULL, @p num_devices will be
 *  updated with the number of gpu's currently being used by process @p pid.
 *  If @p dv_indices is NULL, @p dv_indices will be updated with the number of
 *  gpus currently being used by @p pid. Calling this function with @p
 *  dv_indices being NULL is a way to determine how much memory is required
 *  for when @p dv_indices is not NULL.
 *
 *  @param[in] pid The process id of the process for which the number of gpus
 *  currently being used is requested
 *
 *  @param[inout] dv_indices a pointer to memory provided by the caller to
 *  which indices of devices currently being used by the process will be
 *  written. This may be NULL in which case only @p num_devices will be
 *  updated with the number of devices being used.
 *
 *  @param[inout] num_devices A pointer to a uint32_t, which on input, should
 *  contain the amount of memory in uint32_t's which have been provided by the
 *  @p dv_indices argument. On output, if @p dv_indices is non-NULL, this will
 *  be updated with the number uint32_t's actually written. If @p dv_indices is
 *  NULL, this argument will be updated with the number devices being used.
 *
 *  @retval ::RSMI_STATUS_SUCCESS is returned upon successful call
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *  @retval ::RSMI_STATUS_INSUFFICIENT_SIZE is returned if there were more
 *  gpu indices that could have been written, but not enough space was
 *  provided as indicated by @p dv_indices and @p num_devices, on input.
 *
 */
rsmi_status_t
rsmi_compute_process_gpus_get(uint32_t pid, uint32_t *dv_indices,
                                                       uint32_t *num_devices);

/** @} */  // end of SysInfo

/*****************************************************************************/
/** @defgroup XGMIInfo XGMI Functions
 *  These functions are used to configure, query and control XGMI.
 *  @{
 */

/**
 *  @brief Retrieve the XGMI error status for a device
 *
 *  @details Given a device index @p dv_ind, and a pointer to an
 *  ::rsmi_xgmi_status_t @p status, this function will write the current XGMI
 *  error state ::rsmi_xgmi_status_t for the device @p dv_ind to the memory
 *  pointed to by @p status.
 *
 *  @param[in] dv_ind a device index
 *
 *  @param[inout] status A pointer to an ::rsmi_xgmi_status_t to which the
 *  XGMI error state should be written
 *  If this parameter is nullptr, this function will return
 *  ::RSMI_STATUS_INVALID_ARGS if the function is supported with the provided,
 *  arguments and ::RSMI_STATUS_NOT_SUPPORTED if it is not supported with the
 *  provided arguments.
 *
 *  @retval ::RSMI_STATUS_SUCCESS call was successful
 *  @retval ::RSMI_STATUS_NOT_SUPPORTED installed software or hardware does not
 *  support this function with the given arguments
 *  @retval ::RSMI_STATUS_INVALID_ARGS the provided arguments are not valid
 *
 */
rsmi_status_t
rsmi_dev_xgmi_error_status(uint32_t dv_ind, rsmi_xgmi_status_t *status);

/**
 * @brief Reset the XGMI error status for a device
 *
 * @details Given a device index @p dv_ind, this function will reset the
 * current XGMI error state ::rsmi_xgmi_status_t for the device @p dv_ind to
 * rsmi_xgmi_status_t::RSMI_XGMI_STATUS_NO_ERRORS
 *
 * @param[in] dv_ind a device index
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_dev_xgmi_error_reset(uint32_t dv_ind);

/** @} */  // end of SysInfo


/*****************************************************************************/
/** @defgroup APISupport Supported Functions
 *  API function support varies by both GPU type and the version of the
 *  installed ROCm stack.  The functions described in this section can be used
 *  to determine, up front, which functions are supported for a given device
 *  on a system. If such "up front" knowledge of support for a function is not
 *  needed, alternatively, one can call a device related function and check the
 *  return code.
 *
 *  Some functions have several variations ("variants") where some variants are
 *  supported and others are not. For example, on a given device,
 *  ::rsmi_dev_temp_metric_get may support some types of temperature metrics
 *  (e.g., ::RSMI_TEMP_CRITICAL_HYST), but not others
 *  (e.g., ::RSMI_TEMP_EMERGENCY).
 *
 *  In addition to a top level of variant support for a function, a function
 *  may have varying support for monitors/sensors. These are considered
 *  "sub-variants" in functions described in this section. Continuing the
 *  ::rsmi_dev_temp_metric_get example, if variant
 *  ::RSMI_TEMP_CRITICAL_HYST is supported, perhaps
 *  only the sub-variant sensors ::RSMI_TEMP_TYPE_EDGE
 *  and ::RSMI_TEMP_TYPE_EDGE are supported, but not
 *  ::RSMI_TEMP_TYPE_MEMORY.
 *
 *  In cases where a function takes in a sensor id parameter but does not have
 *  any "top level" variants, the functions in this section will indicate a
 *  default "variant", ::RSMI_DEFAULT_VARIANT, for the top level variant, and
 *  the various monitor support will be sub-variants of this.
 *
 *  The functions in this section use the "iterator" concept to list which
 *  functions are supported; to list which variants of the supported functions
 *  are supported; and finally which monitors/sensors are supported for a
 *  variant.
 *
 *  Here is example code that prints out all supported functions, their
 *  supported variants and sub-variants. Please see the related descriptions
 *  functions and RSMI types.
 *  \latexonly
 *  \pagebreak
 *  \endlatexonly
 *  \code{.cpp}
 *     rsmi_func_id_iter_handle_t iter_handle, var_iter, sub_var_iter;
 *     rsmi_func_id_value_t value;
 *     rsmi_status_t err;
 *
 *     for (uint32_t i = 0; i < <number of devices>; ++i) {
 *      std::cout << "Supported RSMI Functions:" << std::endl;
 *      std::cout << "\tVariants (Monitors)" << std::endl;
 *
 *      err = rsmi_dev_supported_func_iterator_open(i, &iter_handle);
 *
 *      while (1) {
 *        err = rsmi_func_iter_value_get(iter_handle, &value);
 *        std::cout << "Function Name: " << value.name << std::endl;
 *
 *        err = rsmi_dev_supported_variant_iterator_open(iter_handle, &var_iter);
 *        if (err != RSMI_STATUS_NO_DATA) {
 *          std::cout << "\tVariants/Monitors: ";
 *          while (1) {
 *            err = rsmi_func_iter_value_get(var_iter, &value);
 *            if (value.id == RSMI_DEFAULT_VARIANT) {
 *              std::cout << "Default Variant ";
 *            } else {
 *              std::cout << value.id;
 *            }
 *            std::cout << " (";
 *
 *            err =
 *              rsmi_dev_supported_variant_iterator_open(var_iter, &sub_var_iter);
 *            if (err != RSMI_STATUS_NO_DATA) {
 *
 *              while (1) {
 *                err = rsmi_func_iter_value_get(sub_var_iter, &value);
 *                std::cout << value.id << ", ";
 *
 *                err = rsmi_func_iter_next(sub_var_iter);
 *
 *                if (err == RSMI_STATUS_NO_DATA) {
 *                  break;
 *                }
 *              }
 *              err = rsmi_dev_supported_func_iterator_close(&sub_var_iter);
 *            }
 *
 *            std::cout << "), ";
 *
 *            err = rsmi_func_iter_next(var_iter);
 *
 *            if (err == RSMI_STATUS_NO_DATA) {
 *              break;
 *            }
 *          }
 *          std::cout << std::endl;
 *
 *          err = rsmi_dev_supported_func_iterator_close(&var_iter);
 *        }
 *
 *        err = rsmi_func_iter_next(iter_handle);
 *
 *        if (err == RSMI_STATUS_NO_DATA) {
 *          break;
 *        }
 *      }
 *      err = rsmi_dev_supported_func_iterator_close(&iter_handle);
 *    }
 * \endcode
 *
 *  @{
 */


/**
 * @brief Get a function name iterator of supported RSMI functions for a device
 *
 * @details Given a device index @p dv_ind, this function will write a function
 * iterator handle to the caller-provided memory pointed to by @p handle. This
 * handle can be used to iterate through all the supported functions.
 *
 * Note that although this function takes in @p dv_ind as an argument,
 * ::rsmi_dev_supported_func_iterator_open itself will not be among the
 * functions listed as supported. This is because
 * ::rsmi_dev_supported_func_iterator_open does not depend on hardware or
 * driver support and should always be supported.
 *
 * @param[in] dv_ind a device index of device for which support information is
 * requested
 *
 * @param[inout] handle A pointer to caller-provided memory to which the
 * function iterator will be written.
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_dev_supported_func_iterator_open(uint32_t dv_ind,
                                          rsmi_func_id_iter_handle_t *handle);

/**
 * @brief Get a variant iterator for a given handle
 *
 * @details Given a ::rsmi_func_id_iter_handle_t @p obj_h, this function will
 * write a function iterator handle to the caller-provided memory pointed to
 * by @p var_iter. This handle can be used to iterate through all the supported
 * variants of the provided handle. @p obj_h may be a handle to a function
 * object, as provided by a call to ::rsmi_dev_supported_func_iterator_open, or
 * it may be a variant itself (from a call to
 * ::rsmi_dev_supported_variant_iterator_open), it which case @p var_iter will
 * be an iterator of the sub-variants of @p obj_h (e.g., monitors).
 *
 * This call allocates a small amount of memory to @p var_iter. To free this memory
 * ::rsmi_dev_supported_func_iterator_close should be called on the returned
 * iterator handle @p var_iter when it is no longer needed.
 *
 * @param[in] obj_h an iterator handle for which the variants are being requested
 *
 * @param[inout] var_iter A pointer to caller-provided memory to which the
 * sub-variant iterator will be written.
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_dev_supported_variant_iterator_open(rsmi_func_id_iter_handle_t obj_h,
                                        rsmi_func_id_iter_handle_t *var_iter);

/**
 * @brief Advance a function identifer iterator
 *
 * @details Given a function id iterator handle (::rsmi_func_id_iter_handle_t)
 * @p handle, this function will increment the iterator to point to the next
 * identifier. After a successful call to this function, obtaining the value
 * of the iterator @p handle will provide the value of the next item in the
 * list of functions/variants.
 *
 * If there are no more items in the list, ::RSMI_STATUS_NO_DATA is returned.
 *
 * @param[in] handle A pointer to an iterator handle to be incremented
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 * @retval ::RSMI_STATUS_NO_DATA is returned when list of identifiers has been
 * exhausted
 *
 */
rsmi_status_t
rsmi_func_iter_next(rsmi_func_id_iter_handle_t handle);

/**
 * @brief Close a variant iterator handle
 *
 * @details Given a pointer to an ::rsmi_func_id_iter_handle_t @p handle, this
 * function will free the resources being used by the handle
 *
 * @param[in] handle A pointer to an iterator handle to be closed
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_dev_supported_func_iterator_close(rsmi_func_id_iter_handle_t *handle);

/**
 * @brief Get the value associated with a function/variant iterator
 *
 * @details Given an ::rsmi_func_id_iter_handle_t @p handle, this function
 * will write the identifier of the function/variant to the user provided
 * memory pointed to by @p value.
 *
 * @p value may point to a function name, a variant id, or a monitor/sensor
 * index, depending on what kind of iterator @p handle is
 *
 * @param[in] handle An iterator for which the value is being requested
 *
 * @param[inout] value A pointer to an ::rsmi_func_id_value_t provided by the
 * caller to which this function will write the value assocaited with @p handle
 *
 * @retval ::RSMI_STATUS_SUCCESS is returned upon successful call.
 *
 */
rsmi_status_t
rsmi_func_iter_value_get(rsmi_func_id_iter_handle_t handle,
                                                 rsmi_func_id_value_t *value);

/** @} */  // end of APISupport

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // INCLUDE_ROCM_SMI_ROCM_SMI_H_
