/*
 * =============================================================================
 *   ROC Runtime Conformance Release License
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2022, Advanced Micro Devices, Inc.
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
#include <chrono>


#include "gtest/gtest.h"
#include "rocm_smi/rocm_smi.h"
#include "rocm_smi_test/functional/measure_api_execution_time.h"
#include "rocm_smi_test/test_common.h"
#include "rocm_smi_test/test_utils.h"


TestMeasureApiExecutionTime::TestMeasureApiExecutionTime() : TestBase() {
  set_title("RSMI Measure API Execution Time");
  set_description("This test measures execution times for select APIs");
}

TestMeasureApiExecutionTime::~TestMeasureApiExecutionTime(void) {
}

void TestMeasureApiExecutionTime::SetUp(void) {
  TestBase::SetUp();
  return;
}

void TestMeasureApiExecutionTime::DisplayTestInfo(void) {
  TestBase::DisplayTestInfo();
}

void TestMeasureApiExecutionTime::DisplayResults(void) const {
  TestBase::DisplayResults();
  return;
}

void TestMeasureApiExecutionTime::Close() {
  // This will close handles opened within rsmitst utility calls and call
  // rsmi_shut_down(), so it should be done after other hsa cleanup
  TestBase::Close();
}

void TestMeasureApiExecutionTime::Run(void) {
  int64_t val_i64;
  rsmi_gpu_metrics_t smu;
  rsmi_temperature_metric_t met = RSMI_TEMP_CURRENT;
  rsmi_status_t ret;
  float repeat = 300.0;
  bool skip = false;

  TestBase::Run();
  if (setup_failed_) {
    IF_VERB(STANDARD) {
      std::cout << "** SetUp Failed for this test. Skipping.**" << std::endl;
    }
    return;
  }

  auto prev = std::cout.precision(3);
  for (uint32_t dv_ind = 0; dv_ind < num_monitor_devs(); ++dv_ind) {
    PrintDeviceHeader(dv_ind);

    //test execution time for rsmi_dev_fan_speed_get
    auto start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < repeat; ++i){
      ret = rsmi_dev_fan_speed_get(dv_ind, 0, &val_i64);

    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast
                            <std::chrono::microseconds>(stop - start);

    if (ret != RSMI_STATUS_SUCCESS){
      skip = true;
    }
    std::cout << std:: endl;

    if (!skip) {
      std::cout << "\trsmi_dev_fan_speed_get execution time: " <<
                (float(duration.count()) / repeat) << " microseconds" << std::endl;
      EXPECT_LT(duration.count(), 1000 * repeat);
    }
    skip = false;

    //test execution time for rsmi_dev_temp_metric_get
    start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < repeat; ++i){
      ret = rsmi_dev_temp_metric_get(dv_ind, 0, met, &val_i64);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast
                            <std::chrono::microseconds>(stop - start);

    if (ret != RSMI_STATUS_SUCCESS){
      skip = true;
    }
    if (!skip) {
      std::cout << "\trsmi_dev_temp_metric_get execution time: " <<
                (float(duration.count()) / repeat  ) << " microseconds" << std::endl;
      EXPECT_LT(duration.count(), 500 * repeat);
    }
    skip = false;

    //test execution time for rsmi_dev_gpu_metrics_info_get
    start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < repeat; ++i){
      ret = rsmi_dev_gpu_metrics_info_get(dv_ind, &smu);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast
                            <std::chrono::microseconds>(stop - start) ;

    if (ret != RSMI_STATUS_SUCCESS){
      skip = true;
    }
    if (!skip) {
      std::cout << "\trsmi_dev_gpu_metrics_info_get execution time: " <<
                (float(duration.count()) / repeat ) << " microseconds" << std::endl;
      EXPECT_LT(duration.count(), 500 * repeat);
    }
    skip = false;
    std::cout << "----------------------------------------------------------------------------" << std::endl;

    auto val_ui16 = uint16_t(0);
    auto status_code(rsmi_status_t::RSMI_STATUS_SUCCESS);
    auto start_api = std::chrono::high_resolution_clock::now();
    for (int i=0; i < repeat; ++i) {
      status_code = rsmi_dev_metrics_xcd_counter_get(dv_ind, &val_ui16);
    }
    auto stop_api = std::chrono::high_resolution_clock::now();
    auto duration_api = std::chrono::duration_cast<std::chrono::microseconds>(stop_api - start_api);
    if (status_code != rsmi_status_t::RSMI_STATUS_SUCCESS){
      skip = true;
    }
    if (!skip) {
      std::cout << "\rsmi_dev_metrics_xcd_counter_get() execution time: "
                << (float(duration_api.count()) / repeat) << " microseconds" << std::endl;
      EXPECT_LT(duration_api.count(), 500 * repeat);
    }
    skip = false;
    std::cout << "----------------------------------------------------------------------------" << std::endl;

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if (!skip) {
      std::cout << "\rTotal execution time (All APIs): "
                << (float(duration_api.count()) / repeat) << " microseconds" << std::endl;
      EXPECT_LT(duration_api.count(), (500 * repeat));
    }
    skip = false;
    std::cout << "============================================================================" << std::endl;

  }
  std::cout.precision(prev);

}
