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
    private lateinit var logWindow: TextView
    private lateinit var stopButton : Button
    private lateinit var forwardButton : Button
    private lateinit var leftButton : Button
    private lateinit var rightButton : Button
    private lateinit var backwardsButton : Button

    private var isPressed : Boolean = false
    private var isCommsThreadRunning : Boolean = false

    private val logBuffer = StringBuffer()

    private val commsHeader = "RTS"
    private var commsMsg : String? = null
    private val commsThread = Thread(Runnable {
        // TODO: ensure connection active

        while(isCommsThreadRunning){
            if (isPressed){
                Log.d("RTS", "Send command '$commsMsg'")
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

        // start retrieving view elements
        fullscreenContent = findViewById(R.id.fullscreen_content)
        fullscreenContentControls = findViewById(R.id.linlay_cmd)
        logWindow = findViewById(R.id.txtview_cmds)

        stopButton = findViewById(R.id.btn_stp)
        forwardButton = findViewById(R.id.btn_forward)
        leftButton = findViewById(R.id.btn_left)
        rightButton = findViewById(R.id.btn_right)
        backwardsButton = findViewById(R.id.btn_rev)

        // setup callbacks
        val buttons = arrayOf(stopButton, forwardButton, leftButton, rightButton, backwardsButton)
        for (b in buttons){
            b.setOnTouchListener { v, event -> this.moveButtonOnTouchListener(v, event) }
        }

        // Start the comms thread
        this.isCommsThreadRunning = true
        commsThread.start()

        // Title bar? Not needed
        supportActionBar?.hide()

        onScreenLog("System ready")
    }

    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)
    }

    private fun onScreenLog(msg : String){
        logBuffer.append(msg)
        logWindow.text = logBuffer.toString()
    }

    private fun updateMoveDirectionMessage(directionCharacter: Char){
        commsMsg = commsHeader + "MOVD" + directionCharacter
    }

    private fun moveButtonOnTouchListener(v: View, event: MotionEvent) : Boolean{
        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                isPressed = true
                val button = v as Button
                if (button.text == "STOP"){
                    commsMsg = "RTSSTOP"
                    isPressed = false
                }
                else {
                    updateMoveDirectionMessage(button.text[0])
                }
            }

            MotionEvent.ACTION_UP -> {
                isPressed = false
                commsMsg = null
            }
        }

        return v?.onTouchEvent(event) ?: true
    }

    companion object {

    }
}