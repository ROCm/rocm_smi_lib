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

#include <assert.h>

#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <iostream>
#include <algorithm>

#include "rocm_smi/rocm_smi_main.h"
#include "rocm_smi/rocm_smi_monitor.h"
#include "rocm_smi/rocm_smi_utils.h"

namespace amd {
namespace smi {

struct MonitorNameEntry {
    MonitorTypes type;
    const char *name;
};


static const char *kMonTempFName = "temp#_input";
static const char *kMonFanSpeedFName = "pwm#";
static const char *kMonMaxFanSpeedFName = "pwm#_max";
static const char *kMonFanRPMsName = "fan#_input";
static const char *kMonFanControlEnableName = "pwm#_enable";
static const char *kMonNameFName = "name";
static const char *kMonPowerCapName = "power#_cap";
static const char *kMonPowerCapMaxName = "power#_cap_max";
static const char *kMonPowerCapMinName = "power#_cap_min";
static const char *kMonPowerAveName = "power#_average";
static const char *kMonTempMaxName = "temp#_max";
static const char *kMonTempMinName = "temp#_min";
static const char *kMonTempMaxHystName = "temp#_max_hyst";
static const char *kMonTempMinHystName = "temp#_min_hyst";
static const char *kMonTempCriticalName = "temp#_crit";
static const char *kMonTempCriticalHystName = "temp#_crit_hyst";
static const char *kMonTempEmergencyName = "temp#_emergency";
static const char *kMonTempEmergencyHystName = "temp#_emergency_hyst";
static const char *kMonTempCritMinName = "temp#_lcrit";
static const char *kMonTempCritMinHystName = "temp#_lcrit_hyst";
static const char *kMonTempOffsetName = "temp#_offset";
static const char *kMonTempLowestName = "temp#_lowest";
static const char *kMonTempHighestName = "temp#_highest";
static const char *kMonTempLabelName = "temp#_label";

static const char *kTempSensorTypeMemoryName = "mem";
static const char *kTempSensorTypeJunctionName = "junction";
static const char *kTempSensorTypeEdgeName = "edge";

static const std::map<std::string, rsmi_temperature_type_t>
                                                        kTempSensorNameMap = {
    {kTempSensorTypeMemoryName, RSMI_TEMP_TYPE_MEMORY},
    {kTempSensorTypeJunctionName, RSMI_TEMP_TYPE_JUNCTION},
    {kTempSensorTypeEdgeName, RSMI_TEMP_TYPE_EDGE},
};

static const std::map<MonitorTypes, const char *> kMonitorNameMap = {
    {kMonName, kMonNameFName},
    {kMonTemp, kMonTempFName},
    {kMonFanSpeed, kMonFanSpeedFName},
    {kMonFanCntrlEnable, kMonFanControlEnableName},
    {kMonMaxFanSpeed, kMonMaxFanSpeedFName},
    {kMonFanRPMs, kMonFanRPMsName},
    {kMonPowerCap, kMonPowerCapName},
    {kMonPowerCapMax, kMonPowerCapMaxName},
    {kMonPowerCapMin, kMonPowerCapMinName},
    {kMonPowerAve, kMonPowerAveName},
    {kMonTempMax, kMonTempMaxName},
    {kMonTempMin, kMonTempMinName},
    {kMonTempMaxHyst, kMonTempMaxHystName},
    {kMonTempMinHyst, kMonTempMinHystName},
    {kMonTempCritical, kMonTempCriticalName},
    {kMonTempCriticalHyst, kMonTempCriticalHystName},
    {kMonTempEmergency, kMonTempEmergencyName},
    {kMonTempEmergencyHyst, kMonTempEmergencyHystName},
    {kMonTempCritMin, kMonTempCritMinName},
    {kMonTempCritMinHyst, kMonTempCritMinHystName},
    {kMonTempOffset, kMonTempOffsetName},
    {kMonTempLowest, kMonTempLowestName},
    {kMonTempHighest, kMonTempHighestName},
    {kMonTempLabel, kMonTempLabelName},
};

Monitor::Monitor(std::string path, RocmSMI_env_vars const *e) :
                                                        path_(path), env_(e) {
}
Monitor::~Monitor(void) {
}

std::string
Monitor::MakeMonitorPath(MonitorTypes type, int32_t sensor_id) {
  std::string tempPath = path_;
  std::string fn = kMonitorNameMap.at(type);

  std::replace(fn.begin(), fn.end(), '#', static_cast<char>('0' + sensor_id));

  tempPath += "/";
  tempPath += fn;

  return tempPath;
}

int Monitor::writeMonitor(MonitorTypes type, uint32_t sensor_id,
                                                            std::string val) {
  std::string sysfs_path = MakeMonitorPath(type, sensor_id);

  DBG_FILE_ERROR(sysfs_path, &val)
  return WriteSysfsStr(sysfs_path, val);
}

// This string version should work for all valid monitor types
int Monitor::readMonitor(MonitorTypes type, uint32_t sensor_id,
                                                           std::string *val) {
  assert(val != nullptr);

  std::string temp_str;
  std::string sysfs_path = MakeMonitorPath(type, sensor_id);

  DBG_FILE_ERROR(sysfs_path, (std::string *)nullptr)
  return ReadSysfsStr(sysfs_path, val);
}

uint32_t
Monitor::setSensorLabelMap(void) {
  std::string type_str;
  int ret;

  if (temp_type_index_map_.size() > 0) {
    return 0;  // We've already filled in the map
  }
  auto add_temp_sensor_entry = [&](uint32_t file_index) {
    ret = readMonitor(kMonTempLabel, file_index, &type_str);
    if (ret) {
      return ret;
    }

    rsmi_temperature_type_t t_type = kTempSensorNameMap.at(type_str);
    temp_type_index_map_.insert({t_type, file_index});
    return 0;
  };

  for (uint32_t i = 1; i <= 3; ++i) {
    ret = add_temp_sensor_entry(i);
    if (ret) {
      return ret;
    }
  }
  return 0;
}

uint32_t
Monitor::getSensorIndex(rsmi_temperature_type_t type) {
  return temp_type_index_map_.at(type);
}


}  // namespace smi
}  // namespace amd
