package ryan.media.iavideo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;

import ryan.media.iavideo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'iavideo' library on application startup.
    static {
        System.loadLibrary("iavideo");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
        VideoPlayer ijkVideo = binding.ijkVideo;
//        ijkVideo.setVideoListener(this);
        ijkVideo.setPath("http://ipfs.ztgame.com.cn/QmRRGU4aUZEqJsHxKzBb1ns97GHw45eCRRZFe6Eu8GCmZ4.m3u8");
        try {
            ijkVideo.load();
        } catch (IOException e) {
            Toast.makeText(this,"播放失败",Toast.LENGTH_SHORT);
            e.printStackTrace();
        }
    }

    /**
     * A native method that is implemented by the 'iavideo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}