package ryan.media.iavideo;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import ryan.media.iavideo.databinding.ActivityMainBinding;
import ryan.utils.PermissionUtil;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'iavideo' library on application startup.
    static {
        System.loadLibrary("iavideo");
    }

    private ActivityMainBinding binding;
    private ActivityResultLauncher launcher =
            registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), new ActivityResultCallback<ActivityResult>() {
                @Override
                public void onActivityResult(ActivityResult result) {

                }
            });

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
    }

    /**
     * A native method that is implemented by the 'iavideo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    protected void onResume() {
        super.onResume();
        Intent intent = new Intent();
        intent.setAction("ijk_video_player");
        launcher.launch(intent);
    }
}