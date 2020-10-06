package com.suong99.gianphoidoandroid;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Random;

import pl.pawelkleczkowski.customgauge.CustomGauge;

public class BieuDoActivity extends AppCompatActivity {

    private Socket mSocket;
    private Date date;
    private SimpleDateFormat simpleDateFormat;
    private String strDate;
    Toolbar toolbarBieuDo;
    GraphView graphLine;
    CustomGauge gaugeDoAm;
    TextView txtPhanTramDoAm;
    TextView txtDate;
    TextView txtNhietDo;
    private Handler mHandler = new Handler();
    private LineGraphSeries<DataPoint> series;
    private double lastXPoint = 0;
    private Random rnd = new Random();
    private int temperature = 0, humidity = 0;

    {
        try {
            mSocket = IO.socket("http://192.168.43.234:3000/");
//            mSocket = IO.socket("https://gianphoidoonline.herokuapp.com/");
        } catch (URISyntaxException e) {
            System.out.println("Loi: '} catch (URISyntaxException e) {' + " + e.toString());
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR1)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bieu_do);
        AnhXa();

        mSocket.on("server_gui_DHT11", nhandataDHT11);

        // Nhiet Do
        series = new LineGraphSeries<>(new DataPoint[]{
                new DataPoint(0, 0),
                new DataPoint(0, 0),
                new DataPoint(0, 0)
        });
        graphLine.addSeries(series);

        graphLine.getViewport().setMinX(0);
//        graphLine.getViewport().setMaxX(10);
        graphLine.getViewport().setXAxisBoundsManual(true);

        graphLine.getViewport().setMinY(0);
        graphLine.getViewport().setMaxY(80);
        graphLine.getViewport().setYAxisBoundsManual(true);

        graphLine.getViewport().setScrollable(true);
        graphLine.getViewport().setScrollableY(true);
        graphLine.getViewport().setScalable(true);
        graphLine.getViewport().setScalableY(true);

//        addRandomDataPoint();

        // Do am
        gaugeDoAm.setEndValue(1000);
        gaugeDoAm.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
        gaugeDoAm.setValue(500);
        txtPhanTramDoAm.setText(gaugeDoAm.getValue() + "%");
    }

    private void addRandomDataPoint() {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                lastXPoint++;
                series.appendData(new DataPoint(lastXPoint, rnd.nextInt(80)), false, 100);
                addRandomDataPoint();
            }
        }, 1000);
    }

    private void AnhXa() {
        toolbarBieuDo = findViewById(R.id.toolbarBieuDo);
        setSupportActionBar(toolbarBieuDo);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        toolbarBieuDo.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        graphLine = findViewById(R.id.graphNhietDo);
        gaugeDoAm = findViewById(R.id.gaugeDoAm);
        txtPhanTramDoAm = findViewById(R.id.txtPhanTramDoAm);
        txtDate = findViewById(R.id.txtDate);
        txtNhietDo = findViewById(R.id.txtNhietDo);

    }

    private final Emitter.Listener nhandataDHT11 = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    JSONObject data = (JSONObject) args[0];
                    try {
                        temperature = Integer.parseInt(data.getString("Temperature"));
                        humidity = Integer.parseInt(data.getString("Humidity"));

                        date = new Date();
                        simpleDateFormat = new SimpleDateFormat("dd/MM/yyyy - HH:mm:ss");
                        strDate = simpleDateFormat.format(date);
                        System.out.println("Current date in Date Format: " + strDate);
                        txtNhietDo.setText("Nhiệt độ : " + temperature + (char) 186 + "C");
                        txtDate.setText(strDate);
                        System.out.println(temperature + "*C - " + humidity + "%");

                        graphLine.setBackgroundColor(getResources().getColor(android.R.color.holo_green_light));
                        series.appendData(new DataPoint(date.getHours(), temperature), true, 100);
                        gaugeDoAm.setValue(humidity * 10);
                        txtPhanTramDoAm.setText(data.getString("Humidity") + "%");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    };
}
