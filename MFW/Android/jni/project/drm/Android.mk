# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE    := voDivXDRM
LOCAL_SRC_FILES := CvoDivXDRM.cpp voDivXDRM.cpp voDivXDRMJni.cpp vomutex.cpp

VOROOTDIR := /home/david/vomm
LOCAL_C_INCLUDES :=. $(VOROOTDIR)/Include $(VOROOTDIR)/Thirdparty/DivXDRMSDK/inc


LOCAL_STATIC_LIBRARIES := ComponentCryptoLayer DRMCommon

LOCAL_LDLIBS += -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
