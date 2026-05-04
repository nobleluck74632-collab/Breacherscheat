package com.omega.breachers

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.PixelFormat
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.WindowManager
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MainActivity : Activity() {
    private lateinit var glSurfaceView: GLSurfaceView
    private var isOverlayActive = false
    private val handler = Handler(Looper.getMainLooper())
    private external fun initNative()
    private external fun startCheatThread(pid: Int)
    private external fun renderOverlay()
    private external fun cleanupNative()

    companion object {
        init {
            System.loadLibrary("breachers_cheat")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or
            WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE or
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
        )
        glSurfaceView = GLSurfaceView(this)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0)
        glSurfaceView.holder.setFormat(PixelFormat.RGBA_8888)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
                initNative()
            }
            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {}
            override fun onDrawFrame(gl: GL10?) {
                renderOverlay()
            }
        })
        setContentView(glSurfaceView)

        if (checkPermissions()) {
            startCheatService()
        } else {
            requestPermissions()
        }
    }

    private fun checkPermissions(): Boolean {
        return (ContextCompat.checkSelfPermission(this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, android.Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED)
    }

    private fun requestPermissions() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE,
                android.Manifest.permission.READ_EXTERNAL_STORAGE
            ),
            1001
        )
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 1001 && grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
            startCheatService()
        } else {
            finish()
        }
    }

    private fun startCheatService() {
        Thread {
            try {
                // Attach to Breachers process (PID via package name)
                val pid = getProcessId("com.omega.breachers") // Replace with actual package
                if (pid > 0) {
                    startCheatThread(pid)
                    isOverlayActive = true
                } else {
                    Log.e("BreachersCheat", "Game process not found")
                    finish()
                }
            } catch (e: Exception) {
                Log.e("BreachersCheat", "Error: ${e.message}")
                finish()
            }
        }.start()
    }

    private fun getProcessId(packageName: String): Int {
        val am = getSystemService(ACTIVITY_SERVICE) as android.app.ActivityManager
        for (proc in am.runningAppProcesses) {
            if (proc.processName == packageName) {
                return proc.pid
            }
        }
        return -1
    }

    override fun onDestroy() {
        super.onDestroy()
        if (isOverlayActive) {
            cleanupNative()
        }
    }
}
