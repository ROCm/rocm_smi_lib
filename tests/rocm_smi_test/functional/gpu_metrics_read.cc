/*
 * =============================================================================
 *   ROC Runtime Conformance Release License
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <stddef.h>

#include <iostream>
#include <string>
#include <map>

#include "gtest/gtest.h"
#include "rocm_smi/rocm_smi.h"
#include "rocm_smi/rocm_smi_utils.h"
#include "rocm_smi_test/functional/gpu_metrics_read.h"
#include "rocm_smi_test/test_common.h"


TestGpuMetricsRead::TestGpuMetricsRead() : TestBase() {
  set_title("RSMI GPU Metrics Read Test");
  set_description("The GPU Metrics tests verifies that "
                  "the gpu metrics info can be read properly.");
}

TestGpuMetricsRead::~TestGpuMetricsRead(void) {
}

void TestGpuMetricsRead::SetUp(void) {
  TestBase::SetUp();

  return;
}

void TestGpuMetricsRead::DisplayTestInfo(void) {
  TestBase::DisplayTestInfo();
}

void TestGpuMetricsRead::DisplayResults(void) const {
  TestBase::DisplayResults();
  return;
}

void TestGpuMetricsRead::Close() {
  // This will close handles opened within rsmitst utility calls and call
  // rsmi_shut_down(), so it should be done after other hsa cleanup
  TestBase::Close();
}


using GPUMetricResults_t = std::map<std::string, rsmi_status_t>;
GPUMetricResults_t MetricResults{};

template <typename T>
auto print_error_or_value(std::string title, std::string func_name, const T& metric) {
  auto str_values = title;
  const auto status_code = MetricResults.at(func_name);
  if (status_code == rsmi_status_t::RSMI_STATUS_SUCCESS) {
    if constexpr (std::is_array_v<T>) {
      auto idx = uint16_t(0);

      const auto num_elems = static_cast<uint16_t>(std::end(metric) - std::begin(metric));
      str_values += ("\n\t\t num of values: " + std::to_string(num_elems) + "\n");
      for (const auto& el : metric) {
        str_values += "\t\t  [" + std::to_string(idx) + "]: " + std::to_string(el) + "\n";
        ++idx;
      }
      return str_values;
    }
    else if constexpr ((std::is_same_v<T, std::uint16_t>) ||
                      (std::is_same_v<T, std::uint32_t>) ||
                      (std::is_same_v<T, std::uint64_t>)) {

      return str_values += std::to_string(metric);
    }
  }
  else {
    return str_values += ("\n\t\tStatus: [" + std::to_string(status_code) + "] " + "-> " + amd::smi::getRSMIStatusString(status_code));
  }
};


void TestGpuMetricsRead::Run(void) {
  rsmi_status_t err;

  TestBase::Run();
  if (setup_failed_) {
    std::cout << "** SetUp Failed for this test. Skipping.**" << std::endl;
    return;
  }

  for (uint32_t i = 0; i < num_monitor_devs(); ++i) {
    PrintDeviceHeader(i);

    IF_VERB(STANDARD) {
        std::cout << "\t**GPU METRICS: Using static struct (Backwards Compatibility):\n";
    }
    rsmi_gpu_metrics_t smu;
    err = rsmi_dev_gpu_metrics_info_get(i, &smu);
    if (err != RSMI_STATUS_SUCCESS) {
      if (err == RSMI_STATUS_NOT_SUPPORTED) {
        IF_VERB(STANDARD) {
          std::cout << "\t**" <<
          "Not supported on this machine" << std::endl;
          return;
        }
      }
    } else {
      CHK_ERR_ASRT(err);
      IF_VERB(STANDARD) {
          std::cout << std::dec << "\tsystem_clock_counter=" << smu.system_clock_counter << '\n';
          std::cout << std::dec << "\ttemperature_edge=" << smu.temperature_edge << '\n';
          std::cout << std::dec << "\ttemperature_hotspot=" << smu.temperature_hotspot << '\n';
          std::cout << std::dec << "\ttemperature_mem=" << smu.temperature_mem << '\n';
          std::cout << std::dec << "\ttemperature_vrgfx=" << smu.temperature_vrgfx << '\n';
          std::cout << std::dec << "\ttemperature_vrsoc=" << smu.temperature_vrsoc << '\n';
          std::cout << std::dec << "\ttemperature_vrmem=" << smu.temperature_vrmem << '\n';
          std::cout << std::dec << "\taverage_gfx_activity=" << smu.average_gfx_activity << '\n';
          std::cout << std::dec << "\taverage_umc_activity=" << smu.average_umc_activity << '\n';
          std::cout << std::dec << "\taverage_mm_activity=" << smu.average_mm_activity << '\n';
          std::cout << std::dec << "\taverage_socket_power=" << smu.average_socket_power << '\n';
          std::cout << std::dec << "\tenergy_accumulator=" << smu.energy_accumulator << '\n';
          std::cout << std::dec << "\taverage_gfxclk_frequency=" << smu.average_gfxclk_frequency << '\n';
          std::cout << std::dec << "\taverage_uclk_frequency=" << smu.average_uclk_frequency << '\n';
          std::cout << std::dec << "\taverage_vclk0_frequency=" << smu.average_vclk0_frequency << '\n';
          std::cout << std::dec << "\taverage_dclk0_frequency=" << smu.average_dclk0_frequency << '\n';
          std::cout << std::dec << "\taverage_vclk1_frequency=" << smu.average_vclk1_frequency << '\n';
          std::cout << std::dec << "\taverage_dclk1_frequency=" << smu.average_dclk1_frequency << '\n';
          std::cout << std::dec << "\tcurrent_gfxclk=" << smu.current_gfxclk << '\n';
          std::cout << std::dec << "\tcurrent_socclk=" << smu.current_socclk << '\n';
          std::cout << std::dec << "\tcurrent_uclk=" << smu.current_uclk << '\n';
          std::cout << std::dec << "\tcurrent_vclk0=" << smu.current_vclk0 << '\n';
          std::cout << std::dec << "\tcurrent_dclk0=" << smu.current_dclk0 << '\n';
          std::cout << std::dec << "\tcurrent_vclk1=" << smu.current_vclk1 << '\n';
          std::cout << std::dec << "\tcurrent_dclk1=" << smu.current_dclk1 << '\n';
          std::cout << std::dec << "\tthrottle_status=" << smu.throttle_status << '\n';
          std::cout << std::dec << "\tcurrent_fan_speed=" << smu.current_fan_speed << '\n';
          std::cout << std::dec << "\tpcie_link_width=" << smu.pcie_link_width << '\n';
          std::cout << std::dec << "\tpcie_link_speed=" << smu.pcie_link_speed << '\n';
          std::cout << std::dec << "\tgfx_activity_acc=" << std::dec << smu.gfx_activity_acc << '\n';
          std::cout << std::dec << "\tmem_activity_acc=" << std::dec << smu.mem_activity_acc  << '\n';

          for (int i = 0; i < RSMI_NUM_HBM_INSTANCES; ++i) {
            std::cout << "\ttemperature_hbm[" << i << "]=" << std::dec << smu.temperature_hbm[i] << '\n';
          }
          std::cout << "\n";
          std::cout << "\tfirmware_timestamp=" << std::dec << smu.firmware_timestamp  << '\n';
          std::cout << "\tvoltage_soc=" << std::dec << smu.voltage_soc  << '\n';
          std::cout << "\tvoltage_gfx=" << std::dec << smu.voltage_gfx  << '\n';
          std::cout << "\tvoltage_mem=" << std::dec << smu.voltage_mem  << '\n';
          std::cout << "\tindep_throttle_status=" << std::dec << smu.indep_throttle_status  << '\n';
          std::cout << "\tcurrent_socket_power=" << std::dec << smu.current_socket_power  << '\n';

          for (int i = 0; i < RSMI_MAX_NUM_VCNS; ++i) {
            std::cout << "\tvcn_activity[" << i << "]=" << std::dec << smu.vcn_activity[i] << '\n';
          }
          std::cout << "\n";

          for (int i = 0; i < RSMI_MAX_NUM_JPEG_ENGS; ++i) {
            std::cout << "\tjpeg_activity[" << i << "]=" << std::dec << smu.jpeg_activity[i] << '\n';
          }
          std::cout << "\n";

          std::cout << "\tgfxclk_lock_status=" << std::dec << smu.gfxclk_lock_status  << '\n';
          std::cout << "\txgmi_link_width=" << std::dec << smu.xgmi_link_width  << '\n';
          std::cout << "\txgmi_link_speed=" << std::dec << smu.xgmi_link_speed  << '\n';
          std::cout << "\tpcie_bandwidth_acc=" << std::dec << smu.pcie_bandwidth_acc  << '\n';
          std::cout << "\tpcie_bandwidth_inst=" << std::dec << smu.pcie_bandwidth_inst  << '\n';
          std::cout << "\tpcie_l0_to_recov_count_acc=" << std::dec << smu.pcie_l0_to_recov_count_acc << '\n';
          std::cout << "\tpcie_replay_count_acc=" << std::dec << smu.pcie_replay_count_acc << '\n';
          std::cout << "\tpcie_replay_rover_count_acc=" << std::dec << smu.pcie_replay_rover_count_acc << '\n';
          for (int i = 0; i < RSMI_MAX_NUM_XGMI_LINKS; ++i) {
            std::cout << "\txgmi_read_data_acc[" << i << "]=" << std::dec << smu.xgmi_read_data_acc[i] << '\n';
          }

          std::cout << "\n";
          for (int i = 0; i < RSMI_MAX_NUM_XGMI_LINKS; ++i) {
            std::cout << "\txgmi_write_data_acc[" << i << "]=" << std::dec << smu.xgmi_write_data_acc[i] << '\n';
          }

          std::cout << "\n";
          for (int i = 0; i < RSMI_MAX_NUM_GFX_CLKS; ++i) {
            std::cout << "\tcurrent_gfxclks[" << i << "]=" << std::dec << smu.current_gfxclks[i] << '\n';
          }

          std::cout << "\n";
          for (int i = 0; i < RSMI_MAX_NUM_CLKS; ++i) {
            std::cout << "\tcurrent_socclks[" << i << "]=" << std::dec << smu.current_socclks[i] << '\n';
          }

          std::cout << "\n";
          for (int i = 0; i < RSMI_MAX_NUM_CLKS; ++i) {
            std::cout << "\tcurrent_vclk0s[" << i << "]=" << std::dec << smu.current_vclk0s[i] << '\n';
          }

          std::cout << "\n";
          for (int i = 0; i < RSMI_MAX_NUM_CLKS; ++i) {
            std::cout << "\tcurrent_dclk0s[" << i << "]=" << std::dec << smu.current_dclk0s[i] << '\n';
          }
      }
    }

    // Verify api support checking functionality is working
    err = rsmi_dev_gpu_metrics_info_get(i, nullptr);
    ASSERT_EQ(err, RSMI_STATUS_INVALID_ARGS);
  }


  //
  auto val_ui16 = uint16_t(0);
  auto val_ui32 = uint32_t(0);
  auto val_ui64 = uint64_t(0);
  auto status_code(rsmi_status_t::RSMI_STATUS_SUCCESS);

  std::cout << "\n\t**GPU METRICS: Using direct APIs (newer):\n";
  for (uint32_t i = 0; i < num_monitor_devs(); ++i) {
    PrintDeviceHeader(i);

    auto temp_edge_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_edge_get(i, &temp_edge_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_edge_get", status_code);

    auto temp_hotspot_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_hotspot_get(i, &temp_hotspot_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_hotspot_get", status_code);

    auto temp_mem_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_mem_get(i, &temp_mem_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_mem_get", status_code);

    auto temp_vrgfx_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_vrgfx_get(i, &temp_vrgfx_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_vrgfx_get", status_code);

    auto temp_vrsoc_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_vrsoc_get(i, &temp_vrsoc_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_vrsoc_get", status_code);

    auto temp_vrmem_value = val_ui16;
    status_code = rsmi_dev_metrics_temp_vrmem_get(i, &temp_vrmem_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_vrmem_get", status_code);

    GPUMetricTempHbm_t temp_hbm_values{};
    status_code = rsmi_dev_metrics_temp_hbm_get(i, &temp_hbm_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_temp_hbm_get", status_code);

    auto temp_curr_socket_power_value = val_ui16;
    status_code = rsmi_dev_metrics_curr_socket_power_get(i, &temp_curr_socket_power_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_socket_power_get", status_code);

    auto temp_energy_accum_value = val_ui64;
    status_code = rsmi_dev_metrics_energy_acc_get(i, &temp_energy_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_energy_acc_get", status_code);

    auto temp_avg_socket_power_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_socket_power_get(i, &temp_avg_socket_power_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_socket_power_get", status_code);

    auto temp_avg_gfx_activity_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_gfx_activity_get(i, &temp_avg_gfx_activity_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_gfx_activity_get", status_code);

    auto temp_avg_umc_activity_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_umc_activity_get(i, &temp_avg_umc_activity_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_umc_activity_get", status_code);

    auto temp_avg_mm_activity_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_mm_activity_get(i, &temp_avg_mm_activity_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_mm_activity_get", status_code);

    GPUMetricVcnActivity_t temp_vcn_values{};
    status_code = rsmi_dev_metrics_vcn_activity_get(i, &temp_vcn_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_vcn_activity_get", status_code);

    GPUMetricJpegActivity_t temp_jpeg_values{};
    status_code = rsmi_dev_metrics_jpeg_activity_get(i, &temp_jpeg_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_jpeg_activity_get", status_code);

    auto temp_mem_activity_accum_value = val_ui32;
    status_code = rsmi_dev_metrics_mem_activity_acc_get(i, &temp_mem_activity_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_mem_activity_acc_get", status_code);

    auto temp_gfx_activity_accum_value = val_ui32;
    status_code = rsmi_dev_metrics_gfx_activity_acc_get(i, &temp_gfx_activity_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_gfx_activity_acc_get", status_code);

    auto temp_avg_gfx_clock_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_gfx_clock_frequency_get(i, &temp_avg_gfx_clock_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_gfx_clock_frequency_get", status_code);

    auto temp_avg_soc_clock_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_soc_clock_frequency_get(i, &temp_avg_soc_clock_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_soc_clock_frequency_get", status_code);

    auto temp_avg_uclock_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_uclock_frequency_get(i, &temp_avg_uclock_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_uclock_frequency_get", status_code);

    auto temp_avg_vclock0_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_vclock0_frequency_get(i, &temp_avg_vclock0_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_vclock0_frequency_get", status_code);

    auto temp_avg_dclock0_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_dclock0_frequency_get(i, &temp_avg_dclock0_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_dclock0_frequency_get", status_code);

    auto temp_avg_vclock1_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_vclock1_frequency_get(i, &temp_avg_vclock1_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_vclock1_frequency_get", status_code);

    auto temp_avg_dclock1_freq_value = val_ui16;
    status_code = rsmi_dev_metrics_avg_dclock1_frequency_get(i, &temp_avg_dclock1_freq_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_avg_dclock1_frequency_get", status_code);

    auto temp_curr_vclk1_value = val_ui16;
    status_code = rsmi_dev_metrics_curr_vclk1_get(i, &temp_curr_vclk1_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_vclk1_get", status_code);

    auto temp_curr_dclk1_value = val_ui16;
    status_code = rsmi_dev_metrics_curr_dclk1_get(i, &temp_curr_dclk1_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_dclk1_get", status_code);

    auto temp_curr_uclk_value = val_ui16;
    status_code = rsmi_dev_metrics_curr_uclk_get(i, &temp_curr_uclk_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_uclk_get", status_code);

    GPUMetricCurrDClk0_t temp_curr_dclk0_values{};
    status_code = rsmi_dev_metrics_curr_dclk0_get(i, &temp_curr_dclk0_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_dclk0_get", status_code);

    GPUMetricCurrGfxClk_t temp_curr_gfxclk_values{};
    status_code = rsmi_dev_metrics_curr_gfxclk_get(i, &temp_curr_gfxclk_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_gfxclk_get", status_code);

    GPUMetricCurrSocClk_t temp_curr_socclk_values{};
    status_code = rsmi_dev_metrics_curr_socclk_get(i, &temp_curr_socclk_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_socclk_get", status_code);

    GPUMetricCurrVClk0_t temp_curr_vclk0_values{};
    status_code = rsmi_dev_metrics_curr_vclk0_get(i, &temp_curr_vclk0_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_vclk0_get", status_code);

    auto temp_indep_throttle_status_value = val_ui64;
    status_code = rsmi_dev_metrics_indep_throttle_status_get(i, &temp_indep_throttle_status_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_indep_throttle_status_get", status_code);

    auto temp_throttle_status_value = val_ui32;
    status_code = rsmi_dev_metrics_throttle_status_get(i, &temp_throttle_status_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_throttle_status_get", status_code);

    auto temp_gfxclk_lock_status_value = val_ui32;
    status_code = rsmi_dev_metrics_gfxclk_lock_status_get(i, &temp_gfxclk_lock_status_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_gfxclk_lock_status_get", status_code);

    auto temp_curr_fan_speed_value = val_ui16;
    status_code = rsmi_dev_metrics_curr_fan_speed_get(i, &temp_curr_fan_speed_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_curr_fan_speed_get", status_code);

    auto temp_pcie_link_width_value = val_ui16;
    status_code = rsmi_dev_metrics_pcie_link_width_get(i, &temp_pcie_link_width_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_link_width_get", status_code);

    auto temp_pcie_link_speed_value = val_ui16;
    status_code = rsmi_dev_metrics_pcie_link_speed_get(i, &temp_pcie_link_speed_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_link_speed_get", status_code);

    auto temp_pcie_bandwidth_accum_value = val_ui64;
    status_code = rsmi_dev_metrics_pcie_bandwidth_acc_get(i, &temp_pcie_bandwidth_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_bandwidth_acc_get", status_code);

    auto temp_pcie_bandwidth_inst_value = val_ui64;
    status_code = rsmi_dev_metrics_pcie_bandwidth_inst_get(i, &temp_pcie_bandwidth_inst_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_bandwidth_inst_get", status_code);

    auto temp_pcie_l0_recov_count_accum_value = val_ui64;
    status_code = rsmi_dev_metrics_pcie_l0_recov_count_acc_get(i, &temp_pcie_l0_recov_count_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_l0_recov_count_acc_get", status_code);

    auto temp_pcie_replay_count_accum_value = val_ui64;
    status_code = rsmi_dev_metrics_pcie_replay_count_acc_get(i, &temp_pcie_replay_count_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_replay_count_acc_get", status_code);

    auto temp_pcie_replay_rover_count_accum_value = val_ui64;
    status_code = rsmi_dev_metrics_pcie_replay_rover_count_acc_get(i, &temp_pcie_replay_rover_count_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_replay_rover_count_acc_get", status_code);

    auto temp_pcie_nak_sent_count_accum_value = val_ui32;
    status_code = rsmi_dev_metrics_pcie_nak_sent_count_acc_get(i, &temp_pcie_nak_sent_count_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_nak_sent_count_acc_get", status_code);

    auto temp_pcie_nak_rcvd_count_accum_value = val_ui32;
    status_code = rsmi_dev_metrics_pcie_nak_rcvd_count_acc_get(i, &temp_pcie_nak_rcvd_count_accum_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_pcie_nak_rcvd_count_acc_get", status_code);

    auto temp_xgmi_link_width_value = val_ui16;
    status_code = rsmi_dev_metrics_xgmi_link_width_get(i, &temp_xgmi_link_width_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_xgmi_link_width_get", status_code);

    auto temp_xgmi_link_speed_value = val_ui16;
    status_code = rsmi_dev_metrics_xgmi_link_speed_get(i, &temp_xgmi_link_speed_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_xgmi_link_speed_get", status_code);

    GPUMetricXgmiReadDataAcc_t temp_xgmi_read_values{};
    status_code = rsmi_dev_metrics_xgmi_read_data_get(i, &temp_xgmi_read_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_xgmi_read_data_get", status_code);

    GPUMetricXgmiWriteDataAcc_t temp_xgmi_write_values{};
    status_code = rsmi_dev_metrics_xgmi_write_data_get(i, &temp_xgmi_write_values);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_xgmi_write_data_get", status_code);

    auto temp_voltage_soc_value = val_ui16;
    status_code = rsmi_dev_metrics_volt_soc_get(i, &temp_voltage_soc_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_volt_soc_get", status_code);

    auto temp_voltage_gfx_value = val_ui16;
    status_code = rsmi_dev_metrics_volt_gfx_get(i, &temp_voltage_gfx_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_volt_gfx_get", status_code);

    auto temp_voltage_mem_value = val_ui16;
    status_code = rsmi_dev_metrics_volt_mem_get(i, &temp_voltage_mem_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_volt_mem_get", status_code);

    auto temp_system_clock_counter_value = val_ui64;
    status_code = rsmi_dev_metrics_system_clock_counter_get(i, &temp_system_clock_counter_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_system_clock_counter_get", status_code);

    auto temp_firmware_timestamp_value = val_ui64;
    status_code = rsmi_dev_metrics_firmware_timestamp_get(i, &temp_firmware_timestamp_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_firmware_timestamp_get", status_code);

    auto temp_xcd_counter_value = val_ui16;
    status_code = rsmi_dev_metrics_xcd_counter_get(i, &temp_xcd_counter_value);
    if (status_code != RSMI_STATUS_NOT_SUPPORTED) {
      CHK_ERR_ASRT(status_code);
    }
    MetricResults.emplace("rsmi_dev_metrics_xcd_counter_get", status_code);

    IF_VERB(STANDARD) {
      std::cout << "\n";
      std::cout << "\t[Temperature]" << "\n";
      std::cout << print_error_or_value("\t  -> temp_edge(): ", "rsmi_dev_metrics_temp_edge_get", temp_edge_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_hotspot(): ", "rsmi_dev_metrics_temp_hotspot_get", temp_hotspot_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_mem(): ", "rsmi_dev_metrics_temp_mem_get", temp_mem_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_vrgfx(): ", "rsmi_dev_metrics_temp_vrgfx_get", temp_vrgfx_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_vrsoc(): ", "rsmi_dev_metrics_temp_vrsoc_get", temp_vrsoc_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_vrmem(): ", "rsmi_dev_metrics_temp_vrmem_get", temp_vrmem_value) << "\n";
      std::cout << print_error_or_value("\t  -> temp_hbm[]: ", "rsmi_dev_metrics_temp_hbm_get", temp_hbm_values) << "\n";

      std::cout << "\n";
      std::cout << "\t[Power/Energy]" << "\n";
      std::cout << print_error_or_value("\t  -> current_socket_power(): ", "rsmi_dev_metrics_curr_socket_power_get", temp_curr_socket_power_value) << "\n";
      std::cout << print_error_or_value("\t  -> energy_accum(): ", "rsmi_dev_metrics_energy_acc_get", temp_energy_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_socket_power(): ", "rsmi_dev_metrics_avg_socket_power_get", temp_avg_socket_power_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Utilization]" << "\n";
      std::cout << print_error_or_value("\t  -> average_gfx_activity(): ", "rsmi_dev_metrics_avg_gfx_activity_get", temp_avg_gfx_activity_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_umc_activity(): ", "rsmi_dev_metrics_avg_umc_activity_get", temp_avg_umc_activity_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_mm_activity(): ", "rsmi_dev_metrics_avg_mm_activity_get", temp_avg_mm_activity_value) << "\n";
      std::cout << print_error_or_value("\t  -> vcn_activity[]: ", "rsmi_dev_metrics_vcn_activity_get", temp_vcn_values) << "\n";
      std::cout << print_error_or_value("\t  -> jpeg_activity[]: ", "rsmi_dev_metrics_jpeg_activity_get", temp_jpeg_values) << "\n";

      std::cout << "\n";
      std::cout << print_error_or_value("\t  -> mem_activity_accum(): ", "rsmi_dev_metrics_mem_activity_acc_get", temp_mem_activity_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> gfx_activity_accum(): ", "rsmi_dev_metrics_gfx_activity_acc_get", temp_gfx_activity_accum_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Average Clock]" << "\n";
      std::cout << print_error_or_value("\t  -> average_gfx_clock_frequency(): ", "rsmi_dev_metrics_avg_gfx_clock_frequency_get", temp_avg_gfx_clock_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_soc_clock_frequency(): ", "rsmi_dev_metrics_avg_soc_clock_frequency_get", temp_avg_soc_clock_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_uclock_frequency(): ", "rsmi_dev_metrics_avg_uclock_frequency_get", temp_avg_uclock_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_vclock0_frequency(): ", "rsmi_dev_metrics_avg_vclock0_frequency_get", temp_avg_vclock0_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_dclock0_frequency(): ", "rsmi_dev_metrics_avg_dclock0_frequency_get", temp_avg_dclock0_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_vclock1_frequency(): ", "rsmi_dev_metrics_avg_vclock1_frequency_get", temp_avg_vclock1_freq_value) << "\n";
      std::cout << print_error_or_value("\t  -> average_dclock1_frequency(): ", "rsmi_dev_metrics_avg_dclock1_frequency_get", temp_avg_dclock1_freq_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Current Clock]" << "\n";
      std::cout << print_error_or_value("\t  -> current_vclock1(): ", "rsmi_dev_metrics_curr_vclk1_get", temp_curr_vclk1_value) << "\n";
      std::cout << print_error_or_value("\t  -> current_dclock1(): ", "rsmi_dev_metrics_curr_dclk1_get", temp_curr_dclk1_value) << "\n";
      std::cout << print_error_or_value("\t  -> current_uclock(): ", "rsmi_dev_metrics_curr_uclk_get", temp_curr_uclk_value) << "\n";
      std::cout << print_error_or_value("\t  -> current_dclk0[]: ", "rsmi_dev_metrics_curr_dclk0_get", temp_curr_dclk0_values) << "\n";
      std::cout << print_error_or_value("\t  -> current_gfxclk[]: ", "rsmi_dev_metrics_curr_gfxclk_get", temp_curr_gfxclk_values) << "\n";
      std::cout << print_error_or_value("\t  -> current_soc_clock[]: ", "rsmi_dev_metrics_curr_socclk_get", temp_curr_socclk_values) << "\n";
      std::cout << print_error_or_value("\t  -> current_vclk0[]: ", "rsmi_dev_metrics_curr_vclk0_get", temp_curr_vclk0_values) << "\n";

      std::cout << "\n";
      std::cout << "\t[Throttle]" << "\n";
      std::cout << print_error_or_value("\t  -> indep_throttle_status(): ", "rsmi_dev_metrics_indep_throttle_status_get", temp_indep_throttle_status_value) << "\n";
      std::cout << print_error_or_value("\t  -> throttle_status(): ", "rsmi_dev_metrics_throttle_status_get", temp_throttle_status_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Gfx Clock Lock]" << "\n";
      std::cout << print_error_or_value("\t  -> gfxclk_lock_status(): ", "rsmi_dev_metrics_gfxclk_lock_status_get", temp_gfxclk_lock_status_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Current Fan Speed]" << "\n";
      std::cout << print_error_or_value("\t  -> current_fan_speed(): ", "rsmi_dev_metrics_curr_fan_speed_get", temp_curr_fan_speed_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Link/Bandwidth/Speed]" << "\n";
      std::cout << print_error_or_value("\t  -> pcie_link_width(): ", "rsmi_dev_metrics_pcie_link_width_get", temp_pcie_link_width_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_link_speed(): ", "rsmi_dev_metrics_pcie_link_speed_get", temp_pcie_link_speed_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_bandwidth_accum(): ", "rsmi_dev_metrics_pcie_bandwidth_acc_get", temp_pcie_bandwidth_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_bandwidth_inst(): ", "rsmi_dev_metrics_pcie_bandwidth_inst_get", temp_pcie_bandwidth_inst_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_l0_recov_count_accum(): ", "rsmi_dev_metrics_pcie_l0_recov_count_acc_get", temp_pcie_l0_recov_count_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_replay_count_accum(): ", "rsmi_dev_metrics_pcie_replay_count_acc_get", temp_pcie_replay_count_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_replay_rollover_count_accum(): ", "rsmi_dev_metrics_pcie_replay_rover_count_acc_get", temp_pcie_replay_rover_count_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_nak_sent_count_accum(): ", "rsmi_dev_metrics_pcie_nak_sent_count_acc_get", temp_pcie_nak_sent_count_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> pcie_nak_rcvd_count_accum(): ", "rsmi_dev_metrics_pcie_nak_rcvd_count_acc_get", temp_pcie_nak_rcvd_count_accum_value) << "\n";
      std::cout << print_error_or_value("\t  -> xgmi_link_width(): ", "rsmi_dev_metrics_xgmi_link_width_get", temp_xgmi_link_width_value) << "\n";
      std::cout << print_error_or_value("\t  -> xgmi_link_speed(): ", "rsmi_dev_metrics_xgmi_link_speed_get", temp_xgmi_link_speed_value) << "\n";
      std::cout << print_error_or_value("\t  -> xgmi_read_data[]: ", "rsmi_dev_metrics_xgmi_read_data_get", temp_xgmi_read_values) << "\n";
      std::cout << print_error_or_value("\t  -> xgmi_write_data[]: ", "rsmi_dev_metrics_xgmi_write_data_get", temp_xgmi_write_values) << "\n";

      std::cout << "\n";
      std::cout << "\t[Voltage]" << "\n";
      std::cout << print_error_or_value("\t  -> voltage_soc(): ", "rsmi_dev_metrics_volt_soc_get", temp_voltage_soc_value) << "\n";
      std::cout << print_error_or_value("\t  -> voltage_gfx(): ", "rsmi_dev_metrics_volt_gfx_get", temp_voltage_gfx_value) << "\n";
      std::cout << print_error_or_value("\t  -> voltage_mem(): ", "rsmi_dev_metrics_volt_mem_get", temp_voltage_mem_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[Timestamp]" << "\n";
      std::cout << print_error_or_value("\t  -> system_clock_counter(): ", "rsmi_dev_metrics_system_clock_counter_get", temp_system_clock_counter_value) << "\n";
      std::cout << print_error_or_value("\t  -> firmware_timestamp(): ", "rsmi_dev_metrics_firmware_timestamp_get", temp_firmware_timestamp_value) << "\n";

      std::cout << "\n";
      std::cout << "\t[XCD CounterVoltage]" << "\n";
      std::cout << print_error_or_value("\t  -> xcd_counter(): ", "rsmi_dev_metrics_xcd_counter_get", temp_xcd_counter_value) << "\n";
      std::cout << "\n\n";
    }
  }

}
