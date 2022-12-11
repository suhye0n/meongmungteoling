package com.example.mt;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Handler;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;

import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

//서버
import androidx.appcompat.app.AppCompatActivity;

import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.io.IOException;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

//import org.w3c.dom.Text;


public class MainActivity extends AppCompatActivity {

    private enum LightType {ON, OFF};

    //UI
    private Button outModeButton;
    private Button inModeButton;
    private Button connectButton;   // ip 받아오는 버튼
    private  Button quitButton; //연결 종료 버튼
    private EditText ip_edit;   // ip 입력하는 값
    private TextView luxText;  // 서버로부터 받는 데이터
    private TextView logText;
    private ImageView lightPowerImage;

    // 서버
    // private String html = "";
    private Handler mHandler;
    private Socket socket;
    private DataOutputStream dos;
    private DataInputStream dis;

    private String ip = "192.168.0.6";
    private int port = 8080;


    private int currentLux;
    private int standardLux;
    private LightType lightType;
    private boolean isClick = false;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        connectButton = findViewById(R.id.connect);
        ip_edit = findViewById(R.id.editIp);
        quitButton = findViewById(R.id.quit);
        luxText = findViewById(R.id.luxText);
        logText = findViewById(R.id.logText);
        outModeButton = findViewById(R.id.outModeButton);
        inModeButton = findViewById(R.id.inModeButton);

        //currentLux = 50;
        standardLux = 100;
        //luxText.setText(luxText.getText());
        lightType = LightType.OFF;

        Button.OnClickListener onClickListener = new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                isClick = true;
                switch (v.getId()) {
                    case R.id.outModeButton:
                        if(lightType == LightType.ON) {
                            lightType = LightType.OFF;
                            logText.setText("전원 OFF");
                        }
                        else {
                            logText.setText("이미 불이 꺼져 있습니다");
                        }

                        break;

                    case R.id.inModeButton :
                        if(lightType == LightType.ON) {
                            if(currentLux >= standardLux) {
                                lightType = LightType.OFF;
                                logText.setText("불 켜지 않아도 충분히 밝습니다");
                            }
                            else if(currentLux < standardLux) {
                                logText.setText("이미 불이 켜져 있습니다");
                            }
                        }
                        else {
                            if(currentLux < standardLux) {
                                lightType = LightType.ON;
                                logText.setText("집콕모드! 전원 ON");

                            }
                            else if(currentLux >= standardLux) {
                                logText.setText("이미 불이 꺼져 있습니다");
                            }
                        }
                        break;

                    case R.id.connect :
                        connect();
                        break;

                    case R.id.quit :
                        try {
                            socket.close();
                        } catch (Exception e) {

                        }
                }
            }
        };

        outModeButton.setOnClickListener(onClickListener);
        inModeButton.setOnClickListener(onClickListener);
        connectButton.setOnClickListener(onClickListener);

    }

    public void connect() {
        mHandler = new Handler();
        Log.w("connect", "연결하는 중");


        Thread checkUpdate = new Thread() {
            public void run() {
                String newIp = String.valueOf(ip_edit.getText());
                int i = 0;

                try {
                    socket = new Socket(newIp, port);
                    Log.w("서버 접속됨", "서버 접속됨");
                } catch (IOException e1) {
                    Log.w("서버접속못함", "서버접속못합");
                    e1.printStackTrace();
                }
                Log.w("edit 넘어가야 할 값 :", "안드로이드에서 서버로 연결요청");

                try {
                    dos = new DataOutputStream(socket.getOutputStream());
                    dis = new DataInputStream(socket.getInputStream());
                    dos.writeUTF("안드로이드에서 서버로 연결요청");
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.w("버퍼", "버퍼생성 잘못됨");
                }
                Log.w("버퍼", "버퍼생성 잘됨");

                while(i < 5) {

                    try {

                        while(true) {

                            Log.d("서버 데이터 읽기", "서버 데이터");
                            currentLux = (int)dis.read();

                            if(currentLux > 0) {
                                Log.w("서버에서 받아온 값", ""+currentLux);
                                luxText.setText(String.valueOf(currentLux));
                            }
                            else if(currentLux == -1) {
                                Log.w("버퍼의 끝까지 도달", "버퍼 종료");
                                socket.close();
                            }
                        }
                    } catch (Exception e) {

                    }
                    i += 1;
                    try {
                        if(isClick == true) {
                            dos.writeUTF(""+lightType);
                            Thread.sleep(5000);
                            isClick = false;
                        }
                        else {
                            dos.writeUTF(""+lightType);
                        }

                    } catch (Exception e) {

                    }

                }
                try {
                    socket.close();
                } catch (Exception e) {

                }

            }
        };

        checkUpdate.start();
    }
}