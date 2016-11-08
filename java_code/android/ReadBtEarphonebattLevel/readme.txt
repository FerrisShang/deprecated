Android device read bluetooth earphone battery level

this file must also be changed : /package/app/Bluetooth

diff --git a/src/com/android/bluetooth/hfp/HeadsetStateMachine.java b/src/com/android/bluetoo
index 3bbaa35..df11bc2 100644
--- a/src/com/android/bluetooth/hfp/HeadsetStateMachine.java
+++ b/src/com/android/bluetooth/hfp/HeadsetStateMachine.java
@@ -199,6 +199,7 @@ final class HeadsetStateMachine extends StateMachine {
         VENDOR_SPECIFIC_AT_COMMAND_COMPANY_ID = new HashMap<String, Integer>();
         VENDOR_SPECIFIC_AT_COMMAND_COMPANY_ID.put("+XEVENT", BluetoothAssignedNumbers.PLANTR
         VENDOR_SPECIFIC_AT_COMMAND_COMPANY_ID.put("+ANDROID", BluetoothAssignedNumbers.GOOGL
+        VENDOR_SPECIFIC_AT_COMMAND_COMPANY_ID.put("+IPHONEACCEV", BluetoothAssignedNumbers.A
}

