#===============================================================================
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_LED_SERVER)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/SLE_LED_Server/src/SLE_LED_Server_adv.c" "${CMAKE_CURRENT_SOURCE_DIR}/SLE_LED_Server/src/SLE_LED_Server.c" "${CMAKE_CURRENT_SOURCE_DIR}/SLE_LED_Server/inc" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_LED_CLIENT)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/SLE_LED_Client/src/SLE_LED_Client.c" "${CMAKE_CURRENT_SOURCE_DIR}/SLE_LED_Client/inc" PARENT_SCOPE)
endif()