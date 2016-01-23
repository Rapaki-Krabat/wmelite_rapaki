--- android_sdl_mixer/src/org/libsdl/app/SDLActivity.java.orig	2016-01-23 16:40:58.427831335 +0100
+++ android_sdl_mixer/src/org/libsdl/app/SDLActivity.java	2016-01-23 16:36:22.359817586 +0100
@@ -30,6 +30,9 @@
 import android.hardware.*;
 import android.content.pm.ActivityInfo;
 
+// WMELite addition
+import org.deadcode.wmelite.WMELiteFunctions;
+
 /**
     SDL Activity
 */
@@ -60,6 +63,9 @@
     // Audio
     protected static AudioTrack mAudioTrack;
 
+    // WMELite added
+    protected static final WMELiteFunctions wmeLiteFuncs = new WMELiteFunctions();
+
     /**
      * This method is called by SDL before loading the native shared libraries.
      * It can be overridden to provide names of shared libraries to be loaded.
@@ -70,12 +76,17 @@
      */
     protected String[] getLibraries() {
         return new String[] {
+            "gnustl_shared",    // WMELite additional libraries added
+            "freeimage",
             "SDL2",
             // "SDL2_image",
-            // "SDL2_mixer",
+            "SDL2_mixer",
             // "SDL2_net",
             // "SDL2_ttf",
-            "main"
+            "effectsprivate",
+            "echo",
+            "main",
+            "dcscomp"
         };
     }
 
@@ -121,6 +132,9 @@
         Log.v(TAG, "onCreate(): " + mSingleton);
         super.onCreate(savedInstanceState);
 
+        // WMELite added
+        wmeLiteFuncs.setContext(getApplicationContext());
+        
         SDLActivity.initialize();
         // So we can call stuff from static callbacks
         mSingleton = this;
@@ -164,6 +178,13 @@
         // Set up the surface
         mSurface = new SDLSurface(getApplication());
 
+        nativeAddHintCallback(SDL_HINT_ANDROID_USE_UI_LOW_PROFILE, new SDLHintCallback() {
+        	@Override
+        	public void callback(String name, String oldValue, String newValue) {
+        		updateLowProfileSettings(newValue);
+        	}
+        });
+        
         if(Build.VERSION.SDK_INT >= 12) {
             mJoystickHandler = new SDLJoystickHandler_API12();
         }
@@ -211,6 +232,8 @@
         }
 
         SDLActivity.handleResume();
+        
+        updateLowProfileSettings(nativeGetHint(SDL_HINT_ANDROID_USE_UI_LOW_PROFILE));
     }
 
 
@@ -323,6 +346,20 @@
         mSingleton.finish();
     }
 
+    private static final String SDL_HINT_ANDROID_USE_UI_LOW_PROFILE = "SDL_ANDROID_USE_UI_LOW_PROFILE";
+    		
+    void updateLowProfileSettings(String value) {
+    	if ("1".equals(value)) {
+    		runOnUiThread(new Runnable() {
+    			@Override
+    			public void run() {
+    				if (Build.VERSION.SDK_INT >= 14) {
+    					getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
+    				}
+    			}
+    		});
+    	}
+    }
 
     // Messages from the SDLMain thread
     static final int COMMAND_CHANGE_TITLE = 1;
@@ -432,6 +469,11 @@
                                                int is_accelerometer, int nbuttons,
                                                int naxes, int nhats, int nballs);
     public static native int nativeRemoveJoystick(int device_id);
+
+    interface SDLHintCallback {
+    	void callback(String name, String oldValue, String newValue);
+    }
+    public static native void nativeAddHintCallback(String name, SDLHintCallback callback);
     public static native String nativeGetHint(String name);
 
     /**
@@ -953,6 +995,9 @@
 class SDLMain implements Runnable {
     @Override
     public void run() {
+    	// WMELite added
+    	SDLActivity.wmeLiteFuncs.init();
+
         // Runs SDL_main()
         SDLActivity.nativeInit(SDLActivity.mSingleton.getArguments());
 
