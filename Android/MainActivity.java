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
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class MainActivity extends AppCompatActivity {
    private enum LightType {ON, OFF};

    //UI
    private Button outModeButton;
    private Button inModeButton;
    private Button connectButton;
    private EditText ip_edit;
    private TextView luxText;
    private TextView statusText;

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
        luxText = (TextView)findViewById(R.id.luxText);
        statusText = (TextView)findViewById(R.id.statusText);

        standardLux = 100;
        lightType = LightType.OFF;

        Button.OnClickListener onClickListener = new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                isClick = true;
                switch (v.getId()) {
                    case R.id.connect :
                        connect();
                        break;
                }
            }
        };

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
                            currentLux = (char)dis.read();

                            if(currentLux > 0) {
                                Log.w("서버에서 받아온 온도 값", ""+currentLux);
                                luxText.setText(String.valueOf(currentLux));

                                byte[] data = null;
                                //서버로 내보내기 위한 출력 스트림 뚫음
                                OutputStream os = socket.getOutputStream();
                                //출력 스트림에 데이터 씀
                                os.write(0x01);
                                //보냄
                                os.flush();

                                if (currentLux >= 37 || currentLux <= 39) {
                                    statusText.setText("지금 우리 멍뭉이는 정상 체온!");
                                } else if (currentLux < 37.5) {
                                    statusText.setText("멍뭉이의 몸이 차가워요!");
                                } else {
                                    statusText.setText("멍뭉이의 몸이 뜨거워요!");
                                }
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
