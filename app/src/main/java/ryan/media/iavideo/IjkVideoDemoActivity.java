package ryan.media.iavideo;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.IOException;

import ryan.media.iavideo.databinding.IjkVideoDemoBinding;
import ryan.utils.Constant;
import ryan.utils.PermissionUtil;

public class IjkVideoDemoActivity extends AppCompatActivity {

    private IjkVideoDemoBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        PermissionUtil.requestMyPermissions(this);

        binding = IjkVideoDemoBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
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
                Toast.makeText(IjkVideoDemoActivity.this,"开始",Toast.LENGTH_SHORT);
                ijkVideo.start();
            }
        });
    }
}