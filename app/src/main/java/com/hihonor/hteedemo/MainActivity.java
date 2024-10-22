/*
 * Copyright (c) Honor Device Co., Ltd. 2024-2024. All rights reserved.
 */

package com.hihonor.hteedemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.content.pm.PackageManager;
import android.graphics.YuvImage;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.Manifest;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import com.hihonor.android.security.securecamera.SecureCameraConfig;
import com.hihonor.android.security.securecamera.SecureCamera;

/**
 * HTEE kit CA demo
 *
 * @since 2024-09-14
 */
public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private static final int CA_CMD_LOAD_SEC_CAMERA_TA = 0;
    private static final int CA_CMD_GET_SEC_IMAGE = 1;
    private static final int CA_CMD_UNLOAD_SEC_CAMERA_TA = 2;
    private static final int MAGIC_SDK_8_0 = 38;

    private static final int SUPPORT_WIDTH = 640;
    private static final int SUPPORT_HEIGHT = 480;
    private static final int SUPPORT_FPS = 30;
    private static final int BUFFER_SIZE = 1024;
    private static final int QUALITY_VALUE = 70;
    private static final int FAILED = -1;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private ImageView imageView;
    private final String TAName = "25aaa1b6-190a-3396-8a5c-6d8ba36417a8.sec";
    private SecureCamera mSecureCamera;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        copyAssetsFile2Phone(MainActivity.this, TAName);
        String TAPath = MainActivity.this.getFilesDir().getAbsolutePath() + File.separator + MainActivity.this.TAName;
        final TextView tv = findViewById(R.id.sample_text);
        imageView = findViewById(R.id.imageView);

        findViewById(R.id.get_ta_version).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                tv.setText(stringFromTA(TAPath));
            }
        });

        // 使用安全camera能力，需要申请相机权限
        if (ContextCompat.checkSelfPermission(MainActivity.this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        }
        findViewById(R.id.open_service).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // 打开安全camera
                startService();
            }
        });
        findViewById(R.id.close_service).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // 关闭安全camera
                stopService();
            }
        });
    }

    // 调用 htee CA-TA 通信基础功能，获取TA版本号，依赖TA文件
    private native String stringFromTA(String path);

    static void copyAssetsFile2Phone(MainActivity activity, String fileName) {
        try {
            InputStream inputStream = activity.getAssets().open(fileName);
            File file = new File(activity.getFilesDir().getAbsolutePath() + File.separator + fileName);
            if (!file.exists() || file.length() == 0) {
                FileOutputStream fos = new FileOutputStream(file);
                int len;
                byte[] buffer = new byte[BUFFER_SIZE];
                while ((len = inputStream.read(buffer)) != FAILED) {
                    fos.write(buffer, 0, len);
                }
                fos.flush();
                inputStream.close();
                fos.close();
            }
        } catch (IOException e) {
            Log.d(TAG, "copyAssetsFile2Phone failed");
        }
    }

    private native String openTestTA(String path);

    private native byte[] sendCmdToTestTA(int cmd, int data);

    private native String closeTestTA();

    // 开启安全camera服务
    private void startService() {

        // Maigic OS 8.0及以上版本支持安全 Camera 能力
        if (Build.MANUFACTURER.equals("HONOR") && com.hihonor.android.os.Build.VERSION.MAGIC_SDK_INT >= MAGIC_SDK_8_0) {
            mSecureCamera = SecureCamera.getSecureCamera(this);
        } else {
            Toast.makeText(MainActivity.this, "no support Secure Camera!", Toast.LENGTH_LONG).show();
            return;
        }
        if (mSecureCamera == null) {
            Toast.makeText(MainActivity.this, "no support Secure Camera!", Toast.LENGTH_LONG).show();
            return;
        }

        // 开发者需指定业务ta的uuid
        String taUuid = "25aaa1b6190a33968a5c6d8ba36417a8";

        // 开发者需指定业务TA文件路径
        String TAPath = MainActivity.this.getFilesDir().getAbsolutePath() + File.separator + MainActivity.this.TAName;

        // 加载业务TA。
        openTestTA(TAPath);

        // 配置SecureCamera关键参数
        SecureCameraConfig config = new SecureCameraConfig.Builder()
                .setTaUuid(taUuid.getBytes(StandardCharsets.UTF_8)) // set the uuid of ta.
                .setTaType(1) // support ta or not. Supported value: 1
                .setCameraType(0) // means camera is front or rear. 0: front, 1: rear. Supported value: 1
                .setImageFormat(ImageFormat.NV21) // imageFormat securecamera image format information. Supported value: 17
                .setFps(SUPPORT_FPS) // securecamera fps information. Supported value: 30
                .setWidth(SUPPORT_WIDTH) // securecamera width information. Supported value: 640
                .setHeight(SUPPORT_HEIGHT) // securecamera height information. Supported value: 480
                .create();
        SecureCamera.InfoCallback callback = new SecureCamera.InfoCallback() {
            @Override
            public void onReceiveCameraInfo(int ret, int state, byte[] data) {
                Log.d(TAG, "onReceiveCameraInfo, ret = " + ret + ", state = " + state);

                // 与向业务TA获发送指令，从安全 Camera TA 中获取生成的安全图像，并借助HTEE可信传输通道回传给业务CA
                byte[] imageData = sendCmdToTestTA(CA_CMD_GET_SEC_IMAGE, 0);

                // 将NV21格式图片，以质量70压缩成Jpeg，并得到JPEG数据流
                YuvImage image = new YuvImage(imageData, ImageFormat.NV21, SUPPORT_HEIGHT, SUPPORT_WIDTH, null);
                ByteArrayOutputStream outputSteam = new ByteArrayOutputStream();
                image.compressToJpeg(new Rect(0, 0, image.getWidth(), image.getHeight()), QUALITY_VALUE, outputSteam);

                // 从outputSteam得到byte数据
                byte[] jpegData = outputSteam.toByteArray();
                InputStream inputStream = new ByteArrayInputStream(jpegData);
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inSampleSize = 1;
                Bitmap bmp = BitmapFactory.decodeStream(inputStream, null, options);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(TAG, " setImageBitmap");
                        imageView.setImageBitmap(bmp);
                    }
                });
            }
        };
        int ret = FAILED;
        if (mSecureCamera != null) {
            // 开启安全camera服务
            Log.i(TAG, "startService begin");
            ret = mSecureCamera.startService(config, callback, null);
        }
        if (ret == 0) {
            // 开启安全camera服务后，向业务TA发送消息，与安全Camera TA建立连接。
            sendCmdToTestTA(CA_CMD_LOAD_SEC_CAMERA_TA, 0);
            Log.i(TAG, " send cmd: 0 To Test TA end");
        }
    }

    // 关闭安全camera服务
    private void stopService() {
        if (mSecureCamera != null) {
            mSecureCamera.stopService();
        }

        // 关闭业务TA与安全Camera TA的连接。
        sendCmdToTestTA(CA_CMD_UNLOAD_SEC_CAMERA_TA, 0);

        // 关闭业务TA。
        closeTestTA();
        Log.e(TAG, " closeTestTA end");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                imageView.setImageBitmap(null);
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 1: {
                // 如果用户不授权相机权限，grantResults就会是空的
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.e(TAG, " user agree");
                    Toast.makeText(this, "user agree camera permission!", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(this, "user don't agree camera permission!", Toast.LENGTH_LONG).show();
                }
                break;
            }
            default:
                break;
        }
    }

    @Override
    public void onResume() {
        Log.e(TAG, "onResume");
        super.onResume();
    }

    @Override
    public void onPause() {
        Log.e(TAG, "onPause");
        stopService(); // activity pause时，关闭安全camera服务
        super.onPause();
    }

    @Override
    public void onStop() {
        Log.e(TAG, "onStop");
        stopService(); // activity stop时，关闭安全camera服务
        super.onStop();
    }

    @Override
    public void onDestroy() {
        Log.e(TAG, "onDestroy");
        stopService(); // activity 销毁时，关闭安全camera服务
        super.onDestroy();
    }
}