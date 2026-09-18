package com.example.sdl3staticextensions.test;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public final class TestActivity extends Activity {
    private static final String TAG = "grapple-beam-android-test";

    static {
        System.loadLibrary("grapple_android_test");
    }

    private static native int runNativeTests();

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        Thread runner = new Thread(() -> {
            int failures = runNativeTests();
            if (failures == 0) {
                Log.i(TAG, "GRAPPLE_BEAM_ANDROID_TESTS: PASS");
            } else {
                Log.e(TAG, "GRAPPLE_BEAM_ANDROID_TESTS: FAIL (" + failures + " failures)");
            }
            finishAndRemoveTask();
        }, "grapple-beam-native-tests");
        runner.start();
    }
}
