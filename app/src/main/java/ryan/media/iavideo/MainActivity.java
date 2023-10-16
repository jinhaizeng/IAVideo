package ryan.media.iavideo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;

import ryan.media.iavideo.databinding.ActivityMainBinding;
import ryan.utils.PermissionUtil;
import ryan.utils.Constant;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'iavideo' library on application startup.
    static {
        System.loadLibrary("iavideo");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        PermissionUtil.requestMyPermissions(this);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
        tv.bringToFront();

        VideoPlayer ijkVideo = binding.ijkVideo;
//        ijkVideo.setVideoListener(this);
        String path = "/sdcard/DCIM/Camera";
        String fileName = "DCIM/Camera/share_f032ca40160c3d22598faf07728e8f51.mp4";
        String input = new File(
                Environment.getExternalStorageDirectory(),
                fileName
        ).getAbsolutePath();
        ijkVideo.setPath(input);
        try {
            ijkVideo.load();
        } catch (IOException e) {
            Toast.makeText(this,"播放失败",Toast.LENGTH_SHORT);
            e.printStackTrace();
        }
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e(Constant.TAG, "askjdh");
                Toast.makeText(MainActivity.this,"开始",Toast.LENGTH_SHORT);
                ijkVideo.start();
            }
        });
    }

    /**
     * A native method that is implemented by the 'iavideo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}