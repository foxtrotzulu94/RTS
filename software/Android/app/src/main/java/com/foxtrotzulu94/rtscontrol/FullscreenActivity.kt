package com.foxtrotzulu94.rtscontrol

import androidx.appcompat.app.AppCompatActivity
import android.annotation.SuppressLint
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
class FullscreenActivity : AppCompatActivity() {
    private lateinit var fullscreenContent: LinearLayout
    private lateinit var fullscreenContentControls: LinearLayout
    private lateinit var stopButton : Button

    private var isPressed : Boolean = false
    private var isCommsThreadRunning : Boolean = false

    private val commsThread = Thread(Runnable {
        // TODO: ensure connection active

        while(isCommsThreadRunning){
            if (isPressed){
                Log.d("RTS", "Stop button is pressed")
            }

            // Don't starve
            Thread.sleep(50)
        }
    })

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_fullscreen)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        // Set up the user interaction to manually show or hide the system UI.
        fullscreenContent = findViewById(R.id.fullscreen_content)
        fullscreenContentControls = findViewById(R.id.linlay_cmd)

        stopButton = findViewById(R.id.btn_stp)
        stopButton.setOnTouchListener { v, event ->
            when (event?.action) {
                MotionEvent.ACTION_DOWN -> isPressed = true
                MotionEvent.ACTION_UP -> isPressed = false
            }

            v?.onTouchEvent(event) ?: true
        }

        // Start the comms thread
        this.isCommsThreadRunning = true
        commsThread.start()

        // Title bar? Not needed
        supportActionBar?.hide()
    }

    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)
    }

    companion object {

    }
}