package ryan.media.info;

import static ryan.utils.Constant.TAG;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

import ryan.media.iavideo.IAVideoPlayer;

public class GetMediaInfoActivity extends AppCompatActivity {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        super.onResume();
        new Thread(new Runnable() {
            @Override
            public void run() {
                IAVideoPlayer codec =new IAVideoPlayer();
                String fileName = "output_audio.aac";
                String input = new File(
                        Environment.getExternalStorageDirectory(),
                        fileName
                ).getAbsolutePath();
                Log.d(TAG, "input: " + input);
                codec.getMediaInfo(input);
            }
        }).start();
    }
}
