package com.foxtrotzulu94.rtscontrol

import androidx.appcompat.app.AppCompatActivity
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothServerSocket
import android.bluetooth.BluetoothSocket
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import java.io.IOException
import java.lang.Exception
import java.util.*

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

    private val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private var bluetoothSocket : BluetoothSocket? = null
    private val logBuffer = StringBuffer()

    private val commsBTSPP = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    private val commsInitHandler = Handler()
    private val commsTargetName = "ShermanToyTank"
    private val commsHeader = "RTS"
    private var commsMsg : String? = null

    private val commsSenderThread = Thread(Runnable {
        // TODO: ensure connection active

        while(isCommsThreadRunning){
            if (isPressed){
                Log.d("RTS", "Send command '$commsMsg'")
            }

            // Don't starve
            Thread.sleep(50)
        }
    })

    private val commsReceiverThread = Thread(Runnable {
        // TODO: ensure connection active
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
        commsReceiverThread.start()

        // Title bar? Not needed
        supportActionBar?.hide()
    }

    override fun onStart() {
        super.onStart()
        commsInitHandler.postDelayed(Runnable { this.setupBluetooth() }, 3000)
        onScreenLog("System started")
    }

    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)
    }

    private fun onScreenLog(msg : String){
        logBuffer.append(msg)
        logBuffer.append('\n')
        logWindow.text = logBuffer.toString()
    }

    private fun setupBluetooth(){
        onScreenLog("Starting bluetooth connection")

        // Check that we can use bluetooth
        if (bluetoothAdapter == null) {
            onScreenLog("Your system does not seem to support bluetooth!")
        }

        // Request they turn it on
        if (bluetoothAdapter?.isEnabled == false) {
            onScreenLog("Requesting to turn on bluetooth")
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT)
        }

        // Check if 'ShermanToyTank' is around
        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter?.bondedDevices
        val toyTank = pairedDevices?.find { device -> device.name == commsTargetName }
        if (toyTank == null){
            onScreenLog("'$commsTargetName' was not found among paired bluetooth devices")
        }

        bluetoothSocket = toyTank?.createRfcommSocketToServiceRecord(commsBTSPP)
        try{
            bluetoothSocket?.connect();
            onScreenLog("Connected to '$commsTargetName'")
        }
        catch (e: Exception) {
            onScreenLog("Failed to connect '$commsTargetName")
            onScreenLog(e.message!!)
        }
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
        const val REQUEST_ENABLE_BT = 1
    }
}