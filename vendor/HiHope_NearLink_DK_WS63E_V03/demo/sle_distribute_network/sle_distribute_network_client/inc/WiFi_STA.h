/*
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */

/**
 * @defgroup
 * @ingroup
 * @{
 */

#ifndef WIFI_STA_H
#define WIFI_STA_H

/**
 * @if Eng
 * @brief  start wifi sta function, find and connect wifi ap.
 * @attention  NULL
 * @param  [in] ssid              The SSID of WiFi AP
 * @param  [in] ssid_len          The length of ssid, including '\0' at the end of the string
 * @param  [in] key               The Password of WiFi AP
 * @param  [in] key_len           The length of key, Including '\0' at the end of the string
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute failed
 * @par Dependency:
 * @li NULL
 * @else
 * @brief  启动WiFi STA功能，查找并连接WiFi热点。
 * @attention  NULL
 * @param  [in] ssid              WiFi热点的SSID
 * @param  [in] ssid_len          SSID的长度, 包括字符串结尾的'\0'
 * @param  [in] key               WiFi热点的密码
 * @param  [in] key_len           KEY的长度, 包括字符串结尾的'\0'
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li NULL
 * @endif
 */
errcode_t example_sta_function(const char *ssid, uint8_t ssid_len, const char *key, uint8_t key_len);

#endif