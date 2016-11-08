# Copyright 2007-2008 The Android Open Source Project
# svn info:
# URL: svn://124.207.145.157/solution/phone/android-4.0mr1/baseline/tags/2012-08-24_before_dualcard
# Repository Root: svn://124.207.145.157/solution/phone/android-4.0mr1/baseline
# Repository UUID: aa234cfd-0cb6-0410-b29e-f37fc4347524
# Revision: 5363

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_DEX_PREOPT := false

LOCAL_PACKAGE_NAME := ReadBtEarphoneBattLevel

# Builds against the public SDK
#LOCAL_SDK_VERSION := current

include $(BUILD_PACKAGE)

# This finds and builds the test apk as well, so a single make does both.
include $(call all-makefiles-under,$(LOCAL_PATH))
