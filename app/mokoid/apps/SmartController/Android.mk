# Copyright (C) 2009 Mokoid Open Source Project
# Copyright (C) 2009,2010 Moko365 Inc.
#
# Author: Jollen Chen <jollen@moko365.com>
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

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := user eng

# This is the target being built.
LOCAL_PACKAGE_NAME := SmartController

# Only compile source java files in this apk.
# LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES := $(call all-java-files-under)

# Link against the current Android SDK.
LOCAL_SDK_VERSION := current

# Also link against our own custom library.
LOCAL_JAVA_LIBRARIES := mokoid

include $(BUILD_PACKAGE)

