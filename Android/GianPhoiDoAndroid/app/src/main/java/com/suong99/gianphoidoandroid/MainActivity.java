package com.suong99.gianphoidoandroid;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.animation.AnimatorSet;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;
import com.wangjie.rapidfloatingactionbutton.RapidFloatingActionButton;
import com.wangjie.rapidfloatingactionbutton.RapidFloatingActionContent;
import com.wangjie.rapidfloatingactionbutton.RapidFloatingActionLayout;
import com.wangjie.rapidfloatingactionbutton.listener.OnRapidFloatingActionListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_CODE = 100;
    MediaPlayer mediaPlayer;

    private Socket mSocket;
    RadioButton rdThuVao;
    RadioButton rdDayRa;
    TextView txtTrangThaiPhoiDo;
    TextView txtThongBaoTrangThaiDK;
    Switch switchDieuKhien;
    Toolbar toolbarMain;
    RapidFloatingActionButton fabVoice;
    TextView txtNhietDoMain;
    TextView txtDoAmMain;

    {
        try {
            mSocket = IO.socket("http://192.168.43.234:3000/");
//            mSocket = IO.socket("https://gianphoidoonline.herokuapp.com/");
        } catch (URISyntaxException e) {
            System.out.println("Loi: '} catch (URISyntaxException e) {' + " + e.toString());
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        getMenuInflater().inflate(R.menu.menu, menu);

        return super.onCreateOptionsMenu(menu);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        AnhXa();
        mSocket.connect();
        mSocket.on("server_gui_trang_thai_DC", JSON_STATUSDC);
        mSocket.on("server_gui_DHT11", nhandataDHT11);
//        mSocket.on("server_gui_STATECT", nhandataSTATECT);

        txtThongBaoTrangThaiDK.setText("Hệ thống tự vận hành.");
        txtThongBaoTrangThaiDK.setTextColor(Color.rgb(16, 201, 22));

        rdDayRa.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mSocket.emit("android_gui_dieuKhienDC", "d");
            }
        });

        rdThuVao.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mSocket.emit("android_gui_dieuKhienDC", "t");
            }
        });

        switchDieuKhien.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    mSocket.emit("android_gui_Control", true);
                    txtThongBaoTrangThaiDK.setText("Bạn có thể điều khiển được!");
                    txtThongBaoTrangThaiDK.setTextColor(Color.rgb(191, 142, 4));
                } else {
                    mSocket.emit("android_gui_Control", false);
                    txtThongBaoTrangThaiDK.setText("Hệ thống tự vận hành.");
                    txtThongBaoTrangThaiDK.setTextColor(Color.rgb(16, 201, 22));
                }
            }
        });

        toolbarMain.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {

                if (item.getItemId() == R.id.menuBieuDo) {
                    Intent ManHinhBieuDo = new Intent(MainActivity.this, BieuDoActivity.class);
                    startActivity(ManHinhBieuDo);
                }

                return false;
            }
        });

        fabVoice.setOnRapidFloatingActionListener(new OnRapidFloatingActionListener() {
            @Override
            public void onRFABClick() {
                System.out.println("onRFABClick");
                //Create an Intent with “RecognizerIntent.ACTION_RECOGNIZE_SPEECH” action//

                {
                    Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

                    try {

                        //Start the Activity and wait for the response//

                        startActivityForResult(intent, REQUEST_CODE);
                    } catch (ActivityNotFoundException a) {

                    }

                }
            }

            @Override
            public void toggleContent() {

            }

            @Override
            public void expandContent() {

            }

            @Override
            public void collapseContent() {

            }

            @Override
            public RapidFloatingActionLayout obtainRFALayout() {
                return null;
            }

            @Override
            public RapidFloatingActionButton obtainRFAButton() {
                return null;
            }

            @Override
            public RapidFloatingActionContent obtainRFAContent() {
                return null;
            }

            @Override
            public void onExpandAnimator(AnimatorSet animatorSet) {

            }

            @Override
            public void onCollapseAnimator(AnimatorSet animatorSet) {

            }
        });

    }

    private void AnhXa() {
        rdThuVao = findViewById(R.id.rdThuVao);
        rdDayRa = findViewById(R.id.rdDayRa);
        txtTrangThaiPhoiDo = findViewById(R.id.txtTrangThaiPhoiDo);
        txtThongBaoTrangThaiDK = findViewById(R.id.txtThongBaoTrangThaiDK);
        switchDieuKhien = findViewById(R.id.switchDieuKhien);
        toolbarMain = findViewById(R.id.toolbarMain);
        toolbarMain.inflateMenu(R.menu.menu);
        fabVoice = findViewById(R.id.fabVoice);
        txtNhietDoMain = findViewById(R.id.txtNhietDoMain);
        txtDoAmMain = findViewById(R.id.txtDoAmMain);
    }

    private Emitter.Listener JSON_STATUSDC = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        JSONObject data = (JSONObject) args[0];
                        int valueRainSensor = data.getInt("valueRainSensor");
                        int valueCongTac1 = data.getInt("valueCongTac1");
                        int valueCongTac2 = data.getInt("valueCongTac2");
                        int voice = data.getInt("voice");
                        System.out.println(valueRainSensor + "-" + valueCongTac1 + "-" + valueCongTac2 + "-" + voice);
                        if (valueRainSensor == 1 && valueCongTac1 == 1 && valueCongTac2 == 0 && voice == 0) {
                            rdThuVao.setChecked(false);
                            rdDayRa.setChecked(true);
                            txtTrangThaiPhoiDo.setText("Đang phơi quần áo.");
                            txtTrangThaiPhoiDo.setTextColor(Color.rgb(16, 201, 22));
                        } else if (valueRainSensor == 0 && valueCongTac1 == 0 && valueCongTac2 == 1 && voice == 0) {
                            txtTrangThaiPhoiDo.setText("Đã lấy quần áo vào nhà.");
                            txtTrangThaiPhoiDo.setTextColor(Color.RED);
                            rdThuVao.setChecked(true);
                            rdDayRa.setChecked(false);
                        } else if (valueRainSensor == 1 && valueCongTac1 == 1 && valueCongTac2 == 0 && voice == 1) {
                            rdThuVao.setChecked(false);
                            rdDayRa.setChecked(true);
                            txtTrangThaiPhoiDo.setText("Đang phơi quần áo ngoài trời.");
                            txtTrangThaiPhoiDo.setTextColor(Color.rgb(16, 201, 22));
                            mediaPlayer = MediaPlayer.create(MainActivity.this, R.raw.phoido);
                            mediaPlayer.start();
                        } else if (valueRainSensor == 0 && valueCongTac1 == 0 && valueCongTac2 == 1 && voice == 1) {
                            txtTrangThaiPhoiDo.setText("Đã lấy quần áo vào nhà.");
                            txtTrangThaiPhoiDo.setTextColor(Color.RED);
                            rdThuVao.setChecked(true);
                            rdDayRa.setChecked(false);
                            mediaPlayer = MediaPlayer.create(MainActivity.this, R.raw.laydo);
                            mediaPlayer.start();
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    };

    private Emitter.Listener nhandataDHT11 = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    JSONObject data = (JSONObject) args[0];
                    try {
                        txtNhietDoMain.setText("Nhiệt độ: " + data.getString("Temperature") + (char) 186 + "C");
                        txtDoAmMain.setText("Độ ẩm: " + data.getString("Humidity") + "%");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    };

    private Emitter.Listener nhandataSTATECT = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    JSONObject data = (JSONObject) args[0];
                    try {
                        boolean stateCT = data.getBoolean("stateCT");
                        if (stateCT) {
                            txtThongBaoTrangThaiDK.setText("Bạn có thể điều khiển được!");
                            txtThongBaoTrangThaiDK.setTextColor(Color.rgb(191, 142, 4));
                        } else {
                            txtThongBaoTrangThaiDK.setText("Hệ thống tự vận hành.");
                            txtThongBaoTrangThaiDK.setTextColor(Color.rgb(16, 201, 22));
                        }

                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    };

    public void voice(View view) {
        //Create an Intent with “RecognizerIntent.ACTION_RECOGNIZE_SPEECH” action//

        {
            Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

            try {

                //Start the Activity and wait for the response//

                startActivityForResult(intent, REQUEST_CODE);
            } catch (ActivityNotFoundException a) {

            }

        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case REQUEST_CODE: {
                if (resultCode == RESULT_OK && null != data) {
                    ArrayList<String> result = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);

                    if (result.get(0).contains("lấy")) {
                        Toast.makeText(this, result.get(0), Toast.LENGTH_SHORT).show();
                        mSocket.emit("android_gui_dieuKhienDC", "T");
                    } else if (result.get(0).contains("phơi")) {
                        Toast.makeText(this, result.get(0), Toast.LENGTH_SHORT).show();
                        mSocket.emit("android_gui_dieuKhienDC", "D");
                    }
                }
                break;
            }

        }
    }
}