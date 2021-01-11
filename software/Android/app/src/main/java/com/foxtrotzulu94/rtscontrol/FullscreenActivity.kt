package com.foxtrotzulu94.rtscontrol

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.text.method.ScrollingMovementMethod
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.nio.charset.Charset
import java.util.*


/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
class FullscreenActivity : AppCompatActivity() {
    private lateinit var fullscreenContent: LinearLayout
    private lateinit var fullscreenContentControls: LinearLayout
    private lateinit var logWindow: TextView
    private lateinit var cmdInput : EditText
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
        while(bluetoothSocket == null) { Thread.sleep(5000) }
        val socket = bluetoothSocket!!

        while (isCommsThreadRunning){
            if (isPressed){
                if (commsMsg == null) { continue }
                Log.d("RTS", "Send command '$commsMsg'")

                try{
                    socket.outputStream.write((commsMsg?.trimEnd() + "\r\n").toByteArray())
                }
                catch (e : Exception){
                    Log.e("RTS", "An exception occurred sending messages", e)
                }

                // TODO: do better message handling
                // Stop is probably not the only message we want to send once...
                if (commsMsg?.endsWith("STOP") == true
                    || commsMsg?.endsWith("HOLA") == true){
                    isPressed = false
                }
            }

            // Send messages only every 100ms
            // Also, avoid starving other threads
            Thread.sleep(100)
        }
    })

    private val commsReceiverThread = Thread(Runnable {
        while(bluetoothSocket == null) { Thread.sleep(5000) }
        val socket = bluetoothSocket!!

        val buffer = ByteArray(1024)
        var len: Int
        while (isCommsThreadRunning){
            try {
                len = socket.inputStream.read(buffer)
                val data = buffer.copyOf(len)
                val message = data.toString(Charset.defaultCharset()).trimEnd()
                if (message.isNotEmpty()) {
                    onScreenLog("REPLY: $message")
                }
            } catch (e: Exception) {
                onScreenLog("An exception occurred receiving messages")
//                try {
//                    socket.close()
//                } catch (ignored: Exception) { }
//                bluetoothSocket = null
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
        cmdInput  = findViewById(R.id.input_cmd)

        stopButton = findViewById(R.id.btn_stp)
        forwardButton = findViewById(R.id.btn_forward)
        leftButton = findViewById(R.id.btn_left)
        rightButton = findViewById(R.id.btn_right)
        backwardsButton = findViewById(R.id.btn_rev)

        val ackBtn : Button = findViewById(R.id.btn_ack)
        ackBtn.setOnTouchListener(fun(v: View, event: MotionEvent): Boolean {
            if (event.action == MotionEvent.ACTION_DOWN) {
                this.commsMsg = "RTSHOLA"
                isPressed = true
                onScreenLog("SEND: ${this.commsMsg}")
            }

            return v?.onTouchEvent(event) ?: true
        })

        cmdInput.setOnEditorActionListener(fun(v, actionId, event): Boolean{
            if (actionId != EditorInfo.IME_ACTION_DONE){
                return false
            }

            // get the text, clear it and send it
            var cmd = cmdInput.text.toString()
            val isSend =  cmd.startsWith("SEND: ")
            if(isSend) { cmd = cmd.removePrefix("SEND: ") }

            var type = ""
            if (cmd.startsWith(commsHeader) || isSend){
                isPressed = false
                bluetoothSocket?.outputStream?.write((cmd.trimEnd() + "\r\n").toByteArray())
                commsMsg = null
                type = "SEND: "
            }

            onScreenLog(type + cmd)
            cmdInput.text.clear()

            val imm: InputMethodManager = applicationContext.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
            imm.hideSoftInputFromWindow(v.windowToken, 0)

            return true
        })

        // setup callbacks
        val buttons = arrayOf(stopButton, forwardButton, leftButton, rightButton, backwardsButton)
        for (b in buttons){
            b.setOnTouchListener { v, event -> this.moveButtonOnTouchListener(v, event) }
        }

        logWindow.movementMethod = ScrollingMovementMethod()

        // Title bar? Not needed
        supportActionBar?.hide()
    }

    override fun onStart() {
        super.onStart()
        // Should be a background thread
        commsInitHandler.postDelayed(Runnable { this.setupBluetooth() }, 3000)
        onScreenLog("System started")
    }

    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)
    }

    private fun onScreenLog(msg : String){
        val trimmed = msg.trimEnd()
        if (trimmed.isEmpty()){ return }

        logBuffer.append(trimmed)
        if ( !trimmed.endsWith('\n')
            || !trimmed.endsWith('\r')) {
            logBuffer.append('\n')
        }

        this.runOnUiThread(fun() {
            logWindow.text = logBuffer.toString()
            //val scrollAmount = logWindow.layout.getLineTop(logWindow.lineCount) - logWindow.height
            //logWindow.scrollTo(0, scrollAmount)
        })
    }

    private fun setupBluetooth(){
        if (bluetoothSocket != null) { return }

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

            // Send a friendly hello (informal handshake)
            bluetoothSocket?.outputStream?.write("RTSHOLA".toByteArray())
            bluetoothSocket?.outputStream?.write('\r'.toInt())
            bluetoothSocket?.outputStream?.write('\n'.toInt())

            // Get a reply
            val responseBuffer = ByteArray(5)
            bluetoothSocket?.inputStream?.read(responseBuffer)
            val response = responseBuffer.toString(Charset.defaultCharset())
            if (response.startsWith("ACK")){
                onScreenLog("$commsTargetName ACK'ed back")
            }

            // Start the comms thread
            this.isCommsThreadRunning = true
            commsReceiverThread.start()
            commsSenderThread.start()
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
                // TODO: maybe map the buttons to the messages?
                if (button.text == "STOP"){
                    commsMsg = "RTSSTOP"
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